#include "duo_reg.h"
#include "duo_include.h"
#include <stdio.h>
#include "oled_font.h"


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


// void oled_w_byte(uint8_t data,uint8_t cmd_data)
// {
// 	i2c0_start();
// 	i2c0_w_byte(0x78);
// 	if(cmd_data) i2c0_w_byte(0x40);
// 	else i2c0_w_byte(0x00);
// 	i2c0_w_byte(data);
// 	i2c0_stop();
// }

// void oled_w_bytes(uint8_t *data, uint16_t len)
// {
// 	uint16_t i=0;
// 	i2c0_start();
// 	i2c0_w_byte(0x78);
// 	i2c0_w_byte(0x40);
// 	for(i=0;i<len;i++)
// 	{
// 		i2c0_w_byte(data[i]);
// 	}
// 	i2c0_stop();
// }




uint8_t OLED_DisplayBuf[8][128];    //OLED显存数组

uint8_t oled_w_byte(uint8_t data, uint8_t cmd_data)  
{  
    uint8_t count = 20,ret=0;  
    uint8_t reg_addr = cmd_data ? 0x40 : 0x00; // 根据cmd_data选择寄存器地址  
 
    while (count--) {  
		ret=hal_i2c_write(OLED_I2C, OLED_ADDR, reg_addr, 1, &data, 1);
        if (!ret) {
            break;  
        }  
        arch_usleep(5); // 假设需要微秒级的延时  
    }  
	return ret;		//0：成功 1：失败

}


// void oled_w_bytes(uint8_t *data, uint16_t len)
// {
// 	for(uint16_t i=0;i<len;i++)
//     	oled_w_byte(data[i],1);
// }

void oled_w_bytes(uint8_t *data, uint16_t len)
{

	uint8_t completed = len / 64;  
    uint8_t incompleted = len % 64;  
    uint8_t offset = 0,i=0,j=0; 

	for(i=0;i<completed;i++)
	{
		hal_i2c_write(OLED_I2C,OLED_ADDR,0x40,1,&data[offset],64);
		offset += 64; 
	}
	if(incompleted>0)
	{
	
		hal_i2c_write(OLED_I2C, OLED_ADDR, 0x40, 1,&data[offset], incompleted); 
		
	}
	
}


void oled_set_cursor(uint8_t Page, uint8_t X) 
{

 	oled_w_byte(0xB0 | Page,0);
	oled_w_byte(0x10 | ((X & 0xF0) >> 4),0);
	oled_w_byte(0x00 | (X & 0x0F),0); 
}  

uint32_t oled_pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默认为1
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}

void oled_update(void)
{
	uint8_t j;
	for (j=0;j<8;j++)
	{
		oled_set_cursor(j, 0);
		oled_w_bytes(OLED_DisplayBuf[j], 128);
	}
}



void oled_full(void)  
{ 
	uint8_t i, j;
	for (j=0;j<8;j++)				
	{
		for (i=0;i<128;i++)			
		{
			OLED_DisplayBuf[j][i]=0xAA;	
		}
	}
}

void oled_clear(void)  
{ 
	uint8_t i, j;
	for (j=0;j<8;j++)				
	{
		for (i=0;i<128;i++)			
		{
			OLED_DisplayBuf[j][i]=0x00;	
		}
	}
}

/**
  *X:       0-127
  *Y:       0-63
  *Width:   0-128
  *Height:  0-64
  */
void oled_clear_area(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height)
{
	uint8_t i, j;
	for (j=Y;j<Y+Height;j++)		//遍历指定页
	{
		for (i=X;i<X+Width;i++)	//遍历指定列
		{
			if(i<=127&&j<=63) OLED_DisplayBuf[j/8][i]&=~(0x01<<(j%8));	//将显存数组指定数据清零

		}
	}
}


/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void oled_show_image(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	uint8_t i = 0, j = 0;
	uint8_t Page, Shift;

	oled_clear_area(X, Y, Width, Height);
	
	/*遍历指定图像涉及的相关页*/
	/*(Height - 1) / 8 + 1的目的是Height / 8并向上取整*/
	for (j=0;j<(Height-1)/8+1;j++)
	{
		/*遍历指定图像涉及的相关列*/
		for (i=0;i<Width;i++)
		{
			if (X+i<=127)		//超出屏幕的内容不显示
			{
				/*负数坐标在计算页地址和移位时需要加一个偏移*/
				Page = Y / 8;
				Shift = Y % 8;
				if (Page + j <= 7) OLED_DisplayBuf[Page + j][X + i] |= Image[j * Width + i] << (Shift);
				if (Page + j + 1 <= 7) OLED_DisplayBuf[Page + j + 1][X + i] |= Image[j * Width + i] >> (8 - Shift);
				
			}
		}
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void oled_show_char(uint8_t X,uint8_t Y,uint8_t Char,uint8_t FontSize)
{
	if (FontSize == OLED_8X16)		//字体为宽8像素，高16像素
	{
		oled_show_image(X, Y, 8, 16, OLED_F8x16[Char - ' ']);
	}
	else if(FontSize == OLED_6X8)	//字体为宽6像素，高8像素
	{
		oled_show_image(X, Y, 6, 8, OLED_F6x8[Char - ' ']);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void oled_show_string(uint8_t X,uint8_t Y,char *String, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)		//遍历字符串的每个字符
	{
		/*调用oled_show_char函数，依次显示每个字符*/
		oled_show_char(X + i * FontSize, Y, String[i], FontSize);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void oled_show_num(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一位							
	{
		oled_show_char(X + i * FontSize, Y, Number / oled_pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void oled_show_signednum(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	uint32_t Number1;
	
	if (Number >= 0)						//数字大于等于0
	{
		oled_show_char(X, Y, '+', FontSize);	//显示+号
		Number1 = Number;					//Number1直接等于Number
	}
	else									//数字小于0
	{
		oled_show_char(X, Y, '-', FontSize);	//显示-号
		Number1 = -Number;					//Number1等于Number取负
	}
	
	for (i = 0; i < Length; i++)			//遍历数字的每一位								
	{
		/*调用oled_show_char函数，依次显示每个数字*/
		/*Number1 / oled_pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		oled_show_char(X + (i + 1) * FontSize, Y, Number1 / oled_pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void oled_show_floatnum(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
	uint32_t PowNum, IntNum, FraNum;
	
	if (Number >= 0)						//数字大于等于0
	{
		oled_show_char(X, Y, '+', FontSize);	//显示+号
	}
	else									//数字小于0
	{
		oled_show_char(X, Y, '-', FontSize);	//显示-号
		Number = -Number;					//Number取负
	}
	
	/*提取整数部分和小数部分*/
	IntNum = Number;						//直接赋值给整型变量，提取整数
	Number -= IntNum;						//将Number的整数减掉，防止之后将小数乘到整数时因数过大造成错误
	PowNum = oled_pow(10, FraLength);		//根据指定小数的位数，确定乘数
	FraNum = round(Number * PowNum);		//将小数乘到整数，同时四舍五入，避免显示误差
	IntNum += FraNum / PowNum;				//若四舍五入造成了进位，则需要再加给整数
	
	/*显示整数部分*/
	oled_show_num(X + FontSize, Y, IntNum, IntLength, FontSize);
	
	/*显示小数点*/
	oled_show_char(X + (IntLength + 1) * FontSize, Y, '.', FontSize);
	
	/*显示小数部分*/
	oled_show_num(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

void oled_init(void)
{
	uint8_t ret=0,count=30;
	hal_i2c_init(I2C0);

	while(count--)
	{
		ret+=oled_w_byte(0xAE,0);	//设置显示开启/关闭，0xAE关闭，0xAF开启
		ret+=oled_w_byte(0xD5,0);	//设置显示时钟分频比/振荡器频率
		ret+=oled_w_byte(0x80,0);	//0x00~0xFF
		ret+=oled_w_byte(0xA8,0);	//设置多路复用率
		ret+=oled_w_byte(0x3F,0);	//0x0E~0x3F
		ret+=oled_w_byte(0xD3,0);	//设置显示偏移
		ret+=oled_w_byte(0x00,0);	//0x00~0x7F
		ret+=oled_w_byte(0x40,0);	//设置显示开始行，0x40~0x7F
		ret+=oled_w_byte(0xA1,0);	//设置左右方向，0xA1正常，0xA0左右反置
		ret+=oled_w_byte(0xC8,0);	//设置上下方向，0xC8正常，0xC0上下反置
		ret+=oled_w_byte(0xDA,0);	//设置COM引脚硬件配置
		ret+=oled_w_byte(0x12,0);
		ret+=oled_w_byte(0x81,0);	//设置对比度
		ret+=oled_w_byte(0xCF,0);	//0x00~0xFF
		ret+=oled_w_byte(0xD9,0);	//设置预充电周期
		ret+=oled_w_byte(0xF1,0);
		ret+=oled_w_byte(0xDB,0);	//设置VCOMH取消选择级别
		ret+=oled_w_byte(0x30,0);
		ret+=oled_w_byte(0xA4,0);	//设置整个显示打开/关闭
		ret+=oled_w_byte(0xA6,0);	//设置正常/反色显示，0xA6正常，0xA7反色
		ret+=oled_w_byte(0x8D,0);	//设置充电泵
		ret+=oled_w_byte(0x14,0);
		ret+=oled_w_byte(0xAF,0);	//开启显示
		if(!ret) break;

	}
	

	oled_clear();				//清空显存数组
	oled_update();				//更新显示，清屏，防止初始化后未显示内容时花屏
} 




