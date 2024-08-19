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
#include "milkv_duo_io.h"
#include "oled.h"

// #define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif


#include "hal_dw_i2c.h"



DEFINE_CVI_SPINLOCK(mailbox_lock, SPIN_MBOX);



void my_task_test()
{
  uint8_t data= 0,data_read;
  printf("111111111111111111111111111111111111111111111111111111111111111111111111111\r\n");
  hal_i2c_init(I2C0);
	printf("hal_i2c_init after\n");
  uint8_t *data_write =&data;
	
  for (;;) 
  {
	
	oled_full();
    oled_update();
  
		//i2c端口，从机地址，寄存器地址，7位从机地址，写入的数据，写1个数据
		hal_i2c_write(I2C0, 0x01, 0x28, 1, data_write, 1);
		hal_i2c_read(I2C0, 0x28, 0x17, 1, &data_read, 1);
    printf("test the RTOS: %d\r\n", data);
    vTaskDelay(100);
    data++;
  }

}


void main_cvirtos(void)
{
	printf("create cvi task\n");

	/**
	 * 通过 request_irq 注册中断处理函数
	 * 在 rtos_irq_handler 中通过 mailbox 机制访问读取共享内存，
	 * 中断号 mailbox_irq 通过 platform_get_irq_byname 获取
	 * 
	 * 
	 */
  xTaskCreate(my_task_test, "my_task", 1024 * 8, NULL, 1, NULL);

    vTaskStartScheduler();
    printf("cvi task end\n");
	while(1)
        ;
}

