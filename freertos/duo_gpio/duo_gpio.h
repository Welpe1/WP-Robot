#ifndef __DUO_GPIO_H
#define __DUO_GPIO_H

#include "duo_reg.h"

struct gpio_regs
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

#define GPIOA   ((struct gpio_regs*)GPIOA_BASE)
#define GPIOB   ((struct gpio_regs*)GPIOB_BASE)
#define GPIOC   ((struct gpio_regs*)GPIOC_BASE)
#define GPIOD   ((struct gpio_regs*)GPIOD_BASE)


void gpio_w_pin(struct gpio_regs *GPIOx,uint8_t pin,uint8_t bit);
uint8_t gpio_r_pin(struct gpio_regs *GPIOx,uint8_t pin);
void gpio_irq_init(struct gpio_regs *GPIOx,uint8_t pin);
void gpio_clear_it(struct gpio_regs *GPIOx);

#endif
