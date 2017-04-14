#include "timer.h"
#include "led.h"
#include "usart.h"
#include "cont.h"
#include "motor.h"

u8 PWM1,PWM2,PWM3,PWM4;
//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
		if(PWM1!=0)
		{
			PUL1=~PUL1;
			PWM1--;
		}
		if(PWM2!=0)
		{
			PUL2=~PUL2;
			PWM2--;
		}
		if(PWM3!=0)
		{
			IPUL1=~IPUL1;
			PWM3--;
		}
		if(PWM4!=0)
		{
			IPUL2=~IPUL2;
			PWM4--;
		}			    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}


void TIM3_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值   
	TIM3->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQChannel,2);//抢占1，子优先级3，组2									 
}


u8 ov_frame; 	//统计帧数
//定时器6中断服务程序	 
void TIM6_IRQHandler(void)
{ 		    		  			    
	if(TIM6->SR&0X0001)//溢出中断
	{				    
//		printf("frame:%dfps\r\n",ov_frame);	//打印帧率
		ov_frame=0;		    				   				     	    	
	}				   
	TIM6->SR&=~(1<<0);//清除中断标志位 	    
}
//基本定时器6中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM6_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<4;//TIM6时钟使能    
 	TIM6->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM6->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟	  
	TIM6->DIER|=1<<0;   //允许更新中断	 
	TIM6->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM6_IRQChannel,2);//抢占1，子优先级3，组2									 
}

