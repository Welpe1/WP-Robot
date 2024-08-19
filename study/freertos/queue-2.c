/**
 * stm32f103c8t6 FreeRTOS例程
 * 创建两个任务用于传输大块数据
 * 
 */

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sys.h"

static char data_buf[100];      //全局变量，共享内存
TaskHandle_t vSenderTask_Handle,vReceiverTask_Handle;
QueueHandle_t xQueue_Handle;

static void vSenderTask(void *args);
static void vReceiverTask(void *args);

int main( void )
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();
	uart_init(115200);
	
	xQueue_Handle = xQueueCreate(1, sizeof(char *));		//创建长度为1，数据大小为4字节的队列
	if(xQueue_Handle != NULL){
		xTaskCreate(vSenderTask,"Sender",600,NULL,1,&vSenderTask_Handle);
		xTaskCreate(vReceiverTask,"Receiver",600,NULL,1,&vReceiverTask_Handle);
		vTaskStartScheduler();
	} 
	return 0;
} 

static void vSenderTask(void *args)
{
	BaseType_t ret;
	static int cnt = 0;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(10UL);
	char *buffer;
	while(1)
	{
		sprintf(data_buf, "send %d\r\n", cnt++);
		buffer = data_buf; 
		xQueueReset(xQueue_Handle);		//发送前先清空队列
		ret = xQueueSendToBack(xQueue_Handle,&buffer,10); 
		if( ret != pdPASS )
		{
			printf( "Could not send to the queue.\r\n" );
		}
	}
} 

static void vReceiverTask(void *args)
{
	
	char *buffer;		
	const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
	BaseType_t ret;
	while (1)
	{

		/**
		 * 
		 * 若使用xQueueReceive，则接收任务的优先级可以>发送任务的，这是因为当接收不到数据时，xQueueReceive会
		 * 是接收任务阻塞，发送任务得以执行。每当发送任务发送一次数据时，接收任务立刻从阻塞态进入运行态，立刻执
		 * 行，读到数据后队列中数据清空，接收任务再次进入阻塞态
		 * 
		 * 若使用xQueuePeek，则两个任务的优先级必须一样，否则只会执行vReceiverTask任务，这是因为xQueuePeek
		 * 读到数据后并不会清空队列中的数据
		 * 
		 */
		ret = xQueuePeek(xQueue_Handle, &buffer, xTicksToWait);
		
		if( ret == pdPASS ){
			printf("rev: %s", buffer);

		} else{
			printf( "Could not receive from the queue.\r\n" );
		}
	}
}






