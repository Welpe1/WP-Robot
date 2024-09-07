#include <stdio.h>
#include "arch_sleep.h"
#include "duo_gpio.h"

void gpio_w_pin(struct gpio_regs *GPIOx,uint8_t pin,uint8_t bit)
{
    GPIOx->ddr |= (1 << pin);
    if(bit) GPIOx->dr |= (1 << pin);
    else GPIOx->dr &=~(1 << pin);

}

uint8_t gpio_r_pin(struct gpio_regs *GPIOx,uint8_t pin)
{
    GPIOx->ddr &=~(1 << pin);
    return (GPIOx->epr >> pin) & 1;

}

void gpio_irq_init(struct gpio_regs *GPIOx,uint8_t pin)
{
    GPIOx->itlr|= (1 << pin);   //边沿触发
    //GPIOx->itlr&=~(1 << pin);  //电平触发
    GPIOx->ipr|= (1 << pin);    //上升沿触发
    //GPIOx->ipr&=~(1 << pin);    //下降沿触发
    GPIOx->per=0xFFFFFFFF;      //清除中断标志位
    GPIOx->ier|= (1 << pin);    //使能中断

}

//清除中断标志位
void gpio_clear_it(struct gpio_regs *GPIOx)
{
    GPIOx->per=0xFFFFFFFF;
}