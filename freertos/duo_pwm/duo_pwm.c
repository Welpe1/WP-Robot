#include <stdio.h>
#include "cvi_spinlock.h"
#include "duo_reg.h"
#include "duo_pwm.h"


/**
 * @param PWMx:对应具体的pwm(0~11)
 * @param id:id=PWMx的x
 * 
 */
void pwm_init(struct pwm_config *PWMx,uint8_t id)
{

    PWMx->pwm_base = NULL;  
    PWMx->id=id%4;      //基地址中的具体某一项
    id=id/4;    //基地址

    switch (id) {
    case 0:
        PWMx->pwm_base = (struct pwm_regs *)PWM0_BASE;
        break;
    case 1:
        PWMx->pwm_base = (struct pwm_regs *)PWM1_BASE;
        break;
    case 2:
        PWMx->pwm_base = (struct pwm_regs *)PWM2_BASE;
        break;
    case 3:
        PWMx->pwm_base = (struct pwm_regs *)PWM3_BASE;
        break;
    }

    PWMx->pwm_base->lperiod_period[2*PWMx->id+1]=5000000;
    PWMx->pwm_base->lperiod_period[2*PWMx->id]=4850000;
    PWMx->pwm_base->polarity=0;
    PWMx->pwm_base->pwmstart |= (1 << PWMx->id);
    PWMx->pwm_base->pwm_oe |= (1 << PWMx->id);

}


void set_angle(struct pwm_config *PWMx,uint16_t angle)
{
	uint32_t low_level = angle * 200000 / 180 + 4750000;
    PWMx->pwm_base->pwmstart &= (0 << PWMx->id);
    PWMx->pwm_base->pwmstart |= (1 << PWMx->id);
    PWMx->pwm_base->lperiod_period[2*PWMx->id] = low_level;

} 



// void pwm_init(uint8_t pwm_id)
// {

//     uint8_t base_id=pwm_id/4;    //基地址
//     uint8_t detail_id=pwm_id%4;

//     struct pwm_regs* pwm_base=NULL;   

//     switch (base_id) {
//     case 0:
//         pwm_base = (struct pwm_regs *)PWM0_BASE;
//         break;
//     case 1:
//         pwm_base = (struct pwm_regs *)PWM1_BASE;
//         break;
//     case 2:
//         pwm_base = (struct pwm_regs *)PWM2_BASE;
//         break;
//     case 3:
//         pwm_base = (struct pwm_regs *)PWM3_BASE;
//         break;
//     }

//     pwm_base->lperiod_period[2*detail_id+1]=5000000;
//     pwm_base->lperiod_period[2*detail_id]=4850000;
//     pwm_base->polarity=0;
//     pwm_base->pwmstart |= (1 << detail_id);
//     pwm_base->pwm_oe |= (1 << detail_id);

// }


// void set_angle(uint8_t pwm_id,uint16_t angle)
// {
//     uint8_t base_id=pwm_id/4;    //基地址
//     uint8_t detail_id=pwm_id%4;

//     struct pwm_regs* pwm_base=NULL;   

//     switch (base_id) {
//     case 0:
//         pwm_base = (struct pwm_regs *)PWM0_BASE;
//         break;
//     case 1:
//         pwm_base = (struct pwm_regs *)PWM1_BASE;
//         break;
//     case 2:
//         pwm_base = (struct pwm_regs *)PWM2_BASE;
//         break;
//     case 3:
//         pwm_base = (struct pwm_regs *)PWM3_BASE;
//         break;
//     }


// 	uint32_t low_level = angle * 200000 / 180 + 4750000;
//     pwm_base->pwmstart &= (0 << detail_id);
//     pwm_base->pwmstart |= (1 << detail_id);
//     pwm_base->lperiod_period[2*detail_id] = low_level;

// } 

