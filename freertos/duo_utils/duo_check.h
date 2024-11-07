#ifndef __DUO_CHECK_H
#define __DUO_CHECK_H

#include <stdio.h>

uint8_t check_register(uint32_t reg,uint32_t value);
uint8_t check_i2c_init(uint32_t waittime);


#endif