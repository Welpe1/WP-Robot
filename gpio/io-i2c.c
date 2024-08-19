#include "duo_reg.h"
#include <stdio.h>

void i2c0_init(void)
{	
	*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 29;       //设置为输出
	*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 28;

}

void i2c0_w_sda(uint8_t enable)
{
	//*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 29;       //设置为输出
	if (enable) {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) |= 1 << 29;
	} else {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) &=~(1 << 29);
	}
	
	arch_usleep(2);
}

uint8_t i2c0_r_sda(void)
{
    *(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) &= ~(1 << 29);  
    uint32_t ret = *(uint32_t*)(GPIOA_BASE | GPIO_EXT_PORTA); 
    
    arch_usleep(2);
    *(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 29;
     
    return (ret >> 29) & 1;   
    

}

void i2c0_w_scl(uint8_t enable)
{
	//*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DDR) |= 1 << 28;

	if (enable) {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) |= 1 << 28;
	} else {
		*(uint32_t*)(GPIOA_BASE | GPIO_SWPORTA_DR) &=~(1 << 28);
	}
	
	arch_usleep(2);
}

void i2c0_start(void)
{
	i2c0_w_sda(1);
	i2c0_w_scl(1);
	i2c0_w_sda(0);
	i2c0_w_scl(0);

}

void i2c0_stop(void)
{
	i2c0_w_sda(0);
	i2c0_w_scl(1);
	i2c0_w_sda(1);

}


uint8_t i2c0_wait_ack(void)
{
	uint8_t errtime=0;
	i2c0_w_sda(1);
	i2c0_w_scl(1);
	while(i2c0_r_sda())
	{
		errtime++;
		if(errtime>50)
		{
			i2c0_stop();
			return 1;	//无应答
		}
	}
	i2c0_w_scl(0);
	return 0;	//有应答
}

void i2c0_w_ack(uint8_t ack)
{
	i2c0_w_scl(0);
	if(ack) i2c0_w_sda(0);
	else i2c0_w_sda(1);
	i2c0_w_scl(1);
	i2c0_w_scl(0);

}

void i2c0_w_byte(uint8_t byte)
{
	uint8_t i=0;
	for(i=0;i<8;i++)
	{
		i2c0_w_sda(!!(byte & (0x80>>i)));
		i2c0_w_scl(1);
		i2c0_w_scl(0);

	}
	i2c0_w_scl(1);	//额外的一个时钟，不处理应答信号
	i2c0_w_scl(0);

}

uint8_t i2c0_r_byte(uint8_t ack)
{
	uint8_t i=0,ret=0;
	for(i=0;i<8;i++)
	{
		ret<<=1;
		i2c0_w_scl(1);
		if(i2c0_r_sda()) ret++;
		i2c0_w_scl(0);

	}
	i2c0_w_ack(ack);
	return ret;
}


void oled_w_byte(uint8_t data,uint8_t cmd_data)
{
	i2c0_start();
	i2c0_w_byte(0x78);
	if(cmd_data) i2c0_w_byte(0x40);
	else i2c0_w_byte(0x00);
	i2c0_w_byte(data);
	i2c0_stop();
}

void oled_w_bytes(uint8_t *data, uint16_t len)
{
	uint16_t i=0;
	i2c0_start();
	i2c0_w_byte(0x78);
	i2c0_w_byte(0x40);
	for(i=0;i<len;i++)
	{
		i2c0_w_byte(data[i]);
	}
	i2c0_stop();
}