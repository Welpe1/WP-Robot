#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mmio.h"
#include "delay.h"
#include "printf.h"
#include "rtos_cmdqu.h"
#include "cvi_mailbox.h"
#include "intr_conf.h"
#include "top_reg.h"
#include "memmap.h"
#include "comm.h"
#include "cvi_spinlock.h"

#include "duo_reg.h"
#include "duo_oled.h"
#include "duo_uart.h"
#include "duo_mpu6050.h"
#include "duo_check.h"
#include "duo_pwm.h"
#include "duo_gpio.h"

extern const uint8_t BMP1[];
extern const uint8_t BMP2[];
extern const uint8_t BMP3[];
extern const uint8_t BMP4[];

TaskHandle_t check_task_handle;
TaskHandle_t oled_task_handle;


void oled_task()
{
    struct pwm_config PWM10;
    pwm_init(&PWM10,10);
  while(1) 
  {
    //printf("oled_task running\r\n");
		
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

    uint16_t i=0;
    for(i=0;i<180;i++)
    {
        set_angle(&PWM10,i);
        printf("angle=%d\r\n",i);
        
    }
    i=0;
    




  }

}


void check_task()
{
    uint32_t ret=0;
    check_system_init();
    //iic0
    check_system_push(PINMUX_BASE + PINMUX_GP0,3);
    check_system_push(PINMUX_BASE + PINMUX_GP1,3);
    //pwm10
    check_system_push(PINMUX_BASE + PINMUX_GP2,7);

	while(1)
	{
		printf("check_task running\r\n");
		if(!check_system_run())
		{
            check_system_destroy();
            printf("soft i2c0 get ready\r\n");
            
            oled_init();
            xTaskCreate(oled_task, "oled_task", 1024 * 8, NULL, 1, NULL);
            vTaskDelete(NULL);
		  
		}
		vTaskDelay(50);

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
    gpio_irq_init(GPIOA,14,2);
	request_irq(GPIO0_INTR_FLAG,prvGpioISR,0,"gpio_isr",(void*)0);
	xTaskCreate(check_task, "check_task", 1024 * 8, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1);
}
