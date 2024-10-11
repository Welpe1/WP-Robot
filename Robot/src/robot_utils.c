#include "robot_utils.h"


uint8_t read_bit(uint8_t value,uint8_t position)
{
    return value & (0x01 << position);

}


uint8_t count_bit(uint16_t data) 
{  
    uint8_t count = 0;  
    while (data) {  
        count += data & 1;  
        data >>= 1;  
    }  
    return count;  
} 

