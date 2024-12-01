#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>


#define SET_MOOD_STATUS(in,status) (in | (0x01&(status)))
#define SET_TRIGGER_STATUS(in,status) (in | (0x02&(status)))

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
    //set_bit(&a,0,1);
    a=SET_MOOD_STATUS(a,1);
    printf("mood %d=\r\n",a);
    
    return 0;
}