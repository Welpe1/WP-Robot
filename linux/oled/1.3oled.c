#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <wiringx.h>
#include "oled_font.h"

#define I2C_DEV                 "/dev/i2c-0"
#define SLAVE_ADDR          0x3C
#define OLED_8X16				8
#define OLED_6X8				6

int fd_i2c;
uint8_t OLED_DisplayBuf[8][128];    //OLED显存数组


/*
    OLED写数据/命令(单字节)
    cmd_data=0:command
    cmd_data=1:data
 */
void OLED_W_Byte(uint8_t data,uint8_t cmd_data)
{
	if(cmd_data) wiringXI2CWriteReg8(fd_i2c,0x40,data);
	else wiringXI2CWriteReg8(fd_i2c,0x00,data);
}

/*
    OLED写数据(多字节)
 */
void OLED_W_Bytes(uint8_t *data,uint8_t count)
{
    uint8_t i=0;
    for(i=0;i<count;i++)
    {
        wiringXI2CWriteReg8(fd_i2c,0x40,data[i]);
    }

}

void OLED_Set_Cursor(uint8_t Page, uint8_t X) 
{
    X+=2;       //1.3寸oled
 	OLED_W_Byte(0xB0 | Page,0);
	OLED_W_Byte(0x10 | ((X & 0xF0) >> 4),0);
	OLED_W_Byte(0x00 | (X & 0x0F),0); 
}  

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默认为1
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}

void OLED_Update(void)
{
	uint8_t j;
	for (j=0;j<8;j++)
	{
		OLED_Set_Cursor(j, 0);
		OLED_W_Bytes(OLED_DisplayBuf[j], 128);
	}
}

// /**
//   *X:       0-127
//   *Y:       0-63
//   *Width:   0-128
//   *Height:  0-64
//   *Page:    0-7
//   */
// void OLED_UpdateArea(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height)
// {
// 	uint8_t j;
// 	uint8_t Page, Page1;
// 	Page = Y / 8;
// 	Page1 = (Y + Height - 1) / 8 + 1;

// 	/*遍历指定区域涉及的相关页*/
// 	for (j=Page;j<Page1;j++)
// 	{
// 		if(j<=7)		//超出屏幕的内容不显示
// 		{
// 			OLED_Set_Cursor(j, X);
// 			OLED_W_Bytes(&OLED_DisplayBuf[j][X], Width);
// 		}
// 	}
// }

void OLED_Clear(void)  
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
void OLED_ClearArea(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height)
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
void OLED_ShowImage(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	uint8_t i = 0, j = 0;
	uint8_t Page, Shift;

	OLED_ClearArea(X, Y, Width, Height);
	
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
void OLED_ShowChar(uint8_t X,uint8_t Y,uint8_t Char,uint8_t FontSize)
{
	if (FontSize == OLED_8X16)		//字体为宽8像素，高16像素
	{
		OLED_ShowImage(X, Y, 8, 16, OLED_F8x16[Char - ' ']);
	}
	else if(FontSize == OLED_6X8)	//字体为宽6像素，高8像素
	{
		OLED_ShowImage(X, Y, 6, 8, OLED_F6x8[Char - ' ']);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void OLED_ShowString(uint8_t X,uint8_t Y,char *String, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)		//遍历字符串的每个字符
	{
		/*调用OLED_ShowChar函数，依次显示每个字符*/
		OLED_ShowChar(X + i * FontSize, Y, String[i], FontSize);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void OLED_ShowNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一位							
	{
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void OLED_ShowSignedNum(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	uint32_t Number1;
	
	if (Number >= 0)						//数字大于等于0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//显示+号
		Number1 = Number;					//Number1直接等于Number
	}
	else									//数字小于0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//显示-号
		Number1 = -Number;					//Number1等于Number取负
	}
	
	for (i = 0; i < Length; i++)			//遍历数字的每一位								
	{
		/*调用OLED_ShowChar函数，依次显示每个数字*/
		/*Number1 / OLED_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		OLED_ShowChar(X + (i + 1) * FontSize, Y, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  *X(图像左上角的横坐标):       0-127
  *Y(图像左上角的纵坐标):       0-63
  *Width:   0-128
  *Height:  0-64
  */
void OLED_ShowFloatNum(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
	uint32_t PowNum, IntNum, FraNum;
	
	if (Number >= 0)						//数字大于等于0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//显示+号
	}
	else									//数字小于0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//显示-号
		Number = -Number;					//Number取负
	}
	
	/*提取整数部分和小数部分*/
	IntNum = Number;						//直接赋值给整型变量，提取整数
	Number -= IntNum;						//将Number的整数减掉，防止之后将小数乘到整数时因数过大造成错误
	PowNum = OLED_Pow(10, FraLength);		//根据指定小数的位数，确定乘数
	FraNum = round(Number * PowNum);		//将小数乘到整数，同时四舍五入，避免显示误差
	IntNum += FraNum / PowNum;				//若四舍五入造成了进位，则需要再加给整数
	
	/*显示整数部分*/
	OLED_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);
	
	/*显示小数点*/
	OLED_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);
	
	/*显示小数部分*/
	OLED_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

void OLED_Init(void)
{
	OLED_W_Byte(0xAE,0);	//设置显示开启/关闭，0xAE关闭，0xAF开启

	OLED_W_Byte(0xD5,0);	//设置显示时钟分频比/振荡器频率
	OLED_W_Byte(0xC0,0);	//0x00~0xFF

	OLED_W_Byte(0xA8,0);	//设置多路复用率
	OLED_W_Byte(0x3F,0);	//0x0E~0x3F
	OLED_W_Byte(0xD3,0);	//设置显示偏移
	OLED_W_Byte(0x00,0);	//0x00~0x7F
	OLED_W_Byte(0x40,0);	//设置显示开始行，0x40~0x7F
	OLED_W_Byte(0xA1,0);	//设置左右方向，0xA1正常，0xA0左右反置
	OLED_W_Byte(0xC8,0);	//设置上下方向，0xC8正常，0xC0上下反置

	OLED_W_Byte(0xDA,0);	//设置COM引脚硬件配置
	OLED_W_Byte(0x12,0);

	OLED_W_Byte(0x81,0);	//设置对比度
	OLED_W_Byte(0xCF,0);	//0x00~0xFF

	OLED_W_Byte(0xD9,0);	//设置预充电周期
	OLED_W_Byte(0xF1,0);

	OLED_W_Byte(0xDB,0);	//设置VCOMH取消选择级别
	OLED_W_Byte(0x30,0);
	OLED_W_Byte(0xA4,0);	//设置整个显示打开/关闭
	OLED_W_Byte(0xA6,0);	//设置正常/反色显示，0xA6正常，0xA7反色

	OLED_W_Byte(0x8D,0);	//设置充电泵
	OLED_W_Byte(0x14,0);
	OLED_W_Byte(0xAF,0);	//开启显示
	OLED_Clear();				//清空显存数组
	OLED_Update();				//更新显示，清屏，防止初始化后未显示内容时花屏
}  


int main() {
    int data = 0;

    // Duo:     milkv_duo
    // Duo256M: milkv_duo256m
    // DuoS:    milkv_duos
    if(wiringXSetup("milkv_duo", NULL) == -1) {
        wiringXGC();
        return -1;
    }

    if ((fd_i2c = wiringXI2CSetup(I2C_DEV, SLAVE_ADDR)) <0) {
        printf("I2C Setup failed: %i\n", fd_i2c);
        return -1;
    }

    OLED_Init();
    OLED_Clear();

   while(1)
   {

    OLED_ShowImage(0,0,128,64,BMP1);
    OLED_Update();
    OLED_ShowImage(0,0,128,64,BMP2);
    OLED_Update();
    OLED_ShowImage(0,0,128,64,BMP3);
    OLED_Update();
    OLED_ShowImage(0,0,128,64,BMP4);
    OLED_Update();

   }



}


