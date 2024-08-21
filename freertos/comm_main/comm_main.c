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
#include "duo_mpu6050.h"
#include "duo_check.h"


// #define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif

extern const uint8_t BMP1[];
extern const uint8_t BMP2[];
extern const uint8_t BMP3[];
extern const uint8_t BMP4[];


DEFINE_CVI_SPINLOCK(mailbox_lock, SPIN_MBOX);

TaskHandle_t check_task_handle;
TaskHandle_t oled_task_handle;
TaskHandle_t imu_task_handle;


void oled_task()
{
  while(1) 
  {
    printf("oled_task running\r\n");
		
    oled_clear();
    oled_update();
    vTaskDelay(1);

    oled_full();
    oled_update();
    vTaskDelay(1);

    oled_show_image(0,0,128,64,BMP1);
    oled_update();
    vTaskDelay(1);
   
    oled_show_image(0,0,128,64,BMP2);
    oled_update();
    vTaskDelay(1);
 
    oled_show_image(0,0,128,64,BMP3);
    oled_update();
    vTaskDelay(1);

    oled_show_image(0,0,128,64,BMP4);
    oled_update();
    vTaskDelay(1);


  }

}


void check_task()
{
  uint32_t ret=0;
	while(1)
	{
    printf("check_task running\r\n");
    if(!check_i2c_init(500))
    {
      printf("soft i2c0 get ready\r\n");
      oled_init();
      vTaskPrioritySet(oled_task_handle,2);
      vTaskDelete(check_task_handle);

      
    }

	}

}

void imu_task()
{
	uint8_t data=0;
	struct mpu_data mpu;

	while(1) 
	{
		mpu6050_get_data(&mpu);
		printf("dataz=%d\r\n",mpu.accz);
		printf("id=%d\r\n",mpu6050_get_id());
		printf("gyrox=%d\r\n",mpu6050_get_accx());

		data++;
		

	}


}

void main_cvirtos(void)
{
	

	xTaskCreate(check_task, "check_task", 1024 * 8, NULL, 2, NULL);
	xTaskCreate(oled_task, "oled_task", 1024 * 8, NULL, 1, NULL);


	vTaskStartScheduler();

	while(1);
	}
