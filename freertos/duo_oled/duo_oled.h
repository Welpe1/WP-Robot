#ifndef __DUO_OLED_H
#define __DUO_OLED_H

#include <stdio.h>

#define OLED_8X16				8
#define OLED_6X8				6
#define OLED_I2C    I2C0
#define OLED_ADDR   0x3C  //not 0x78


void i2c0_init(void);
void i2c0_w_sda(uint8_t enable);
uint8_t i2c0_r_sda(void);
void i2c0_w_scl(uint8_t enable);
void i2c0_start(void);
void i2c0_stop(void);
uint8_t i2c0_wait_ack(void);
void i2c0_w_ack(uint8_t ack);
void i2c0_w_byte(uint8_t byte);
uint8_t i2c0_r_byte(uint8_t ack);

uint8_t oled_w_byte(uint8_t data,uint8_t cmd_data);
void oled_w_bytes(uint8_t *data, uint16_t len);
void oled_set_cursor(uint8_t Page, uint8_t X);
uint32_t oled_pow(uint32_t X, uint32_t Y);
void oled_update(void);
void oled_full(void);
void oled_clear(void);
void oled_clear_area(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height);
void oled_show_image(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);
void oled_show_char(uint8_t X,uint8_t Y,uint8_t Char,uint8_t FontSize);
void oled_show_string(uint8_t X,uint8_t Y,char *String, uint8_t FontSize);
void oled_show_num(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void oled_show_signednum(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void oled_show_floatnum(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void oled_init(void);



#endif
