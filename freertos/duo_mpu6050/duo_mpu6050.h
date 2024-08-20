#ifndef __DUO_MPU6050_H
#define __DUO_MPU6050_H

#include <stdio.h>
#include "top_reg.h"

typedef struct mpu_data{
	int accx;
	int accy;
	int accz;
	int gyrox;
	int gyroy;
	int gyroz;
};


#define MPU6050_I2C	I2C1
#define MPU6050_ADDR	0xD0

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

uint8_t mpu6050_w_byte(uint8_t reg,uint8_t data);
uint8_t mpu6050_r_byte(uint8_t reg);
void mpu6050_init(void);
void mpu6050_get_data(struct mpu_data *mpu);



#endif