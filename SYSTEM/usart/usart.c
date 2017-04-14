#include "sys.h"
#include "usart.h"	 
#include "led.h"
#include "lcd.h" 

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
/*串口1中断函数*/
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	  
//u8 Flag;  
void USART1_IRQHandler(void)
{
	u8 res;	
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntEnter();    
#endif
	if(USART1->SR&(1<<5))	//接收到数据
	{	 
		res=USART1->DR; 
//		printf("%f",5.0);
//		printf("%f",0.0);
		USART2->DR=res;
		while((USART2->SR&0X40)==0);//TC位置1说明发送完成 
//		if(res==' ')  LED1=!LED1;
//Flag=1;
//LED1=!LED1;
		
		if((USART1_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART1_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)
					USART1_RX_STA=0;//接收错误,重新开始	  //0x0a为换行符
				else 
				{
					USART1_RX_STA|=0x8000;	//接收完成了					
					USART2->DR = 0x0d;	
					while((USART2->SR&0X40)==0);
					USART2->DR = 0x0a;	
					while((USART2->SR&0X40)==0);
				}
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)	//0x0d为回车符
				{
					USART1_RX_STA|=0x4000;	   
					
				}
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=res;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))
						USART1_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  	 	 									     
	}
	 
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	


/*串口2中断函数*/
#if EN_USART2_RX   //如果使能了usart2接收
//串口2中断服务程序
//注意，读取USARTx->SR能避免莫名其妙的错误  	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲，最大USART2_REC_LEN个字节
//接收状态
//bit15，接收完成字节
//bit14，接收到0x0d
//bit13~0，接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记  
  
void USART2_IRQHandler(void)
{
	u8 res;	
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了，说明使用了ucosII了
	   OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		res=USART2->DR; 
	
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART2_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)
					USART2_RX_STA=0;//接收错误，重新开始
				else 
					{
						USART2_RX_STA|=0x8000;	 //接收完成
					} 
			}else //还没收到0X0D
			{	
				if(res==0x0d)
					USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))
						USART2_RX_STA=0;//接收数据错误，重新开始接收		  
				}		 
			}
		}		 									     
	}
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了，说明使用ucosII了.
	   OSIntExit();  		//不懂什么是ucosII									 
#endif
} 
#endif	


									 
//初始化IO 串口1 串口2
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart_init(u32 pclk2, u32 pclk1, u32 bound1,u32 bound2)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	

	//usart1   
	temp=(float)(pclk2*1000000)/(bound1*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction;
	
	 

/*使能usart1*/
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口1时钟
	 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
	GPIOA->CRH|=0X000008B0;//IO状态设置，PA9设置为50MHz的复用功能推挽输出模式，PA10为浮空输入模式
		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
#if EN_USART1_RX		  //如果使能了接收
	//使能接收中断
//	USART1->CR1|=1<<8;    //PE校验中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//组2，优先级高于usart2 
#endif





	/*使能usart2*/
	temp=(float)(pclk1*1000000)/(bound2*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 

	RCC->APB1ENR|=1<<17;  //使能串口2时钟

	
					//PA2/3 ，前面已经使能了PORTA的时钟
	GPIOA->CRL&=0XFFFF00FF;//IO状态设置
	GPIOA->CRL|=0X00008B00;/*IO状态设置 
							 PA2设置为50MHz的复用功能推挽输出模式 
						     PA3为上拉输入模式，不应该选为浮空输入*/
		  
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=mantissa; //波特率设置	 
	USART2->CR1|=0X200C;  //1位停止，8个数据位，无校验位

#if EN_USART2_RX		  //如果使能了接收
	//使能接收中断
//	USART2->CR1|=1<<8;    //PE中断使能
	USART2->CR1|=1<<5;    //接收缓冲区非空中断使能    	
	MY_NVIC_Init(3,3,USART2_IRQChannel,1);//组1，最低优先级 
#endif



}
