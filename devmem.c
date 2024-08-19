#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>


/*
    输入参数范围在3-5
    只有以下两种模式

    3:
    ./devmem -r 0x1110000
    4:
    ./devmem -r 0x1110000 x(0-63)
    x指寄存器具体某一位
    5:
    ./devmem -w 0x1110000 x(0-63) 1/0

*/

#define MAP_SIZE    4096UL
#define MAP_MASK    (MAP_SIZE-1)

int main(int argc,char* argv[])
{
    int fd=0,id=0;
    unsigned int position=0,position_data=0,position_mask=0,flag=0;
    off_t target;
    char *endptr; // 用于存储strtol停止转换的位置 
    void *map_base,*virtual_addr;
    char mode='r';

    if(argc<3){
        printf("Parameter needs more than 2\n");
        exit(1);
    }else{
        if(strlen(argv[1])==2 && argv[1][0]=='-'){
            if(argv[1][1]=='r'|| argv[1][1]=='w'){
                mode=argv[1][1];
                printf("Mode=%c\n",mode);
            }
            else goto ERR_USAGE;
        }
        else goto ERR_USAGE;

    }

    target = strtol(argv[2], &endptr,0); 
    if (endptr == argv[2] || *endptr != '\0') {  // 检查转换是否成功  
        printf("Invalid hexadecimal input: %s\n", argv[2]);  
        exit(1);  
    }

    if((fd=open("/dev/mem",O_RDWR | O_SYNC))==-1){
        printf("Open /dev/mem fail\n");
        exit(1);
    }

    map_base=mmap(0,MAP_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED,fd,target & (~MAP_MASK));
    if(map_base==(void*)-1){
        printf("Memory mapped fail\n");
        exit(1);
    }else{
        printf("Memory mapped at address %p.\n", map_base);
    }
    virtual_addr=map_base+(target&MAP_MASK);

    if(argc>=4){
        position=atoi(argv[3]);
        if(position>64){
            printf("position needs less than 64\n");
            goto ERR_USAGE;
        }

        flag = (position < 32) ? 0 : 1;  
        position_mask = 1U << position; // 使用无符号字面量  
        position_data = (*(volatile unsigned int *)(virtual_addr + flag * 4)) & position_mask;

    }

    switch(mode){
        case 'r':
        {
            if(argc>4){
                printf("Read mode needs less than 5 parameters\n");
                goto ERR_USAGE;
            }
            printf("reg[31:0]  = %x \r\n", *(volatile unsigned int *)(virtual_addr)); 
            printf("reg[63:32] = %x \r\n", *(volatile unsigned int *)(virtual_addr+4));
            printf("reg[%d]  = %d \r\n", position, position_data != 0); 

        }break;
        case 'w':
        {
            if(argc!=5){
                printf("Write mode needs 5 parameters\n");
                goto ERR_USAGE;
            }

            printf("old_reg[31:0]  = %x \r\n", *(volatile unsigned int *)(virtual_addr)); 
            printf("old_reg[63:32] = %x \r\n", *(volatile unsigned int *)(virtual_addr+4));
            printf("old_reg[%d]    = %d \n", position, position_data != 0); 
            if(atoi(argv[4])){
                *(volatile unsigned int *)(virtual_addr + flag * 4) |= (1<<(position-32*flag));         //高

            }else{
                *(volatile unsigned int *)(virtual_addr + flag * 4) &= ~(1<<(position-32*flag));         //低
            }

            position_mask = 1U << position; // 使用无符号字面量  
            position_data = (*(volatile unsigned int *)(virtual_addr + flag * 4)) & position_mask;

            printf("new_reg[%d]    = %d \n", position, position_data != 0); 
            printf("new_reg[31:0]  = %x \r\n", *(volatile unsigned int *)(virtual_addr)); 
            printf("new_reg[63:32] = %x \r\n", *(volatile unsigned int *)(virtual_addr+4));
        
        }break;

    }

    close(fd);
    return 0;

ERR_USAGE:
    printf("usage err\n");
    exit(1);


}



