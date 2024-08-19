void oled_w_byte(uint8_t data,uint8_t cmd_data)
{
    if(cmd_data)  hal_i2c_write(OLED_I2C,OLED_ADDR,0x40,1,data,1);
	else  hal_i2c_write(OLED_I2C,OLED_ADDR,0x00,1,data,1);
}


void oled_w_bytes(uint8_t *data, uint16_t len)
{
    hal_i2c_write(OLED_I2C,OLED_ADDR,0x40,1,data,len);;
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