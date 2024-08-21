
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <wiringx.h>
#include "mpu6050.h"

#define MPU6050_DEV                 "/dev/i2c-1"
#define MPU6050_ADDR          0x68

int fd_mpu;

void MPU6050_Init(void)
{
     if(wiringXSetup("duo", NULL) == -1) {
        wiringXGC();
        return -1;
    }

    if ((fd_mpu = wiringXI2CSetup(MPU6050_DEV,MPU6050_ADDR)) <0) {
        printf("I2C Setup failed: %i\n", fd_mpu);
        return -1;
    }

    wiringXI2CWriteReg8(fd_mpu,MPU6050_PWR_MGMT_1,0x01);
    wiringXI2CWriteReg8(fd_mpu,MPU6050_PWR_MGMT_2, 0x00);
    wiringXI2CWriteReg8(fd_mpu,MPU6050_SMPLRT_DIV, 0x09);
    wiringXI2CWriteReg8(fd_mpu,MPU6050_CONFIG, 0x06);
    wiringXI2CWriteReg8(fd_mpu,MPU6050_GYRO_CONFIG, 0x18);
    wiringXI2CWriteReg8(fd_mpu,MPU6050_ACCEL_CONFIG, 0x18);

}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = wiringXI2CReadReg8(fd_mpu,MPU6050_ACCEL_XOUT_H);
	DataL = wiringXI2CReadReg8(fd_mpu,MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = wiringXI2CReadReg8(fd_mpu,MPU6050_ACCEL_YOUT_H);
	DataL = wiringXI2CReadReg8(fd_mpu,MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = wiringXI2CReadReg8(fd_mpu,MPU6050_ACCEL_ZOUT_H);
	DataL = wiringXI2CReadReg8(fd_mpu,MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_XOUT_H);
	DataL = wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_YOUT_H);
	DataL = wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_ZOUT_H);
	DataL = wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}

// void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
// 						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
// {


//     *AccX=wiringXI2CReadReg16(fd_mpu,MPU6050_ACCEL_XOUT_H);
// 	*AccY=wiringXI2CReadReg16(fd_mpu,MPU6050_ACCEL_YOUT_H);
//     *AccZ=wiringXI2CReadReg16(fd_mpu,MPU6050_ACCEL_ZOUT_H);
// 	*GyroX=wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_XOUT_H);
// 	*GyroY=wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_YOUT_H);
// 	*GyroZ=wiringXI2CReadReg8(fd_mpu,MPU6050_GYRO_ZOUT_H);
// }

int main()
{
    MPU6050_Init();
    int16_t AX, AY, AZ, GX, GY, GZ;

    while(1)
    {
        MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
   printf("AX=%d\r\n",AX);
   printf("AY=%d\r\n",AY);
   printf("AZ=%d\r\n",AZ);
   printf("GX=%d\r\n",GX);
   printf("GY=%d\r\n",GY);
   printf("GZ=%d\r\n",GZ);


    }


   return 0;




}