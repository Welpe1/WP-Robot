#include <stdio.h>
#include "arch_sleep.h"
#include "duo_gpio.h"

void gpio_w_pin(struct GPIO_REGS *GPIOx,uint8_t pin,uint8_t bit)
{
    GPIOx->ddr |= (1 << pin);
    if(bit) GPIOx->dr |= (1 << pin);
    else GPIOx->dr &=~(1 << pin);

}

uint8_t gpio_r_pin(struct GPIO_REGS *GPIOx,uint8_t pin)
{
    GPIOx->ddr &=~(1 << pin);
    return (GPIOx->epr >> pin) & 1;

}


/**
 * mode
 * 0：高电平触发
 * 1：低电平触发
 * 2：上升沿触发
 * 3：下降沿触发
 * 
 */
void gpio_irq_init(struct GPIO_REGS *GPIOx,uint8_t pin,uint8_t mode)
{
    switch(mode)
    {
        case 0:
        {
            GPIOx->itlr&=~(1 << pin);  //电平触发
            GPIOx->ipr|= (1 << pin);    //上升沿触发
            break;
        };
        case 1:
        {
            GPIOx->itlr&=~(1 << pin);  //电平触发
            GPIOx->ipr&=~(1 << pin);    //下降沿触发
            break;
        };
        case 2:
        {
            GPIOx->itlr|= (1 << pin);   //边沿触发
            GPIOx->ipr|= (1 << pin);    //上升沿触发
            break;
        };
        case 3:
        {
            GPIOx->itlr|= (1 << pin);   //边沿触发
            GPIOx->ipr&=~(1 << pin);    //下降沿触发
            break;
        };
    }
    // GPIOx->itlr|= (1 << pin);   //边沿触发
    // //GPIOx->itlr&=~(1 << pin);  //电平触发
    // GPIOx->ipr|= (1 << pin);    //上升沿触发
    // //GPIOx->ipr&=~(1 << pin);    //下降沿触发
    GPIOx->per=0xFFFFFFFF;      //清除中断标志位
    GPIOx->ier|= (1 << pin);    //使能中断

}

//清除中断标志位
void gpio_clear_it(struct GPIO_REGS *GPIOx)
{
    GPIOx->per=0xFFFFFFFF;
}



