#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//通过改变TIM3->CCR2的值来改变占空比，从而控制LED0的亮度
#define LED0_PWM_VAL TIM3->CCR2    
//TIM4 CH1作为PWM DAC的输出通道 
#define PWM_DAC_VAL  TIM4->CCR1 

void TIM3_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);

extern u16  PWM1,PWM2,PWM3,PWM4;

#endif























