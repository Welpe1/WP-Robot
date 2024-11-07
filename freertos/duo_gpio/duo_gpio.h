#ifndef __DUO_GPIO_H
#define __DUO_GPIO_H

#include "duo_reg.h"
#include <stdint.h>

struct GPIO_REGS
{
    volatile uint32_t dr;   /*0x00 Port A data register*/
    volatile uint32_t ddr;  /*0x04 Port A data direction register*/
    volatile uint32_t reserved0[10];
    volatile uint32_t ier;  /*0x30 Interrupt enable register*/
    volatile uint32_t imr;  /*0x34 Interrupt mask register*/
    volatile uint32_t itlr; /*0x38 Interrupt level register*/
 	volatile uint32_t ipr;  /*0x3C Interrupt polarity register*/
    volatile uint32_t isr;  /*0x40 Interrupt status of Port A*/
    volatile uint32_t risr; /*0x44 Raw interrupt status of Port A*/
    volatile uint32_t der;  /*0x48 Debounce enable register*/
    volatile uint32_t per;  /*0x4C Port A clear interrupt register*/
    volatile uint32_t epr;  /*0x50 Port A external port register*/
    volatile uint32_t reserved1[4];
    volatile uint32_t lsr;  /*0x60*/
};



#define GPIOA   ((struct GPIO_REGS*)GPIOA_BASE)
#define GPIOB   ((struct GPIO_REGS*)GPIOB_BASE)
#define GPIOC   ((struct GPIO_REGS*)GPIOC_BASE)
#define GPIOD   ((struct GPIO_REGS*)GPIOD_BASE)


void gpio_w_pin(struct GPIO_REGS *GPIOx,uint8_t pin,uint8_t bit);
uint8_t gpio_r_pin(struct GPIO_REGS *GPIOx,uint8_t pin);
void gpio_irq_init(struct GPIO_REGS *GPIOx,uint8_t pin,uint8_t mode);
void gpio_clear_it(struct GPIO_REGS *GPIOx);

#endif
