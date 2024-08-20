#ifndef __DUO_REG_H
#define __DUO_REG_H

#include <stdio.h>

#define GPIOA_BASE          	0x03020000  //GPIOA基地址，gpiochip0
#define GPIOB_BASE          	0x03021000  //gpiochip1
#define GPIOC_BASE          	0x03022000  //gpiochip2
#define GPIOD_BASE          	0x03023000  //gpiochip3
#define PWR_GPIO_BASE       	0x05021000  //gpiochip4
#define GPIO_SWPORTA_DR     	0x000  //GPIO输出数据寄存器偏移地址
#define GPIO_EXT_PORTA     		0x050  //GPIO输入数据寄存器偏移地址
#define GPIO_SWPORTA_DDR    	0x004  //GPIO端口方向设置寄存器偏移地址（0输入 1输出）

#define OLED_8X16				8
#define OLED_6X8				6
#define OLED_I2C    			I2C0
#define OLED_ADDR   			0x3C  //not 0x78


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
#define MPU6050_ADDR			0xD0
#define MPU6050_I2C    			I2C1


//UART0~3在top_reg.h中有定义
#define UART4_BASE          	0x041C0000
#define UART_LCR_WLS_MSK 		0x03 /* character length select mask */
#define UART_LCR_WLS_5 			0x00 /* 5 bit character length */
#define UART_LCR_WLS_6 			0x01 /* 6 bit character length */
#define UART_LCR_WLS_7 			0x02 /* 7 bit character length */
#define UART_LCR_WLS_8 			0x03 /* 8 bit character length */
#define UART_LCR_STB 			0x04 /* # stop Bits, off=1, on=1.5 or 2) */
#define UART_LCR_PEN 			0x08 /* Parity eneble */
#define UART_LCR_EPS 			0x10 /* Even Parity Select */
#define UART_LCR_STKP 			0x20 /* Stick Parity */
#define UART_LCR_SBRK 			0x40 /* Set Break */
#define UART_LCR_BKSE 			0x80 /* Bank select enable */
#define UART_LCR_DLAB 			0x80 /* Divisor latch access bit */
#define UART_MCR_DTR 			0x01 /* DTR   */
#define UART_MCR_RTS 			0x02 /* RTS   */
#define UART_LSR_THRE 			0x20 /* Transmit-hold-register empty */
#define UART_LSR_DR 			0x01 /* Receiver data ready */
#define UART_LSR_TEMT 			0x40 /* Xmitter empty */
#define UART_FCR_FIFO_EN 		0x01 /* Fifo enable */
#define UART_FCR_RXSR 			0x02 /* Receiver soft reset */
#define UART_FCR_TXSR 			0x04 /* Transmitter soft reset */
#define UART_MCRVAL 			(UART_MCR_DTR | UART_MCR_RTS) /* RTS/DTR */
#define UART_FCR_DEFVAL 		(UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR)
#define UART_LCR_8N1 			0x03

struct uart_regs{
	volatile uint32_t rbr; /* 0x00 Data register */
	volatile uint32_t ier; /* 0x04 Interrupt Enable Register */
	volatile uint32_t fcr; /* 0x08 FIFO Control Register */
	volatile uint32_t lcr; /* 0x0C Line control register */
	volatile uint32_t mcr; /* 0x10 Line control register */
	volatile uint32_t lsr; /* 0x14 Line Status Register */
	volatile uint32_t msr; /* 0x18 Modem Status Register */
	volatile uint32_t spr; /* 0x20 Scratch Register */


};

struct mpu_data{
	int accx;
	int accy;
	int accz;
	int gyrox;
	int gyroy;
	int gyroz;
};


#endif
