#include "duo_reg.h"
#include <stdio.h>
#include "cvi_spinlock.h"
#include "arch_sleep.h"


uint8_t mpu6050_w_byte(uint8_t reg,uint8_t data)
{
	uint8_t count = 20,ret=0;  
    while (count--) {  
		ret=hal_i2c_write(MPU6050_I2C,MPU6050_ADDR, reg,1, &data,1);
        if (!ret) {
            break;  
        }  
        arch_usleep(5); // 假设需要微秒级的延时  
    }  
	return ret;		//0：成功 1：失败

}

uint8_t mpu6050_r_byte(uint8_t reg)
{
	uint8_t count = 20,ret=0,revbuf;
	 while (count--) {  
		ret=hal_i2c_read(MPU6050_I2C,MPU6050_ADDR,reg,1,&revbuf,1);
        if (!ret) {
            break;  
        }  
        arch_usleep(5); // 假设需要微秒级的延时  
    }  
	return revbuf;
}



void mpu6050_init(void)
{
	mpu6050_w_byte(MPU6050_PWR_MGMT_1, 0x01);
	mpu6050_w_byte(MPU6050_PWR_MGMT_2, 0x00);
	mpu6050_w_byte(MPU6050_SMPLRT_DIV, 0x09);
	mpu6050_w_byte(MPU6050_CONFIG, 0x06);
	mpu6050_w_byte(MPU6050_GYRO_CONFIG, 0x18);
	mpu6050_w_byte(MPU6050_ACCEL_CONFIG, 0x18);

}

void mpu6050_get_data(mpu_data *mpu)
{
	mpu->accx=(mpu6050_r_byte(MPU6050_ACCEL_XOUT_H) << 8)|(mpu6050_r_byte(MPU6050_ACCEL_XOUT_L));
	mpu->accy=(mpu6050_r_byte(MPU6050_ACCEL_YOUT_H) << 8)|(mpu6050_r_byte(MPU6050_ACCEL_YOUT_L));
	mpu->accz=(mpu6050_r_byte(MPU6050_ACCEL_ZOUT_H) << 8)|(mpu6050_r_byte(MPU6050_ACCEL_ZOUT_L));
	mpu->gyrox=(mpu6050_r_byte(MPU6050_GYRO_XOUT_H) << 8)|(mpu6050_r_byte(MPU6050_GYRO_XOUT_L));
	mpu->gyroy=(mpu6050_r_byte(MPU6050_GYRO_YOUT_H) << 8)|(mpu6050_r_byte(MPU6050_GYRO_YOUT_L));
	mpu->gyroz=(mpu6050_r_byte(MPU6050_GYRO_ZOUT_H) << 8)|(mpu6050_r_byte(MPU6050_GYRO_ZOUT_L));
}
