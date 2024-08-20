#include "duo_reg.h"
#include <stdio.h>
#include "cvi_spinlock.h"


static struct uart_regs *uart4 = 0;

void uart4_init(int baudrate)
{

	int divisor = 25 * 1000 * 1000 / (16 * baudrate);
	uart4 = (struct uart_regs *)UART4_BASE;						//uart4的地址为UART4_BASE，随后每个寄存器在uart4的基础上偏移
	uart4->lcr = uart4->lcr | UART_LCR_DLAB | UART_LCR_8N1;
	uart4->rbr = divisor & 0xff;								//[31:8]无效，当[7]=1时存储16位波特率的低8位
	uart4->ier = (divisor >> 8) & 0xff;							//[31:8]无效，当[7]=1时存储16位波特率的高8位
	uart4->lcr = uart4->lcr & (~UART_LCR_DLAB);					//使能fifo，设置软复位
	uart4->ier = 0;												//关闭中断等
	uart4->mcr = UART_MCRVAL;
	uart4->fcr = UART_FCR_DEFVAL;
	uart4->lcr = 3;
}

uint8_t uart4_putc(uint8_t ch)
{
	if (ch == '\n') {
		while (!(uart4->lsr & UART_LSR_THRE));
		uart4->rbr = '\r';
	}
	while (!(uart4->lsr & UART_LSR_THRE));
	uart4->rbr = ch;
	return ch;
}

void uart4_puts(char *str)
{
	if (!str)
		return;

	while (*str) {
		uart4_putc(*str++);
	}
}

int uart4_getc(void)
{
	while (!(uart4->lsr & UART_LSR_DR))
		return (int)uart4->rbr;
}

int uart4_tstc(void)
{
	return (!!(uart4->lsr & UART_LSR_DR));
}


