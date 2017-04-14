#include "motor.h"
#include "sys.h"
#include "usart.h"		
#include "delay.h"
#include "cont.h"
#include "timer.h"


void MOTOR_Init(void)
{
	PUL1=0;
	DIR1=0;
	PUL2=0;
	DIR2=0;	

	
	IDIR1=0;
	IDIR2=0;
	IPUL1=0;
	IPUL2=0;	
}

//Direct�ǵ��ת������ motor_num�ǵ����ţ� motor_step�ǲ���
void MOTOR_Control(u8 Direct,u8 motor_num,u8 motor_step)
{ 
	
	if(motor_num==0){PWM1=motor_step;DIR1=Direct;}  //Direct 0��1������ת��ת
	else if(motor_num==1){PWM2=motor_step;DIR2=Direct;}
	else if(motor_num==2){PWM3=motor_step;IDIR1=Direct;}
	else if(motor_num==3){PWM4=motor_step;IDIR2=Direct;}
	while(PWM4 || PWM3 || PWM2 || PWM1)	 //������ȴ���֪�����ת�����
	{									 //ʹ���˶�ʱ��3������PWM����
	}
}

