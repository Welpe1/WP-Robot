/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mmio.h"
#include "delay.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "cvi_mailbox.h"
#include "intr_conf.h"
#include "top_reg.h"
#include "memmap.h"
#include "comm.h"
#include "cvi_spinlock.h"

/* Milk-V Duo */
#include "duo_reg.h"
#include "duo_oled.h"
#include "duo_uart.h"
#include "duo_check.h"
#include "duo_mpu6050.h"
#include "duo_gpio.h"
#include "duo_pwm.h"

#include "arch_helpers.h"


DEFINE_CVI_SPINLOCK(mailbox_lock, SPIN_MBOX);


QueueHandle_t queue_handle;

void prvQueueISR(void);
void mailbox_task(void);
void servo_task(void);

/* mailbox parameters */
volatile struct mailbox_set_register *mbox_reg;
volatile struct mailbox_done_register *mbox_done_reg;
volatile unsigned long *mailbox_context; // mailbox buffer context is 64 Bytess

volatile uint8_t *value=NULL;
uintptr_t addr;
uint16_t gFreertos_Data;
uint64_t gLinux_Data;
float offset_dead_block = 0.1;
uint8_t kp = 50;
uint8_t last_degree = 90;

void calculate_offset(int img_width, int img_height, int face_x, int face_y, float* offset_x, float* offset_y) {  
    *offset_x = (float)(face_x / (float)img_width - 0.5) * 2;  
    *offset_y = (float)(face_y / (float)img_height - 0.5) * 2;  
}

uint8_t servo_control(float offset_x)
{
	if(abs(offset_x) < offset_dead_block)
       offset_x = 0;
    float delta_degree = offset_x * kp;
    float next_degree = last_degree + delta_degree;
    if(next_degree < 0)
        next_degree = 0;
    else if(next_degree > 180)
        next_degree = 180;
    return (int)next_degree;
}

void main_cvirtos(void)
{
	printf("create cvi task\n");
	value = (uint8_t *)malloc(4 * sizeof(uint8_t));  
	/**
	 * 通过 request_irq 注册中断处理函数
	 * 在 rtos_irq_handler 中通过 mailbox 机制访问读取共享内存，
	 * 中断号 mailbox_irq 通过 platform_get_irq_byname 获取
	 */
	request_irq(MBOX_INT_C906_2ND, prvQueueISR, 0, "mailbox", (void *)0);
	
	queue_handle = xQueueCreate(30, sizeof(cmdqu_t));
	if(queue_handle != NULL)
	{
		xTaskCreate(mailbox_task,"mailbox_task",configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5,NULL);		//创建任务
        xTaskCreate(servo_task, "servo_task", 1024 * 8, NULL, 1, NULL);
	}
	
    vTaskStartScheduler();
    printf("cvi task end\n");
	while(1);
}

void mailbox_task(void)
{
	cmdqu_t from_linux;		//接收来自linux的数据
	cmdqu_t *cmdq;
	cmdqu_t *to_linux;		//向linux发送数据
	int ret = 0;
	int flags;		//存储中断标志。
	int valid;		//检查mailbox的有效性

	unsigned int reg_base = MAILBOX_REG_BASE;

	cmdq = &from_linux;

	mbox_reg = (struct mailbox_set_register *) reg_base;
	mbox_done_reg = (struct mailbox_done_register *) (reg_base + 2);
	mailbox_context = (unsigned long *) (MAILBOX_REG_BUFF);

	cvi_spinlock_init();
	printf(" prvCmdQuRunTask_mailbox run\n");

	while(1) {
		xQueueReceive(queue_handle, &from_linux, portMAX_DELAY);
		switch (from_linux.cmd_id) {

            case CMD_TEST_B:
                printf("cmd_test_b\r\n");
                from_linux.cmd_id = CMD_TEST_B;
				from_linux.resv.valid.rtos_valid = 1;
				from_linux.resv.valid.linux_valid = 0;
				addr=(uintptr_t)from_linux.param_ptr;
                value=(volatile uint8_t*)addr;
	            printf("addr=%p\r\n",addr);
                printf("value_addr=%p\r\n",value);
				goto send_label;
	
			case CMD_TEST_C:
                printf("cmd_test_c\r\n");
				from_linux.cmd_id = CMD_TEST_C;
				from_linux.resv.valid.rtos_valid = 1;
				from_linux.resv.valid.linux_valid = 0;
				gLinux_Data=(volatile uint64_t)from_linux.param_ptr;
				printf("gLinux_Data=%d\n",gLinux_Data);
				goto send_label;

			default:
send_label:
				to_linux = (cmdqu_t *) mailbox_context;		//指向mailbox寄存器

				drv_spin_lock_irqsave(&mailbox_lock, flags);
				if (flags == MAILBOX_LOCK_FAILED) {
					printf("[%s][%d] drv_spin_lock_irqsave failed! ip_id = %d , cmd_id = %d\n" , cmdq->ip_id , cmdq->cmd_id);
					break;
				}

				for (valid = 0; valid < MAILBOX_MAX_NUM; valid++) {
					if (to_linux->resv.valid.linux_valid == 0 && to_linux->resv.valid.rtos_valid == 0) {
						/**
						 * 本rtos已经使用了一个mailbox（linux向rtos传递信息），需要找一个新的mailbox（rtos向linux返回信息）
						 * 遍历MAILBOX_MAX_NUM个mailbox，找到一个既没有被linux使用也没有被rtos使用的mailbox
						 * 
						 */
						// mailbox buffer context is 4 bytes write access
						int *ptr = (int *)to_linux;

						from_linux.resv.valid.rtos_valid = 1;
						*ptr = ((from_linux.ip_id << 0) | (from_linux.cmd_id << 8) | (from_linux.block << 15) |
								(from_linux.resv.valid.linux_valid << 16) |
								(from_linux.resv.valid.rtos_valid << 24));

						//小核向大核传递数据
						to_linux->param_ptr = gFreertos_Data;
						gFreertos_Data=0;

						// clear mailbox
						mbox_reg->cpu_mbox_set[SEND_TO_CPU1].cpu_mbox_int_clr.mbox_int_clr = (1 << valid);
						// trigger mailbox valid to rtos
						mbox_reg->cpu_mbox_en[SEND_TO_CPU1].mbox_info |= (1 << valid);
						mbox_reg->mbox_set.mbox_set = (1 << valid);
						break;
					}
					to_linux++;
				}
				drv_spin_unlock_irqrestore(&mailbox_lock, flags);
				if (valid >= MAILBOX_MAX_NUM) {
				    printf("No valid mailbox is available\n");
				    return -1;
				}
				gFreertos_Data=0;
				break;
		}
	}
}

void servo_task(void)
{
	uint8_t degree;
    struct pwm_config PWM2;
    pwm_init(&PWM2,2);

	while(1)
	{
		
		float offset_x, offset_y;  
		uint16_t face_x = gLinux_Data;
		uint16_t face_y = (gLinux_Data>>16);
		printf("face_x=%d\n",face_x);
		printf("face_y=%d\n",face_y);
		calculate_offset(1280, 720, face_x, face_y, &offset_x, &offset_y);
		degree=servo_control(offset_x);
		printf("degree=%d\n",degree);
		set_angle(&PWM2,degree);
        vTaskDelay(5);

	}
}

/**
 * 用于处理来自Linux系统的mailbox中断
 * 当Linux系统向RTOS（实时操作系统）发送命令时，会触发这个中断
 * 
 */
void prvQueueISR(void)
{
	printf("prvQueueISR\n");
	unsigned char set_val;		//存储mailbox中断设置寄存器的值 (set_val!=0，表示有中断发生)
	unsigned char valid_val;		//检查特定的mailbox条目是否有效
	int i;
	cmdqu_t *cmdq;
	BaseType_t YieldRequired = pdFALSE;

	set_val = mbox_reg->cpu_mbox_set[RECEIVE_CPU].cpu_mbox_int_int.mbox_int;

	if (set_val) {//如果set_val不为0，表示有中断发生
		for(i = 0; i < MAILBOX_MAX_NUM; i++) {		//遍历所有mailbox
			valid_val = set_val  & (1 << i);		//找到有效的mailbox

			if (valid_val) {
				cmdqu_t rtos_cmdq;
				cmdq = (cmdqu_t *)(mailbox_context) + i;		//计算cmdq指针，使其指向当前有效的mailbox条目

			
				/* mailbox buffer context is send from linux, clear mailbox interrupt */
				mbox_reg->cpu_mbox_set[RECEIVE_CPU].cpu_mbox_int_clr.mbox_int_clr = valid_val;		//清除mailbox中断标志
				// need to disable enable bit
				mbox_reg->cpu_mbox_en[RECEIVE_CPU].mbox_info &= ~valid_val;

				// copy cmdq context (8 bytes) to buffer ASAP
				*((unsigned long *) &rtos_cmdq) = *((unsigned long *)cmdq);		//将mailbox条目的内容复制到rtos_cmdq中
				/* need to clear mailbox interrupt before clear mailbox buffer */
				*((unsigned long*) cmdq) = 0;		//清除mailbox条目的内容（将其设置为0）

				/* mailbox buffer context is send from linux*/
				if (rtos_cmdq.resv.valid.linux_valid == 1) {	//检查rtos_cmdq的linux_valid标志，如果为1，则表示命令来自Linux系统，需要将其发送到RTOS的任务队列中

					xQueueSendFromISR(queue_handle, &rtos_cmdq, &YieldRequired);

					portYIELD_FROM_ISR(YieldRequired);
				} 
			}
		}
	}
}


