#include <stdio.h>
#include "duo_check.h"
#include "duo_reg.h"
#include "arch_sleep.h"

struct Check_Config stCS_Config={0};

/**
 * @brief 检查寄存器的值(最多重复检测waittime次)
 *        与期望值相同返回0，不同返回1
 * @param reg 寄存器地址
 * @param value 期望的值
 */
uint8_t check_register(uint32_t reg,uint32_t value)
{
    uint16_t waittime=300;
    while(*(uint32_t*)(reg)!=value)
    {
        waittime--;
        if(!waittime) return 1;
        arch_usleep(500);
    }
    return 0;
}

/**
 * @brief 向检查栈推入新的寄存器数据
 *        当栈已满了，操作无效
 *        成功入栈返回0，失败返回1
 * 
 */
uint8_t check_system_push(uint32_t reg,uint32_t value)
{
    if(stCS_Config.pointer<CHECK_STACK_SIZE)
    {
        
        stCS_Config.reg[stCS_Config.pointer]=reg;
        stCS_Config.value[stCS_Config.pointer]=value;  
        stCS_Config.pointer++;
        return 0;
    }else{
        printf("out of check stack size\r\n");
        return 1;
    }
}

uint8_t check_system_pop(void)
{
    if(stCS_Config.pointer>0) 
    {
        stCS_Config.pointer--;
        return 0;
    }else{
        printf("check stack size already empty\r\n");
        return 1;
    }
}

void check_system_clear(void)
{
    stCS_Config.pointer=0;
    memset(stCS_Config.reg,0,CHECK_STACK_SIZE * sizeof(uint32_t));
    memset(stCS_Config.value,0,CHECK_STACK_SIZE * sizeof(uint32_t));
}

void check_system_init(void)
{
    check_system_clear();
    // for(uint8_t i=0;i<CHECK_STACK_SIZE;i++)
    // {
    //     printf("value[%d]=%d\r\n",i,stCS_Config.value[i]);
    // }
    
}

/**
 * @brief 检查各软件引脚复用是否正确（0：正确，1：错误）
 * @param waittime 等待时间
 */
uint8_t check_system_run(void)
{
    // for(uint8_t i=0;i<CHECK_STACK_SIZE;i++)
    // {
    //     printf("value[%d]=%d\r\n",i,stCS_Config.value[i]);
    // }

    for(uint8_t i=0;i<stCS_Config.pointer;i++)
    {
        if(check_register(stCS_Config.reg[i],stCS_Config.value[i])) return 1;
    }
    return 0;
    
}

void check_system_destroy(void)
{
   check_system_clear();
    
}
