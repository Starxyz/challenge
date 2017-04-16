#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 	 	 
#include "key.h"	 	 	 	 
#include "lcd.h"	 
#include "ov7670.h"	
#include "timer.h"
#include "exti.h"
#include "cont.h"
#include "motor.h"
#include "pid.h"
#include "math.h"
#define  Motor1  0
#define  Motor2  1
#define  Motor3  2
#define  Motor4  3

pid ShiftMotor1,AngleMotor1;
//#define K 134

int  MoveByPid(pid *PID,int Setpoint,u8 motor)
{
			int dir=1, step = 0, tmp = 0;			
			tmp = PIDCale(PID, Setpoint);
			LCD_ShowString(90,50,50,16,16,"pid dir=");
			LCD_ShowNum(115,50, dir ,4,16);
			LCD_ShowString(90,30,50,16,16,"pid step=");
			LCD_ShowNum(115,30,step,4,16);
			if(tmp < 0)
			{
				dir = 0; //�ƶ�����������
				step = (-1)*tmp;
			}
			else 
				step = tmp;
  			MOTOR_Control(dir, motor, step);//��ֱ����Ļ���
		return 0;
}

			

	  
int main(void)
{	

	u8 id=0;
	u8 Key=0;       //ɨ�谴��
	u8 Key_Location=0; //ѡ����λ��
	u8 Loop1=1,Loop3=1;	//3��whileѭ��������
	
//	long int y_point,x_point;
	
	u8 length,i,k; //���ڷ�������
	u16 rec[10];
	u8 Motor_Num,Motor_Dir;
	u16 Motor_Step;				
//		 CMOS_x[6],CMOS_y[6],
 	Stm32_Clock_Init(9);	//ϵͳʱ������
	uart_init(72,36,256000,921600);	 	//���ڳ�ʼ��Ϊ921600  /256000

	delay_init(72);	   	 	//��ʱ��ʼ�� 
 	LCD_Init();			   	//��ʼ��LCD	
	KEY_Init();         	//��ʼ���밴�����ӵ�Ӳ���ӿ�
	CONT_Init();		//��ʼ����LED���ӵ�Ӳ���ӿ�
	TIM3_Init(100,7199);
	MOTOR_Init();



	if(lcddev.id==0X6804) 	//ǿ��������Ļ�ֱ���Ϊ320*240.��֧��3.5�����
	{
		lcddev.width=240;
		lcddev.height=320; 
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////


	POINT_COLOR=RED;		//��������Ϊ��ɫ 

//��ʾ��������											      
	LCD_ShowString(40,30,200,16,16,"Motor1");	      
	LCD_ShowString(40,60,200,16,16,"right");	      
	LCD_ShowString(40,90,200,16,16,"left");

	LCD_ShowString(130,30,200,16,16,"Motor2");	      
	LCD_ShowString(130,60,200,16,16,"down");	      
	LCD_ShowString(130,90,200,16,16,"up");


	LCD_ShowString(130,130,200,16,16,"Slideway2");	      
	LCD_ShowString(130,160,200,16,16,"up");	      
	LCD_ShowString(130,190,200,16,16,"down");

	LCD_ShowString(60,260,200,16,16,"ATUO");
		
//	LCD_ShowS(60,150,200,40,1);
    
	POINT_COLOR=BLUE;		//��������Ϊ��ɫ
	LCD_DrawRectangle(30,54,98,80);
	
	POINT_COLOR=WHITE; 
	LCD_DrawRectangle(30,84,98,110);
	LCD_DrawRectangle(30,154,98,180);
	LCD_DrawRectangle(30,184,98,210);
	LCD_DrawRectangle(120,54,188,80);
	LCD_DrawRectangle(120,84,188,110);
	LCD_DrawRectangle(120,154,188,180);
	LCD_DrawRectangle(120,184,188,210);
	LCD_DrawRectangle(30,224,98,250); 
	LCD_DrawRectangle(30,254,98,280);

	while(Loop1)  //LOOP1=1
	{	

//USART1->DR='0'+1;

		//�����ǲ��Խ�����������
		if(USART1_RX_STA&0x8000)
		{
			
			/*length = USART1_RX_STA & 0x3fff;
			for(i=0;i<length;i++)
			{	
				USART1->DR = i+1+'0' ;
				while((USART1->SR&0X40)==0);//TCλ��1˵���������
				rec[i]=USART1_RX_BUF[i];
				USART2->DR=rec[i];
				while((USART2->SR&0X40)==0);//TCλ��1˵���������
			}
			USART2->DR = 0x0d;
			while((USART2->SR&0X40)==0);//TCλ��1˵���������
			USART2->DR = 0x0a;
			while((USART2->SR&0X40)==0);//TCλ��1˵���������
			LED0=~LED0;	 */
			USART1_RX_STA=0;
		} 
	
// 	if(Flag)
// {				
// 			LED1=!LED1;
//			Flag=0;
// }
		Key=KEY_Scan(0);		//�õ���ֵ
		delay_ms(20);		//��������
//		LED1=0;
	   	if(Key)			
		{								   
			switch(Key)
			{				 
				case KEY_UP:
					{
						if(Key_Location > 0)
							Key_Location -= 1;
						else Key_Location = 7;
					}	
				
					break;
//����Ѿ����ˡ�������ֱ����ͨkey3.��
//				case KEY_LEFT:
//					{
//						if(Key_Location > 0)
//							Key_Location -= 1;
//						else Key_Location = 7;
//					}
//					break;

				case KEY_DOWN:		 
					{	
						POINT_COLOR=BLUE;		//��������Ϊ��ɫ

						if(Key_Location==0)	 // ���� +   y-  ��������
						{
//							k=USART1->DR;
						  MOTOR_Control(0,Motor1,5);		//ˮƽ����
						  LCD_ShowString(60,250,200,16,16,"1"); 
						  		
//						LCD_ShowNum(60,250,USART1_RX_BUF[0],16,16);
						}	     
						else if(Key_Location==1)// ����  -   y+  ��������
						{
						  MOTOR_Control(1,Motor1,5);
						  LCD_ShowString(60,250,200,16,16,"2"); 
						} 

						else if(Key_Location==2)  // ����  +   x+  ��������
						{
						   MOTOR_Control(0,Motor2,5);			  //��ֱ����
						   LCD_ShowString(60,250,200,16,16,"3");
						}	
						else if(Key_Location==3)   // ����  -   x-  ��������
						{
						  MOTOR_Control(1,Motor2,5);
						  LCD_ShowString(60,250,200,16,16,"4");
						}

						else if(Key_Location==4)			//��������
						{
						  MOTOR_Control(0,Motor3,100);			    //��ֱ��������������
						  LCD_ShowString(60,250,200,16,16,"17");
						}	 
						else if(Key_Location==5)					//����
						{
						  MOTOR_Control(1,Motor3,100);
						  LCD_ShowString(60,250,200,16,16,"18");
						}	 
						else if(Key_Location==6)
						{
							Loop1=0;	//��������ɨ��
						    Loop3=0;    //������auto������show
						}
						else if(Key_Location==7)
						{
						  Loop1=0;	  //��������ɨ��
						}
				   //test
					//	while((USART2_RX_STA&0x8000) == 0)		  //һֱ�ڷ��͡�ֱ������������
//						{
//					   	  	 USART2->DR=0x0D;//�ж��Ƿ����
//							 while((USART2->SR&0X40)==0);//TCλ��1˵���������   
//							 USART2->DR=0x0A;		 //�ӻ����յ�0x11��ˢ������
//							 while((USART2->SR&0X40)==0);//TCλ��1˵���������
//							 delay_ms(400);							 						     
//						}
					}
						break; 

				
				case KEY_RIGHT:	
					{
						if(Key_Location<7)
						   Key_Location += 1;
						else Key_Location=0;
					}
					break;
		    	}    

//��Ϊ�������ɨ���Ѿ���������������������ж���ˢ��ͼ��
	
		if(Key_Location==0)
		{
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ
			//����
			LCD_DrawRectangle(30,54,98,80);
			
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250);
			LCD_DrawRectangle(30,254,98,280); 
	
	
		}
		if(Key_Location==1)
		{
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ
			LCD_DrawRectangle(30,84,98,110);
			
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250); 
			LCD_DrawRectangle(30,254,98,280);
		}
		if(Key_Location==2)
		{
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ
			LCD_DrawRectangle(120,54,188,80);
			
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250);
			LCD_DrawRectangle(30,254,98,280); 
		}
		if(Key_Location==3)
		{
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ			
			LCD_DrawRectangle(120,84,188,110);
	
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250); 
			LCD_DrawRectangle(30,254,98,280);		
		}
				

		if(Key_Location==4)
		{
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ
			LCD_DrawRectangle(120,154,188,180);			
			POINT_COLOR=WHITE; 	
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);			
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250);
			LCD_DrawRectangle(30,254,98,280); 
					
		}
		if(Key_Location==5)
		{
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ
			LCD_DrawRectangle(120,184,188,210);
			
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);			
			LCD_DrawRectangle(30,224,98,250); 
			LCD_DrawRectangle(30,254,98,280);
					
		}
		if(Key_Location==7)
		{
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ
			LCD_DrawRectangle(30,254,98,280);
			
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250);
			 
					
		}

		else delay_ms(10); 
 	 }	 

   }

	LCD_Clear(WHITE);	//����
			    
 	POINT_COLOR=RED;    //��������Ϊ��ɫ  

	while(Loop3)   //auto
	{		
        delay_ms(500);					
		id++;
 		if(id==5)//DS0��˸	������ʾ�Ƿ�����ѭ��������
		{
			id=0;
			LED0=!LED0;
 		}			
		


		if(USART1_RX_STA&0x8000)
		{
			//LED1=~LED1;
//			LED0=~LED0;
			LCD_Clear(WHITE);
			length = USART1_RX_STA & 0x3fff;
			for(i=0;i<length;i++)
			{
				rec[i]=USART1_RX_BUF[i];
			} 	
			USART1_RX_STA=0;

			Motor_Num = rec[0];
			Motor_Dir = rec[1];
			Motor_Step = rec[2]+(rec[3]<<8);
//			Motor_Step = rec[2];
			if(Motor_Num==0x00)
			{
				LCD_ShowString(60,100,200,16,16,"Motor:Horizonal");

				if(Motor_Dir==0x01)
				{
					 LCD_ShowString(60,116,200,16,16,"Motor_Dir: Left");
				}
				else LCD_ShowString(60,116,200,16,16,"Motor_Dir:Right");  

			}	 
			else 
			{
				LCD_ShowString(60,100,200,16,16,"Motor:Vertical");
				if(Motor_Dir==0x01)
				{
					LCD_ShowString(60,116,200,16,16,"Motor_Dir:down");
				}
				else LCD_ShowString(60,116,200,16,16,"Motor_Dir:  up");
			}

			LCD_ShowString(60,132,200,16,16,"Motor_Step:");
			LCD_ShowxNum(45,132,Motor_Step,20,16,1);

			MOTOR_Control(Motor_Dir,Motor_Num,Motor_Step);
			//USART2->DR='e';
//	while((USART1->SR&0X40)==0);//�ȴ��������
			LED1=!LED1;
			

		}




/*		PIDInit(&ShiftMotor1);	  //y��
		PIDInit(&AngleMotor1);	  //x��
//�����ͷ�õ��Ĺ����������
				
		if(USART1_RX_STA&0x8000)
		{
			length = USART1_RX_STA & 0x3fff;
			for(i=0;i<length;i++)
			{
				rec[i]=USART1_RX_BUF[i];
			}
			for(i=0;i<6;i++)
			{
				CMOS_x[i]=rec[i];
			}
			for(i=0;i<6;i++)
			{
				CMOS_y[i]=rec[i+6];
			}
			USART1_RX_STA=0;
			x_point = CMOS_x[1]*10000+CMOS_x[2]*1000+CMOS_x[3]*100+CMOS_x[4]*10+CMOS_x[5];
			y_point = CMOS_y[1]*10000+CMOS_y[2]*1000+CMOS_y[3]*100+CMOS_y[4]*10+CMOS_y[5];

			if(CMOS_x[0]==0x02)
				x_point = (-1) * x_point;

			if(CMOS_y[0]==0x02)
				y_point = (-1) * y_point;



//			if(y_point>0)
//			MOTOR_Control(1,Motor4,100);			
//			else
			while(abs(y_point)>30)
			{ 
				MOTOR_Control(0,Motor4,100);

			if(USART1_RX_STA&0x8000)
			{
			length = USART1_RX_STA & 0x3fff;
			for(i=0;i<length;i++)
			{
				rec[i]=USART1_RX_BUF[i];
			}
			for(i=0;i<6;i++)
			{
				CMOS_y[i]=rec[i+6];
			}
			USART1_RX_STA=0;
			y_point = CMOS_y[1]*10000+CMOS_y[2]*1000+CMOS_y[3]*100+CMOS_y[4]*10+CMOS_y[5];


			if(CMOS_y[0]==0x02)
				y_point = (-1) * y_point;
			}
			}
//			while(abs(y_point)<30)
//			{		if(USART1_RX_STA&0x8000)
//				{
//				length = USART1_RX_STA & 0x3fff;
//				for(i=0;i<length;i++)
//				{
//					rec[i]=USART1_RX_BUF[i];
//				}
//				for(i=0;i<6;i++)
//				{
//					CMOS_x[i]=rec[i];
//				}
//				for(i=0;i<6;i++)
//				{
//					CMOS_y[i]=rec[i+6];
//				}
//				USART1_RX_STA=0;
//				y_point = CMOS_y[1]*10000+CMOS_y[2]*1000+CMOS_y[3]*100+CMOS_y[4]*10+CMOS_y[5];
//				if(CMOS_y[0]==0x02)
//					y_point = (-1) * y_point;
//				}
//			}

			if(x_point>0)
			MOTOR_Control(1,Motor1,1);
			else MOTOR_Control(0,Motor1,1);
			while(abs(x_point)<30)
			{		
				if(USART1_RX_STA&0x8000)
				{
				length = USART1_RX_STA & 0x3fff;
				for(i=0;i<length;i++)
				{
					rec[i]=USART1_RX_BUF[i];
				}
				for(i=0;i<6;i++)
				{
					CMOS_x[i]=rec[i];
				}
				for(i=0;i<6;i++)
				{
					CMOS_y[i]=rec[i+6];
				}
				USART1_RX_STA=0;
				x_point = CMOS_x[1]*10000+CMOS_x[2]*1000+CMOS_x[3]*100+CMOS_x[4]*10+CMOS_x[5];
				if(CMOS_x[0]==0x02)
					x_point = (-1) * x_point;
				}
			} 
			while(abs(y_point)>50)
			{
			   	ShiftMotor1.NowPoint = y_point;		
				MoveByPid(&ShiftMotor1,0,Motor4);//PID������ֱ����
				LED0=0;
				LED1=1;
			}
			while(abs(x_point)>50)
			{
				AngleMotor1.NowPoint = x_point;
			    MoveByPid(&AngleMotor1,0,Motor1);//PID����ˮƽ����
				LED1=0;
				LED0=1;
			}
		
		 }*/						
	} 
		   
}

