#ifndef __DUO_UART_H
#define __DUO_UART_H

#include <stdio.h>

void uart4_init(int baudrate, int uart_clock);
uint8_t uart4_putc(uint8_t ch);
void uart4_puts(char *str);
int uart4_getc(void);
int uart4_tstc(void);
int uart4_put_buff(char *buf);

#endif

