#include <stdio.h>
#include "arch_sleep.h"
#include "duo_mpu6050.h"


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

void mpu6050_get_data(struct mpu_data *mpu)
{
	mpu->accx=(mpu6050_r_byte(MPU6050_ACCEL_XOUT_H) << 8)|(mpu6050_r_byte(MPU6050_ACCEL_XOUT_L));
	mpu->accy=(mpu6050_r_byte(MPU6050_ACCEL_YOUT_H) << 8)|(mpu6050_r_byte(MPU6050_ACCEL_YOUT_L));
	mpu->accz=(mpu6050_r_byte(MPU6050_ACCEL_ZOUT_H) << 8)|(mpu6050_r_byte(MPU6050_ACCEL_ZOUT_L));
	mpu->gyrox=(mpu6050_r_byte(MPU6050_GYRO_XOUT_H) << 8)|(mpu6050_r_byte(MPU6050_GYRO_XOUT_L));
	mpu->gyroy=(mpu6050_r_byte(MPU6050_GYRO_YOUT_H) << 8)|(mpu6050_r_byte(MPU6050_GYRO_YOUT_L));
	mpu->gyroz=(mpu6050_r_byte(MPU6050_GYRO_ZOUT_H) << 8)|(mpu6050_r_byte(MPU6050_GYRO_ZOUT_L));
}


const float RtD = 57.2957795f;
const float DtR = 0.0174532925f;
const float Gyro_G = 0.06103512f;
const float Gyro_Gr = 0.0010632f;

float Q_rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
 
    x2 = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );
    y = * ( float * ) &i;
    y = y * ( threehalfs - ( x2 * y * y ) );//1st iteration （第一次牛顿迭代）

 
    return y;
}


