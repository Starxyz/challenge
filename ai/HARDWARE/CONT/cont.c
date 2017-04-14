#include "cont.h"

void CONT_Init(void)
{

	RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	   	 	
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<6;    //ʹ��PORTEʱ�� 
	
	GPIOA->CRL&=0X0000FFFF; 
	GPIOA->CRL|=0X33330000;//PA.4~7 �������   	 
    GPIOA->ODR|=0X00;      //PA.4~7 ����� //GPIOA->ODR|=0XF0;      //PA.4~7 �����

	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X00033000;//PA.11~12 �������   	 
    GPIOA->ODR|=0X00;      //PA.11~12 ����� //GPIOA->ODR|=0X18;      //PA.11~12 �����

	GPIOB->CRL&=0XFF0FFFFF; 
	GPIOB->CRL|=0X00300000;//PB.5 �������   	 
    GPIOB->ODR|=1<<5;      //PB.5 �����

	GPIOB->CRH&=0XF00FFFFF; 
	GPIOB->CRH|=0X03300000;//PB.13~14 �������   	 
    GPIOB->ODR|=0X00;      //PB.13~14 ����� //GPIOB->ODR|=0X60;      //PB.13~14 �����
						
											  
	GPIOE->CRL&=0XFF0FFFFF;
	GPIOE->CRL|=0X00300000;//PE.5�������
	GPIOE->ODR|=1<<5;      //PE.5����� 

}
