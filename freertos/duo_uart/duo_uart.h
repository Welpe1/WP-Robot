#ifndef __DUO_UART_H
#define __DUO_UART_H

#include <stdio.h>

typedef struct uart_regs {
	volatile uint32_t rbr; /* 0x00 Data register */
	volatile uint32_t ier; /* 0x04 Interrupt Enable Register */
	volatile uint32_t fcr; /* 0x08 FIFO Control Register */
	volatile uint32_t lcr; /* 0x0C Line control register */
	volatile uint32_t mcr; /* 0x10 Line control register */
	volatile uint32_t lsr; /* 0x14 Line Status Register */
	volatile uint32_t msr; /* 0x18 Modem Status Register */
	volatile uint32_t spr; /* 0x20 Scratch Register */


};

void uart4_init(int baudrate);
uint8_t uart4_putc(uint8_t ch);
void uart4_puts(char *str);
int uart4_getc(void);
int uart4_tstc(void);
int uart4_put_buff(char *buf);

#endif

