#include <stdio.h>
#include "arch_sleep.h"
#include "duo_mpu6050.h"
#include "duo_reg.h"

/**
 * 使用软件i2c
 * 自定义i2c5
 * sda gpa23
 * scl gpa24
 * 
 */


void i2c5_init(void)
{	
	*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 23;       //设置为输出
	*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 24;

}

void i2c5_w_sda(uint8_t enable)
{
	*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 23;       //设置为输出
	if (enable) {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) |= 1 << 23;
	} else {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) &=~(1 << 23);
	}
	
	arch_usleep(10);
}

uint8_t i2c5_r_sda(void)
{
    *(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) &= ~(1 << 24);  
    uint32_t ret = *(uint32_t*)(GPIOA_BASE | GPIO_EXT_PORTA); 
    
    arch_usleep(10);
    *(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 24;
     
    return (ret >> 24) & 1;   
    

}

void i2c5_w_scl(uint8_t enable)
{
	*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 23;

	if (enable) {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) |= 1 << 23;
	} else {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) &=~(1 << 23);
	}
	
	arch_usleep(10);
}

void i2c5_start(void)
{
	i2c5_w_sda(1);
	i2c5_w_scl(1);
	i2c5_w_sda(0);
	i2c5_w_scl(0);

}

void i2c5_stop(void)
{
	i2c5_w_sda(0);
	i2c5_w_scl(1);
	i2c5_w_sda(1);

}


uint8_t i2c5_wait_ack(void)
{
	uint8_t errtime=0;
	i2c5_w_sda(1);
	i2c5_w_scl(1);
	while(i2c5_r_sda())
	{
		errtime++;
		if(errtime>50)
		{
			i2c5_stop();
			return 1;	//无应答
		}
	}
	i2c5_w_scl(0);
	return 0;	//有应答
}

void i2c5_w_ack(uint8_t ack)
{
	i2c5_w_scl(0);
	if(ack) i2c5_w_sda(0);
	else i2c5_w_sda(1);
	i2c5_w_scl(1);
	i2c5_w_scl(0);

}

void i2c5_w_byte(uint8_t byte)
{
	uint8_t i=0;
	for(i=0;i<8;i++)
	{
		i2c5_w_sda(!!(byte & (0x80>>i)));
		i2c5_w_scl(1);
		i2c5_w_scl(0);

	}

}

uint8_t i2c5_r_byte(uint8_t ack)
{
	uint8_t i=0,ret=0;
    i2c5_w_sda(1);
	for(i=0;i<8;i++)
	{
		i2c5_w_scl(1);
		if(i2c5_r_sda()) {ret|=(0x80>>i);}
		i2c5_w_scl(0);

	}
    i2c5_w_ack(ack);
	return ret;
}

uint8_t mpu6050_w_byte(uint8_t reg,uint8_t data)
{
	i2c5_start();
    i2c5_w_byte(MPU6050_ADDR);
    i2c5_wait_ack();
    i2c5_w_byte(reg);
    i2c5_wait_ack();
    i2c5_w_byte(data);
    i2c5_wait_ack();
    i2c5_stop();

}

uint8_t mpu6050_r_byte(uint8_t reg)
{
	uint8_t data;
    i2c5_start();
    i2c5_w_byte(MPU6050_ADDR);
    i2c5_wait_ack();
    i2c5_w_byte(reg);
    i2c5_wait_ack();

    i2c5_start();
    i2c5_w_byte(MPU6050_ADDR|0x01);
    i2c5_wait_ack();
    data=i2c5_r_byte(1);
    i2c5_stop();

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

uint8_t mpu6050_get_id(void)
{
	return mpu6050_r_byte(MPU6050_WHO_AM_I);
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


