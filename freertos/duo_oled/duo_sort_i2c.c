#include "duo_soft_i2c.h"
#include "arch_sleep.h"
#include "duo_reg.h"

void sda_w(struct Soft_I2C_Base *i2c,uint8_t bit)
{
    gpio_w_pin(i2c->GPIOx,i2c->sda,bit);
	arch_nsleep(10);

}

void scl_w(struct Soft_I2C_Base *i2c,uint8_t bit)
{

    gpio_w_pin(i2c->GPIOx,i2c->scl,bit);
	arch_nsleep(10);

}

uint8_t sda_r(struct Soft_I2C_Base *i2c)
{
    return gpio_r_pin(i2c->GPIOx,i2c->sda);  
}


void i2c_start(struct Soft_I2C_Base *i2c)
{
	sda_w(i2c,1);
	scl_w(i2c,1);
	sda_w(i2c,0);
	scl_w(i2c,0);

}

void i2c_stop(struct Soft_I2C_Base *i2c)
{
	sda_w(i2c,0);
	scl_w(i2c,1);
	sda_w(i2c,1);

}


uint8_t i2c_wait_ack(struct Soft_I2C_Base *i2c)
{
	uint8_t errtime=0;
	sda_w(i2c,1);
	scl_w(i2c,1);
	while(sda_r(i2c))
	{
		errtime++;
		if(errtime>50)
		{
			i2c_stop(i2c);
			return 1;	//无应答
		}
	}
	scl_w(i2c,0);
	return 0;	//有应答
}

void i2c_w_ack(struct Soft_I2C_Base *i2c,uint8_t ack)
{
	scl_w(i2c,0);
	if(ack) sda_w(i2c,0);
	else sda_w(i2c,1);
	scl_w(i2c,1);
	scl_w(i2c,0);

}

void i2c_w_byte(struct Soft_I2C_Base *i2c,uint8_t byte)
{
	uint8_t i=0;
	for(i=0;i<8;i++)
	{
		sda_w(i2c,!!(byte & (0x80>>i)));
		scl_w(i2c,1);
		scl_w(i2c,0);

	}
    i2c_wait_ack(i2c);

}

uint8_t i2c_r_byte(struct Soft_I2C_Base *i2c,uint8_t ack)
{
	uint8_t i=0,ret=0;
    sda_w(i2c,1);
	for(i=0;i<8;i++)
	{
		scl_w(i2c,1);
		if(sda_r(i2c)) {ret|=(0x80>>i);}
		scl_w(i2c,0);

	}
    i2c_w_ack(i2c,ack);
	return ret;
}