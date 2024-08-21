#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sys.h"

#include "oled.h"
#include "mpu6050.h"
#include "imu.h"

QueueHandle_t xQueue_Handle;
TaskHandle_t xOLED_Handle,xIMU_Handle,xPID_Handle;
static float databuf[3];

void vOLED_Task(void *args)
{
	float *revbuf;
	BaseType_t ret;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(10UL);
	while(1)
	{
		ret=xQueuePeek(xQueue_Handle,&revbuf,xTicksToWait);
		if( ret == pdPASS ){
			printf("oled task roll=%f\r\n",revbuf[0]);
			printf("oled task pitch=%f\r\n",revbuf[1]);
			printf("oled task yaw=%f\r\n",revbuf[2]);
			OLED_ShowFloatNum(1,1,revbuf[0]);
			OLED_ShowFloatNum(2,1,revbuf[1]);
			OLED_ShowFloatNum(3,1,revbuf[2]);
			//OLED_Show(1,1,revbuf);

		}
	}
}

void vIMU_Task(void *args)
{
	IMU Imu;
	BaseType_t ret;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(10UL);
	float *revbuf;
	while(1)
	{
		
		
		MPU6050_GetData(&Imu.Mpu.accX,&Imu.Mpu.accY,&Imu.Mpu.accZ,
		&Imu.Mpu.gyroX,&Imu.Mpu.gyroY,&Imu.Mpu.gyroZ);
		GetAngle(&Imu,0.01);
		
		xQueueReset(xQueue_Handle);		//清空队列
		databuf[0]=Imu.Angle.roll;
		databuf[1]=Imu.Angle.pitch;
		databuf[2]=Imu.Angle.yaw;
		revbuf=databuf;
		ret=xQueueSend(xQueue_Handle,&revbuf,xTicksToWait);
		if( ret != pdPASS )
		{
			printf( "Could not send to the queue.\r\n" );
		}
		
	}
	
}


void vPID_Task(void *args)
{
	float *revbuf;
	BaseType_t ret;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(10UL);
	while(1)
	{
		ret=xQueuePeek(xQueue_Handle,&revbuf,xTicksToWait);
		if( ret == pdPASS ){
			printf("pid task roll=%f\r\n",revbuf[0]);
			printf("pid task pitch=%f\r\n",revbuf[1]);
			printf("pid task yaw=%f\r\n",revbuf[2]);
			
		}
	}
	
}


int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();
	uart_init(115200);
	OLED_Init();
	MPU6050_Init();
	
	xQueue_Handle = xQueueCreate(1, sizeof(char *));		//创建长度为1，数据大小为4字节的队列
	if(xQueue_Handle != NULL){
		xTaskCreate(vOLED_Task,"oeld task",400,NULL,1,&xOLED_Handle);
		xTaskCreate(vIMU_Task,"imu task",400,NULL,1,&xIMU_Handle);
		xTaskCreate(vPID_Task,"pid task",400,NULL,1,&xPID_Handle);
		vTaskStartScheduler();
	} 
	return 0;
	
	

}









cccx c c  