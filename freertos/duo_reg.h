#include <stdio.h>

#define GPIOA_BASE          0x03020000  //GPIOA基地址，gpiochip0
#define GPIOB_BASE          0x03021000  //gpiochip1
#define GPIOC_BASE          0x03022000  //gpiochip2
#define GPIOD_BASE          0x03023000  //gpiochip3
#define PWR_GPIO_BASE       0x05021000  //gpiochip4

#define GPIO_SWPORTA_DR     0x000  //GPIO输出数据寄存器偏移地址
#define GPIO_EXT_PORTA      0x050  //GPIO输入数据寄存器偏移地址
#define GPIO_SWPORTA_DDR    0x004  //GPIO端口方向设置寄存器偏移地址（0输入 1输出）

