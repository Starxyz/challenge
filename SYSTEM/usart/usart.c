#include "sys.h"
#include "usart.h"	 
#include "led.h"
#include "lcd.h" 

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
/*����1�жϺ���*/
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���	  
//u8 Flag;  
void USART1_IRQHandler(void)
{
	u8 res;	
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART1->SR&(1<<5))	//���յ�����
	{	 
		res=USART1->DR; 
//		printf("%f",5.0);
//		printf("%f",0.0);
		USART2->DR=res;
		while((USART2->SR&0X40)==0);//TCλ��1˵��������� 
//		if(res==' ')  LED1=!LED1;
//Flag=1;
//LED1=!LED1;
		
		if((USART1_RX_STA&0x8000)==0)//����δ���
		{
			if(USART1_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)
					USART1_RX_STA=0;//���մ���,���¿�ʼ	  //0x0aΪ���з�
				else 
				{
					USART1_RX_STA|=0x8000;	//���������					
					USART2->DR = 0x0d;	
					while((USART2->SR&0X40)==0);
					USART2->DR = 0x0a;	
					while((USART2->SR&0X40)==0);
				}
			}
			else //��û�յ�0X0D
			{	
				if(res==0x0d)	//0x0dΪ�س���
				{
					USART1_RX_STA|=0x4000;	   
					
				}
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=res;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))
						USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  	 	 									     
	}
	 
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif	


/*����2�жϺ���*/
#if EN_USART2_RX   //���ʹ����usart2����
//����2�жϷ������
//ע�⣬��ȡUSARTx->SR�ܱ���Ī������Ĵ���  	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ��壬���USART2_REC_LEN���ֽ�
//����״̬
//bit15����������ֽ�
//bit14�����յ�0x0d
//bit13~0�����յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���  
  
void USART2_IRQHandler(void)
{
	u8 res;	
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD�����ˣ�˵��ʹ����ucosII��
	   OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=USART2->DR; 
	
		if((USART2_RX_STA&0x8000)==0)//����δ���
		{
			if(USART2_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)
					USART2_RX_STA=0;//���մ������¿�ʼ
				else 
					{
						USART2_RX_STA|=0x8000;	 //�������
					} 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)
					USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))
						USART2_RX_STA=0;//�������ݴ������¿�ʼ����		  
				}		 
			}
		}		 									     
	}
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD�����ˣ�˵��ʹ��ucosII��.
	   OSIntExit();  		//����ʲô��ucosII									 
#endif
} 
#endif	


									 
//��ʼ��IO ����1 ����2
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init(u32 pclk2, u32 pclk1, u32 bound1,u32 bound2)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	

	//usart1   
	temp=(float)(pclk2*1000000)/(bound1*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction;
	
	 

/*ʹ��usart1*/
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���1ʱ��
	 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬���ã�PA9����Ϊ50MHz�ĸ��ù����������ģʽ��PA10Ϊ��������ģʽ
		  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
#if EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
//	USART1->CR1|=1<<8;    //PEУ���ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//��2�����ȼ�����usart2 
#endif





	/*ʹ��usart2*/
	temp=(float)(pclk1*1000000)/(bound2*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 

	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���2ʱ��

	
					//PA2/3 ��ǰ���Ѿ�ʹ����PORTA��ʱ��
	GPIOA->CRL&=0XFFFF00FF;//IO״̬����
	GPIOA->CRL|=0X00008B00;/*IO״̬���� 
							 PA2����Ϊ50MHz�ĸ��ù����������ģʽ 
						     PA3Ϊ��������ģʽ����Ӧ��ѡΪ��������*/
		  
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; //����������	 
	USART2->CR1|=0X200C;  //1λֹͣ��8������λ����У��λ

#if EN_USART2_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
//	USART2->CR1|=1<<8;    //PE�ж�ʹ��
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��    	
	MY_NVIC_Init(3,3,USART2_IRQChannel,1);//��1��������ȼ� 
#endif



}
