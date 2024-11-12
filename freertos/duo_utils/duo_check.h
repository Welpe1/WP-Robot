#ifndef __DUO_CHECK_H
#define __DUO_CHECK_H

#include <stdio.h>
#include <stdint.h>

#define CHECK_STACK_SIZE     8

struct Check_Config{
    uint8_t pointer;     //栈指针
    uint32_t reg[CHECK_STACK_SIZE];      //寄存器
    uint8_t value[CHECK_STACK_SIZE];     //预期值

};

uint8_t check_register(uint32_t reg,uint32_t value);
uint8_t check_system_push(uint32_t reg,uint32_t value);
uint8_t check_system_pop(void);
void check_system_clear(void);
void check_system_init(void);
uint8_t check_system_run(void);
void check_system_destroy(void);

#endif