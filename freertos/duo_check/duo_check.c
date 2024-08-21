#include <stdio.h>
#include "duo_reg.h"
#include "arch_sleep.h"

/**
 * @brief 检查寄存器的值，与期望值相同返回0，不同返回1
 * @param reg 寄存器地址
 * @param value 期望的值
 * 
 */
uint8_t check_register(uint32_t reg,uint32_t value)
{
    uint32_t ret=0;
    ret=*(uint32_t*)(reg);
    if(ret==value) return 0;
    else return 1;

}

/**
 * @brief 检查软件i2c引脚复用是否正确（0：正确，1：错误）
 * @param waittime 等待时间
 */
uint8_t check_i2c_init(uint32_t waittime)
{
    while (check_register(PINMUX_BASE + PINMUX_GP0, 3) && check_register(PINMUX_BASE + PINMUX_GP1, 3))
    {
        waittime--;
        printf("soft i2c0 not ready\r\n");
        if(!waittime)
        {
            return 1;
        }
        arch_usleep(10);
    }

    return 0;
}