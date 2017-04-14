#include "cont.h"

void CONT_Init(void)
{

	RCC->APB2ENR|=1<<3;    //使能PORTB时钟	   	 	
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	RCC->APB2ENR|=1<<6;    //使能PORTE时钟 
	
	GPIOA->CRL&=0X0000FFFF; 
	GPIOA->CRL|=0X33330000;//PA.4~7 推挽输出   	 
    GPIOA->ODR|=0X00;      //PA.4~7 输出低 //GPIOA->ODR|=0XF0;      //PA.4~7 输出高

	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X00033000;//PA.11~12 推挽输出   	 
    GPIOA->ODR|=0X00;      //PA.11~12 输出低 //GPIOA->ODR|=0X18;      //PA.11~12 输出高

	GPIOB->CRL&=0XFF0FFFFF; 
	GPIOB->CRL|=0X00300000;//PB.5 推挽输出   	 
    GPIOB->ODR|=1<<5;      //PB.5 输出高

	GPIOB->CRH&=0XF00FFFFF; 
	GPIOB->CRH|=0X03300000;//PB.13~14 推挽输出   	 
    GPIOB->ODR|=0X00;      //PB.13~14 输出低 //GPIOB->ODR|=0X60;      //PB.13~14 输出高
						
											  
	GPIOE->CRL&=0XFF0FFFFF;
	GPIOE->CRL|=0X00300000;//PE.5推挽输出
	GPIOE->ODR|=1<<5;      //PE.5输出高 

}
