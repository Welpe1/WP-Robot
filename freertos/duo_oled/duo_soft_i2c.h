#ifndef __DUO_SOFT_I2C_H
#define __DUO_SORT_I2C_H

#include <stdint.h>
#include "duo_gpio.h"

struct Soft_I2C_Base{
    uint8_t id;
    struct gpio_regs *GPIOx;
    volatile uint32_t sda;
    volatile uint32_t scl;
};


void sda_w(struct Soft_I2C_Base *i2c,uint8_t bit);
void scl_w(struct Soft_I2C_Base *i2c,uint8_t bit);
uint8_t sda_r(struct Soft_I2C_Base *i2c);
void i2c_start(struct Soft_I2C_Base *i2c);
void i2c_stop(struct Soft_I2C_Base *i2c);
uint8_t i2c_wait_ack(struct Soft_I2C_Base *i2c);
void i2c_w_ack(struct Soft_I2C_Base *i2c,uint8_t ack);
void i2c_w_byte(struct Soft_I2C_Base *i2c,uint8_t byte);
uint8_t i2c_r_byte(struct Soft_I2C_Base *i2c,uint8_t ack);

#endif
