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

#define GPIO2 0x03022000
#define GPIO_SWPORTA_DR 0x000
#define GPIO_SWPORTA_DDR 0x004

void duo_led_control(int enable)
{
	*(uint32_t*)(GPIO2 | GPIO_SWPORTA_DDR) = 1 << 24;

	if (enable) {
		*(uint32_t*)(GPIO2 | GPIO_SWPORTA_DR) = 1 << 24;
	} else {
		*(uint32_t*)(GPIO2 | GPIO_SWPORTA_DR) = 0;
	}
}


QueueHandle_t queue_handle;

void prvQueueISR(void);
void mailbox_task(void);

/* mailbox parameters */
volatile struct mailbox_set_register *mbox_reg;
volatile struct mailbox_done_register *mbox_done_reg;
volatile unsigned long *mailbox_context; // mailbox buffer context is 64 Bytess

DEFINE_CVI_SPINLOCK(mailbox_lock, SPIN_MBOX);

void main_cvirtos(void)
{
	printf("create cvi task\n");
	/**
	 * 通过 request_irq 注册中断处理函数
	 * 在 rtos_irq_handler 中通过 mailbox 机制访问读取共享内存，
	 * 中断号 mailbox_irq 通过 platform_get_irq_byname 获取
	 */
	request_irq(MBOX_INT_C906_2ND, prvQueueISR, 0, "mailbox", (void *)0);
	
	queue_handle = xQueueCreate(30, sizeof(cmdqu_t));
	if(queue_handle != NULL)
	{
		xTaskCreate(mailbox_task,"mailbox_task",configMINIMAL_STACK_SIZE, NULL,tskIDLE_PRIORITY + 5, NULL);		//创建任务

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
			case CMD_TEST_A:
				//send to C906B
				from_linux.cmd_id = CMD_TEST_A;
				from_linux.param_ptr = 0x12345678;
				from_linux.resv.valid.rtos_valid = 1;	//标记该mailbox已经被rtos使用
				from_linux.resv.valid.linux_valid = 0;	//标记该mailbox未被linux使用
				goto send_label;
			case CMD_TEST_B:
				break;
			case CMD_TEST_C:
				from_linux.cmd_id = CMD_TEST_C;
				from_linux.param_ptr = 0x55aa;
				from_linux.resv.valid.rtos_valid = 1;
				from_linux.resv.valid.linux_valid = 0;
				// printf("recv cmd(%d) from C906B...send [0x%x] to C906B\n", from_linux.cmd_id, from_linux.param_ptr);
				goto send_label;
			case CMD_DUO_LED:
				from_linux.cmd_id = CMD_DUO_LED;
				if (from_linux.param_ptr == DUO_LED_ON) {
					duo_led_control(1);
				} else {
					duo_led_control(0);
				}
				from_linux.param_ptr = DUO_LED_DONE;
				from_linux.resv.valid.rtos_valid = 1;
				from_linux.resv.valid.linux_valid = 0;
				printf("recv cmd(%d) from C906B...send [0x%x] to C906B\n", from_linux.cmd_id, from_linux.param_ptr);
				goto send_label;

			case CMD_TEST:
				from_linux.cmd_id = CMD_TEST;
				
				//from_linux.param_ptr = 0xBB;
				from_linux.resv.valid.rtos_valid = 1;
				from_linux.resv.valid.linux_valid = 0;
				printf("recv cmd(%d) from C906B...send [0x%x] to C906B\n", from_linux.cmd_id, from_linux.param_ptr);
				goto send_label;
			default:
send_label:
				/* used to send command to linux*/
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

						// cmdq->resv.valid.rtos_valid = 1;
						// /**
						//  * cmdq指向本rtos所使用的mailbox
						//  * 通知linux本rtos系统已经接受到了linux发来的信息并正在处理
						//  * 
						//  */

						// //将本rtos的信息打包成一个32位整数（4字节）
						// *ptr = ((cmdq->ip_id << 0) | (cmdq->cmd_id << 8) | (cmdq->block << 15) |
						// 		(cmdq->resv.valid.linux_valid << 16) |
						// 		(cmdq->resv.valid.rtos_valid << 24));
						// to_linux->param_ptr = cmdq->param_ptr;

						from_linux.resv.valid.rtos_valid = 1;
						*ptr = ((from_linux.ip_id << 0) | (from_linux.cmd_id << 8) | (from_linux.block << 15) |
								(from_linux.resv.valid.linux_valid << 16) |
								(from_linux.resv.valid.rtos_valid << 24));

						//小核向大核传递数据
						to_linux->param_ptr = from_linux.param_ptr;

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
				break;
		}
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