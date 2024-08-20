#ifndef __DUO_MPU6050_H
#define __DUO_MPU6050_H

#include "duo_reg.h"
#include <stdio.h>

uint8_t mpu6050_w_byte(uint8_t reg,uint8_t data);
uint8_t mpu6050_r_byte(uint8_t reg);
void mpu6050_init(void);
void mpu6050_get_data(mpu_data *mpu);


#endif
