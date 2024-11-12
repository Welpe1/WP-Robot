#include <stdio.h>
#include <stdint.h>
#include "duo_mpu6050.h"
#include "duo_soft_i2c.h"
#include "hal_dw_i2c.h"

/**
 * 使用软件i2c
 * 自定义i2c5
 * sda gpa14
 * scl gpa15
 * 
 */

struct Soft_I2C_Base i2c5;
const float RtD = 57.2957795f;
const float DtR = 0.0174532925f;
const float Gyro_G = 0.06103512f;
const float Gyro_Gr = 0.0010632f;

struct Quaternion QNum = {1, 0, 0, 0};
volatile struct A_Temp GyroIntegError = {0};
volatile struct A_Temp Gravity, Acc, Gyro, AccGravity;

uint8_t mpu6050_w_byte(uint8_t reg,uint8_t data)
{
    i2c_start(&i2c5);
    i2c_w_byte(&i2c5,MPU6050_ADDR);
    i2c_w_byte(&i2c5,reg);
    i2c_w_byte(&i2c5,data);
	i2c_stop(&i2c5);
}

uint8_t mpu6050_r_byte(uint8_t reg)
{
    uint8_t data=0;
    i2c_start(&i2c5);
    i2c_w_byte(&i2c5,MPU6050_ADDR);
    i2c_w_byte(&i2c5,reg);
    i2c_start(&i2c5);
    i2c_w_byte(&i2c5,MPU6050_ADDR|0x01);
    data=i2c_r_byte(&i2c5,1);
	i2c_stop(&i2c5);
    return data;
}



void mpu6050_init(void)
{
    i2c5.id=0;
    i2c5.GPIOx=GPIOA;
    i2c5.sda=14;
    i2c5.scl=15;

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



float Q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
    x2 = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );
    y = * ( float * ) &i;
    y = y * ( threehalfs - ( x2 * y * y ) );
    return y;
}


void GetAngle(struct mpu_data *Imu,float dt)
{    
    static  float KpDef = 0.5f ;
    static  float KiDef = 0.0001f;
    Gravity.x = 2*(QNum.q1 * QNum.q3 - QNum.q0 * QNum.q2);                                
    Gravity.y = 2*(QNum.q0 * QNum.q1 + QNum.q2 * QNum.q3);                          
    Gravity.z = 1-2*(QNum.q1 * QNum.q1 + QNum.q2 * QNum.q2);
    
    float NormAcc=Q_rsqrt(SQUA(Imu->accx)+ SQUA(Imu->accy) +SQUA(Imu->accz));
    Acc.x = Imu->accx * NormAcc;
    Acc.y = Imu->accy * NormAcc;
    Acc.z = Imu->accz * NormAcc;
    
    AccGravity.x = Acc.y * Gravity.z - Acc.z * Gravity.y;
    AccGravity.y = Acc.z * Gravity.x - Acc.x * Gravity.z;
    AccGravity.z = Acc.x * Gravity.y - Acc.y * Gravity.x;
    
    //加速度积分补偿角速度的补偿值
    GyroIntegError.x += AccGravity.x * KiDef;
    GyroIntegError.y += AccGravity.y * KiDef;
    GyroIntegError.z += AccGravity.z * KiDef;

    Gyro.x = Imu->gyrox * Gyro_Gr + KpDef * AccGravity.x  +  GyroIntegError.x;
    Gyro.y = Imu->gyroy * Gyro_Gr + KpDef * AccGravity.y  +  GyroIntegError.y;
    Gyro.z = Imu->gyroz * Gyro_Gr + KpDef * AccGravity.z  +  GyroIntegError.z;
    
    float q0_t, q1_t, q2_t, q3_t;
    q0_t = dt * (-QNum.q1 * Gyro.x - QNum.q2 * Gyro.y - QNum.q3 * Gyro.z) * 0.5f;
    q1_t = dt * (QNum.q0 * Gyro.x - QNum.q3 * Gyro.y + QNum.q2 * Gyro.z) * 0.5f;
    q2_t = dt * (QNum.q3 * Gyro.x + QNum.q0 * Gyro.y - QNum.q1 * Gyro.z) * 0.5f;
    q3_t = dt * (-QNum.q2 * Gyro.x + QNum.q1 * Gyro.y + QNum.q0 * Gyro.z) * 0.5f;
    
    QNum.q0 += q0_t;
    QNum.q1 += q1_t;
    QNum.q2 += q2_t;
    QNum.q3 += q3_t;

    float NormQ=Q_rsqrt(SQUA(QNum.q0) + SQUA(QNum.q1) + SQUA(QNum.q2) + SQUA(QNum.q3));
    QNum.q0 *= NormQ;
    QNum.q1 *= NormQ;
    QNum.q2 *= NormQ;
    QNum.q3 *= NormQ;
        
    float yaw_G = Imu->gyroz * Gyro_G;
    if((yaw_G > 1.2f) || (yaw_G < -1.2f))
    {
        Imu->angle.yaw  += yaw_G * dt;            
    }
        
    Imu->angle.pitch = asin(2 * QNum.q0 * QNum.q2 - 2 * QNum.q1 * QNum.q3) * RtD;
    Imu->angle.roll = atan2(2 * QNum.q2 * QNum.q3 + 2 * QNum.q0 * QNum.q1, 1 - 2 * QNum.q1 *QNum.q1 - 2 * QNum.q2 * QNum.q2) * RtD;

}