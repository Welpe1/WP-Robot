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

static void vSenderTask(void *args);
static void vReceiverTask(void *args);

QueueHandle_t xQueue_Handle;

int main( void )
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();
	uart_init(115200);
	
	xQueue_Handle = xQueueCreate(1, sizeof(char *));		//创建长度为1，数据大小为4字节的队列
	if(xQueue_Handle != NULL){
		xTaskCreate(vSenderTask, "Sender", 600, NULL, 1, NULL);
		xTaskCreate( ReceiverTask, "Receiver", 600, NULL, 1,NULL);
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

		/**
		 * 要发送大块数据，可以定义一个大数组data_buf，然后将该数组的地址传入队列即可，此时
		 * 队列的长度必须为1
		 * 现在要传输的数据是data_buf的地址，而xQueueSendToBack函数的第二个参数指向
		 * 要传输的数据的指针，所以实际传入的是指向data_buf的地址的指针，即二级指针
		 * 
		 */
		ret = xQueueSendToBack(xQueue_Handle, &buffer,pdMS_TO_TICKS); 
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





// static void vSenderTask( void *pvParameters )
// {
// 	BaseType_t xStatus;
// 	static int cnt = 0;
// 	char *buffer;

// 	for( ;; )
// 	{
// 	sprintf(pcBuffer, "www.100ask.net Msg %d\r\n", cnt++);
// 	buffer = pcBuffer; 


// 	/**
// 	 * 队列的长度为1，第一次调用xQueueSendToBack函数后buffer的地址入队
// 	 * 此后再调用会报错（调用的xQueuePeek函数读完数据以后不会自动清空队列）-->队列里面不能再有新成员加入，有且仅有buffer的地址
// 	 * 这个错误是队列不能入队的错误，而不是数据不能发送的错误，队列中已有的数据依然可以发送
// 	 * 此后会一直将buffer里面的数值发送出去
// 	 * 
// 	 * 
// 	 */
// 	xStatus = xQueueSendToBack( xQueue, &buffer,5); 
	
// 	if( xStatus != pdPASS )
// 	{
// 		printf( "Could not send to the queue.\r\n" );
// 	}
// }
// } 

// static void vReceiverTask( void *pvParameters )
// {
// 	/* 读取队列时, 用这个变量来存放数据 */
// 	char *buffer;
// 	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );
// 	BaseType_t xStatus;
// 	/* 无限循环 */
// 	for( ;; )
// 	{
// 		/* 读队列
// 		* xQueue: 读哪个队列
// 		* &xReceivedStructure: 读到的数据复制到这个地址
// 		* xTicksToWait: 没有数据就阻塞一会
// 		*/
// 		xStatus = xQueuePeek( xQueue, &buffer, xTicksToWait); 
// 		if( xStatus == pdPASS ){
// 		/* 读到了数据 */
// 		printf("Get: %s", buffer);
	
// 		} else{
// 			/* 没读到数据 */
// 			printf( "Could not receive from the queue.\r\n" );
// 		}
// 		vTaskDelay(1000);
// 	}
// }
