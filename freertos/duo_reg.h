#ifndef __DUO_REG_H
#define __DUO_REG_H

#include "top_reg.h"

#define GPIOA_BASE          0x03020000  //GPIOA基地址，gpiochip0
#define GPIOB_BASE          0x03021000  //gpiochip1
#define GPIOC_BASE          0x03022000  //gpiochip2
#define GPIOD_BASE          0x03023000  //gpiochip3
#define PWR_GPIO_BASE       0x05021000  //gpiochip4

#define GPIO_SWPORTA_DR     0x000   //GPIO输出数据寄存器偏移地址
#define GPIO_EXT_PORTA      0x050   //GPIO输入数据寄存器偏移地址
#define GPIO_SWPORTA_DDR    0x004   //GPIO端口方向设置寄存器偏移地址（0输入 1输出）
#define GPIO_INTEN          0x030   //中断使能寄存器
#define GPIO_INTMASK        0x034 
#define GPIO_INTTYPE_LEVEL  0x038   //选择电平触发或边沿触发
#define GPIO_INT_POLARITY   0x03C   //选择低电平/高电平触发和下降沿/上升沿触发
#define GPIO_INTSTATUS      0x040   //中断状态寄存器
#define GPIO_RAW_INTSTATUS  0x044
#define GPIO_DEBOUNCE       0x048
#define GPIO_PORTA_EOI      0x04C   //中断清除寄存器
#define GPIO_EXT_PORTA      0x050
#define GPIO_LS_SYNC        0x060


//UART0~3在top_reg.h中有定义
#define UART4_BASE          0x041C0000

#define UART_LCR_WLS_MSK    0x03 /* character length select mask */
#define UART_LCR_WLS_5      0x00 /* 5 bit character length */
#define UART_LCR_WLS_6      0x01 /* 6 bit character length */
#define UART_LCR_WLS_7      0x02 /* 7 bit character length */
#define UART_LCR_WLS_8      0x03 /* 8 bit character length */
#define UART_LCR_STB        0x04 /* # stop Bits, off=1, on=1.5 or 2) */
#define UART_LCR_PEN        0x08 /* Parity eneble */
#define UART_LCR_EPS        0x10 /* Even Parity Select */
#define UART_LCR_STKP       0x20 /* Stick Parity */
#define UART_LCR_SBRK       0x40 /* Set Break */
#define UART_LCR_BKSE       0x80 /* Bank select enable */
#define UART_LCR_DLAB       0x80 /* Divisor latch access bit */

#define UART_MCR_DTR        0x01 /* DTR   */
#define UART_MCR_RTS        0x02 /* RTS   */

#define UART_LSR_THRE       0x20 /* Transmit-hold-register empty */
#define UART_LSR_DR         0x01 /* Receiver data ready */
#define UART_LSR_TEMT       0x40 /* Xmitter empty */

#define UART_FCR_FIFO_EN    0x01 /* Fifo enable */
#define UART_FCR_RXSR       0x02 /* Receiver soft reset */
#define UART_FCR_TXSR       0x04 /* Transmitter soft reset */

#define UART_MCRVAL         (UART_MCR_DTR | UART_MCR_RTS) /* RTS/DTR */
#define UART_FCR_DEFVAL     (UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR)
#define UART_LCR_8N1        0x03


#define PINMUX_BASE         0x03001000
#define PINMUX_GP0          0x4C
#define PINMUX_GP1          0x50
#define PINMUX_GP16         0x3C
#define PINMUX_GP17         0x40




#endif
