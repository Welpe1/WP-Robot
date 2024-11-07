#ifndef __DUO_PWM_H
#define __DUO_PWM_H

#include <stdint.h>
#include "duo_reg.h"

struct pwm_regs {
    volatile uint32_t lperiod_period[8];
	// volatile uint32_t hlperiod0; /* 0x00 PWM0 low -level shooting number (低电平数)*/
	// volatile uint32_t period0; /* 0x04 PWM0 square wave cycle shooting (周期数)*/
	// volatile uint32_t hlperiod1; /* 0x08 PWM1 low -level shooting number */
	// volatile uint32_t period1; /* 0x0C PWM1 square wave cycle shooting */
    // volatile uint32_t hlperiod2; /* 0x10 PWM2 low -level shooting number */
	// volatile uint32_t period2; /* 0x14 PWM2 square wave cycle shooting */
    // volatile uint32_t hlperiod3; /* 0x18 PWM3 low -level shooting number */
	// volatile uint32_t period3; /* 0x1C PWM3 square wave cycle shooting */
	volatile uint32_t reserved0[8];
    volatile uint32_t polarity; /* 0x40 PWM mode settings */
    volatile uint32_t pwmstart; /* 0x44 Make PWM output */
    volatile uint32_t pwmdone; /* 0x48 PWM Ending State */
    volatile uint32_t pwmupdate; /* 0x4c Dynamic loading PWM cycle parameter */
    volatile uint32_t pcount[4];
    // volatile uint32_t pcount0; /* 0x50 Set pwm0 pulse number */
    // volatile uint32_t pcount1; /* 0x54 Set pwm1 pulse number */
    // volatile uint32_t pcount2; /* 0x58 Set pwm2 pulse number */
    // volatile uint32_t pcount3; /* 0x5c Set pwm3 pulse number */
    volatile uint32_t pulsecount[4];
    // volatile uint32_t pulsecount0; /* 0x60 PWM0 output pulse meter status */
    // volatile uint32_t pulsecount1; /* 0x64 PWM1 output pulse meter status */
    // volatile uint32_t pulsecount2; /* 0x68 PWM2 output pulse meter status */
    // volatile uint32_t pulsecount3; /* 0x6c PWM3 output pulse meter status */
    volatile uint32_t reserved1[4];
    volatile uint32_t shiftcount[4];
    // volatile uint32_t shiftcount0; /* 0x80 Synchronous mode PWM0 initial difference */
    // volatile uint32_t shiftcount1; /* 0x84 Synchronous mode PWM1 initial difference */
    // volatile uint32_t shiftcount2; /* 0x88 Synchronous mode PWM2 initial difference */
    // volatile uint32_t shiftcount3; /* 0x8c Synchronous mode PWM3 initial difference */
    volatile uint32_t shiftstart; /* 0x90 PWM synchronization mode enable */
    volatile uint32_t reserved2[15];
    volatile uint32_t pwm_oe; /* 0xd0 PWM IO output enable */
};

struct pwm_config{
    struct pwm_regs* pwm_base;
    uint8_t id;
};

void pwm_init(struct pwm_config *PWMx,uint8_t id);
void set_angle(struct pwm_config *PWMx,uint16_t angle);

#endif
