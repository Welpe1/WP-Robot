#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>


uint8_t read_bit(uint64_t value,uint8_t position)
{
    if((value >> position)& 1) return 1;
    else return 0;
}

void set_bit(uint64_t *value,uint8_t position,uint8_t bit)
{
    if(bit){
        *value |= (1 << position);
    }else{
        *value &=~(1 << position);
    }
}

int main()
{
    // for(int i=0;i<64;i++)
    // {
    //     printf("bit[%d]=%d\r\n",i,read_bit(0xF00FFFFFFFFFFF00,i));
    // }
    // printf("bit[2]=%d\r\n",read_bit(0xF00FFFFFFFFFFF00,2));
    uint8_t a=0x00;
    set_bit(&a,0,1);
    printf("value=%d\r\n",a);
    printf("bit[2]=%d\r\n",read_bit(a,2));
    
    return 0;
}