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

TaskHandle_t check_task_handle;
TaskHandle_t imu_task_handle;

void imu_task()
{
    struct mpu_data mpu;
    while(1) 
    {

        printf("oled_task running\r\n");
        mpu6050_get_data(&mpu);
        GetAngle(&mpu,0.05);

        oled_clear();
        oled_show_floatnum(1,1,mpu.angle.roll,3,3,8);
        oled_update();
      

        // printf("MPU6050_PWR_MGMT_1=%d\r\n",mpu6050_r_byte(MPU6050_PWR_MGMT_1));
        // printf("MPU6050_PWR_MGMT_2=%d\r\n",mpu6050_r_byte(MPU6050_PWR_MGMT_2));
        // printf("MPU6050_SMPLRT_DIV=%d\r\n",mpu6050_r_byte(MPU6050_SMPLRT_DIV));
        // printf("MPU6050_CONFIG=%d\r\n",mpu6050_r_byte(MPU6050_CONFIG));
        // printf("MPU6050_GYRO_CONFIG=%d\r\n",mpu6050_r_byte(MPU6050_GYRO_CONFIG));
        // printf("MPU6050_ACCEL_CONFIG=%d\r\n",mpu6050_r_byte(MPU6050_ACCEL_CONFIG));
    
        vTaskDelay(10);


    }

}


void check_task()
{
    uint32_t ret=0;
	while(1)
	{
		printf("check_task running\r\n");



        if(!check_i2c_init(50))
		{
		  oled_init();
            mpu6050_init();
            xTaskCreate(imu_task, "imu_task", 1024 * 8, NULL, 1, NULL);
            vTaskDelete(NULL);
            
		  
		}
        vTaskDelay(50);




	}

}


void main_cvirtos(void)
{
	
	xTaskCreate(check_task, "check_task", 1024 * 8, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1);
}
