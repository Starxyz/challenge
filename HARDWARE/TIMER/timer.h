#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//ͨ���ı�TIM3->CCR2��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define LED0_PWM_VAL TIM3->CCR2    
//TIM4 CH1��ΪPWM DAC�����ͨ�� 
#define PWM_DAC_VAL  TIM4->CCR1 

void TIM3_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);

extern u16  PWM1,PWM2,PWM3,PWM4;

#endif























