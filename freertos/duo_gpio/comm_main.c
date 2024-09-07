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
#include "duo_gpio.h"

void gpio_task()
{
	while(1)
	{
        printf("gpio task running\r\n");

		vTaskDelay(10);

	}

}




void prvGpioISR(void)
{
    gpio_clear_it(GPIOA);
    printf("gpio isr 1111111111111111111111111111111\r\n");

    printf("GPIOA->isr=%d\r\n",(GPIOA->isr>>14)&1);

}



void main_cvirtos(void)
{
	printf("create cvi task\n");

    gpio_irq_init(GPIOA,14);

    request_irq(GPIO0_INTR_FLAG,prvGpioISR,0,"gpio_isr",(void*)0);
    xTaskCreate(gpio_task, "check_task", 1024 * 8, NULL, 1, NULL);
    vTaskStartScheduler();
    printf("cvi task end\n");
	while(1);
}

