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
#define  Motor1  0
#define  Motor2  1
#define  Motor3  2
#define  Motor4  3
extern u8 ov_sta;	//在exit.c里面定义
extern u8 ov_frame;	//在timer.c里面定义

//#include "usmart.h"
//const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};							//5种光照模式	    
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 





u32 Quadrant1=0,Quadrant2=0,Quadrant3=0,Quadrant4=0; //四个象限的数据
long int x0=0,y0=0,pac_x_gray=0,pac_y_gray=0; //精准算法的 x、y
int Precise_x,Precise_y;  //原程序的xc和yc
//u32 sumx=0,sumy=0;
//int Precise_y=0,Precise_x=0;  //精确法收数据的坐标
//u8 m,n;	//用在精确法收坐标



pid ShiftMotor1, ShiftMotor2;
#define K 134
//u32 rec;      以前调试用的
//u8 k=0;	    以前调试用的
//u16 xy[2][5]; 以前调试用的
//FILE *fp2;




/* ***************************************************** */
// 函数名称：color_Gray
// 函数功能：将color转换为rgb形式再转换为gray（灰度）
// 入口参数：color_rg  是读取到的数据
// 出口参数：无
/* ***************************************************** */
u16 color_Gray(u16 color_rg) 
{
	u8 r,gray;	
//	b=(color_rg>>0)&0x1f;
//	g=(color_rg>>5)&0x3f;
	r=(color_rg>>11)&0x1f;
	gray=r;
//	gray =(r*28+g*151+b*77)>>8;	
//	gray=(r*30+g*59+b*11)/100;
    return gray;	
}
/* ***************************************************** */
// 函数名称：four_quadrant
// 函数功能：四象限法收集数据
// 入口参数： *Point_gray使四象限获得数值,*x, *y用于扫描？
// 出口参数：无
/* ***************************************************** */
void four_quadrant(u16 *Point_gray,u16 *Four_x,u16 *Four_y)//四象限法收集数据
{
    
	if(*Four_y>=240)   //320 *２４０
	*Four_y=0;				 
	if(*Four_x>=320)
	{										
		*Four_x=0;
	    *Four_y++;			   //按行扫描？
	}
	*Four_x++;
	if(*Four_y<120)	//因为是320*240，所以160，120是分界点
	{
		if(*Four_x<160) Quadrant2+=*Point_gray;
	    else Quadrant1+=*Point_gray;
    }
	else
	{
		if(*Four_x<160) Quadrant3+=*Point_gray;
	    else Quadrant4+=*Point_gray;
    }
}
/* ***************************************************** */
// 函数名称：our_quadrant_coordinate
// 函数功能：四象限法定坐标
// 入口参数：*m、*n是显示负号的条件
//   		 *sumx、*sumy是算法中的分母	 
// 出口参数：无
/* ***************************************************** */
void four_quadrant_coordinate(u8 *m,u8 *n)//四象限法定坐标
{
        x0=(Quadrant1+Quadrant4-Quadrant2-Quadrant3);
        y0=(Quadrant1+Quadrant2-Quadrant3-Quadrant4);
				
		if(x0<0) 
		{
		  x0=x0*(-1);
		  *n=1;
		 }	
		else
		  *n=0;
//		sumx=x0;
//		x0=7059*x0/(sum1+sum2+sum3+sum4);
		x0=x0/(Quadrant1+Quadrant2+Quadrant3+Quadrant4);
//		if(x0>120) x0=0;  	
		if(y0<0)
		 {
		  y0=y0*(-1);
		  *m=1;
		 }	 
		else 
		  *m=0;
//		sumy=y0;
//		y0=720*y0/(sum1+sum2+sum3+sum4);
		y0=y0/(Quadrant1+Quadrant2+Quadrant3+Quadrant4);
//		if(y0>120) y0=0;
}

/* ***************************************************** */
// 函数名称：precise_algorithm
// 函数功能：精确算法收集数据，用指针进行值传递以减少全局变量的使用
// 入口参数：*Precise_x，*Precise_y 坐标，*Point_gray与传感器有关的值 *pac算法的分母
// 出口参数：无	  
/* ***************************************************** */
//基本思想就是四象限法
void precise_algorithm(u16 *Point_gray,u32 *Total_gray)
{	
	if(Precise_x>=320)   
		{
		  Precise_x=0;	   
		  Precise_y++;	   //为什么要y++
		}
	if(Precise_y>=240)    //pac01又是什么 gray1呢。
	   {
	   	  Precise_y=0;
	   }
	   		  
    pac_x_gray += *Point_gray*(Precise_x-160); //因为是320*240，所以160，120是分界点
	pac_x_gray += *Point_gray*(Precise_y-120);
	*Total_gray += *Point_gray;			 
	Precise_x++;			
}


/* ***************************************************** */
// 函数名称：precise_algorithm_coordinate
// 函数功能：精确算法定坐标
// 入口参数：*m、*n是显示负号的条件
//   		 *pac、*sumx、*sumy是算法中的分母	 
// 出口参数：无
/* ***************************************************** */
void precise_algorithm_coordinate(u8 *m,u8 *n,u32 *Total_gray)//精确算法定坐标
{
	if(pac_x_gray<30000&&pac_x_gray>-30000) 
	{
		pac_x_gray=0;
	}
	if(pac_x_gray<30000&&pac_x_gray>-30000)
	{
		pac_x_gray=0;
	}

	x0=1*pac_x_gray;	  
    y0=1*pac_x_gray;
			
	if(x0<0)
	{
		x0 *= (-1);
		*n=1;
	}	 
	else  
	*n=0;

//	sumx = x0;
	x0 /= *Total_gray;
		
	if(y0<0) 
	{
		y0 *= (-1);
		*m=1;
	}
	else  *m=0;

//	sumy=y0;
	y0 /= *Total_gray;
	    
	*Total_gray=pac_x_gray=pac_x_gray=0;	
}

/* ***************************************************** */
// 函数名称：camera_refresh
// 函数功能：更新LCD显示
// 入口参数：*m、*n是显示负号的条件
//   		 *sumx、*sumy是算法中的分母  都是用于值传递
//			 *x	 *y//刷新就清零。
// 出口参数：Point_gray 读取的传感器数据
/* ***************************************************** */

void camera_refresh(u8 *m,u8 *n,u16 *Four_x,u16 *Four_y)
{
	
	u32 Total_gray;   //所有象限的灰度值总和
	u16 Point_gray; //读取的数据
//	int Precise_y=0,Precise_x=0;  //精确法收数据的坐标
	u32 j;//循环条件
//	u32 aa;	  
 	u16 color; 
	*Four_x=*Four_y=0;                        //四象限法的坐标
////////////	Precise_x=0;Precise_y=0;	//精确算法
	Quadrant1=Quadrant2=Quadrant3=Quadrant4=0;
	Total_gray=pac_x_gray=pac_x_gray=0; 
//	USART1->DR =0x00;  //先清空串口	
//	while(aa!=0x11) aa=USART1->DR;  //接收到信号才开始发送

	
	ov_sta=0;					//开始下一次采集
	while(ov_sta != 2);


	if(ov_sta == 2)
	{
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右 
		LCD_SetCursor(0x00,0x0000);	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_CS=0;
		OV7670_RRST=0;				//开始复位读指针  			
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK=1;  
		for(j=0;j<76800;j++)
		{
			OV7670_RCK=0; 			
			color=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK=1; 
			color<<=8;    
			OV7670_RCK=0;				
			color|=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK=1; 
			LCD->LCD_RAM=color;
			Point_gray=color_Gray(color);
			if(Point_gray<=11) //默认该像素点无数据
			 	Point_gray=0;
//循环发送像素数据
/*	
		if(w==0) //溢出时不传输数据
			{
				printf("%d", gray1);
				//USART1->DR =gray1;
            	//while((USART1->SR&0X40)==0);//循环发送,直到发送完毕      	
				if(h>=319)			  
				{ 
					h=0;
			    //	USART1->DR =0xff;
                //	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕 
				}	
				h++;
			}		    
  */
      	    four_quadrant(&Point_gray,Four_x,Four_y);//四象限法收集数据
			precise_algorithm(&Point_gray,&Total_gray);//精确算法收集数据		   
		}

		if(Quadrant1<1000&&Quadrant2<1000&&Quadrant3<1000&&Quadrant4<1000)
		 Quadrant1=Quadrant2=Quadrant3=Quadrant4=0;

//  	four_quadrant_coordinate(m,n);//四象限法定坐标
		precise_algorithm_coordinate(m,n,&Total_gray);//精确算法定坐标
									//不加地址的四个是因为，已经定义成指针	   							 
		EXTI->PR=1<<8;     			//清除LINE8上的中断标志位
//		ov_sta=0;					//开始下一次采集
 		ov_frame++; 
        LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	} 
}

/* ***************************************************** */
// 函数名称：camera_2_refresh
// 函数功能：更新LCD显示
// 入口参数：*m、*n是显示负号的条件
//   		 *sumx、*sumy是算法中的分母  都是用于值传递	 
// 出口参数：无
/* ***************************************************** */
void camera_2_refresh(u8 *m,u8 *n,u16 *Four_x,u16 *Four_y)
{		
//  u8 flagxy=0;
  static u8 slave[16] = {0};  //用于串口接收数据
  u8 z;	
  u16 Length;//接收到的数据长度
//  u8 flag1=0;
  u8 Camera2_X_Num, Camera2_Y_Num;
  long int Camera2_X, Camera2_Y;
//  u32  step = 0;
//  long int tmp = 0;
//  u8   a=0;
  char b[32]={0};
		if(x0<=2&&y0<=2&&(!(( *Four_x == 0 )&&( *Four_y == 0 )))) // gai
		{
		  POINT_COLOR=GREEN;// 设置字体为绿字
	      LCD_ShowString(38,100,200,16,16,"Spot center in CMOS-2");
	      LCD_ShowString(30,120,200,16,16,"emulsion coordinates :");
//		  uart_init(72,9600);
		//	USART2->DR = 0x00;
		while((USART2_RX_STA&0x8000) == 0)
		{
	     	 USART2->DR=0x0D;//判断是否结束
			 while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
			 USART2->DR=0x0A;		 //从机接收到0x11才刷新数据
			 while((USART2->SR&0X40)==0);//循环发送,直到发送完毕
			 delay_ms(400);
		}
		    	
			//LCD_ShowString(90,160,50,16,16,"USART2_RX_STA IS 0X8000");				   
			  //printf("break1\n");
			  Length=USART2_RX_STA&0x3FFF;//得到此次接收到的数据长度
			  //printf("len = %d\n", len);
			  for(z = 0; z < Length; z++)
			   {
				/*****************************************/
				slave[z]=USART2_RX_BUF[z] - 48;	  //48是‘0’
			//	printf("%d", slave[z]);	
				/****************************************/
				while((USART2->SR&0X40)==0);//等待发送接收
				
			   }
			 // printf("\n");
			      
	
			  Camera2_X_Num = slave[0];	//cmos的正负号
			  Camera2_X     = slave[1]*1000 + slave[2]*100 + slave[3]*10 + slave[4];
			                   //数据缩放
			  Camera2_Y_Num = slave[5];
			  Camera2_Y     = slave[6]*1000 + slave[7]*100 + slave[8]*10 + slave[9];	
							 
	//		  printf("Camera2_X_Num=%d, Camera2_X=%ld, Camera2_Y_Num=%d, Camera2_Y=%ld\n",Camera2_X_Num,Camera2_X,Camera2_Y_Num,Camera2_Y);
				

			    LCD_ShowNum(115,160,Camera2_X,6,16);
					
		        if(Camera2_X_Num == 1 && Camera2_X > 0)
				LCD_ShowString(90,160,50,16,16,"S1: -");

		        else LCD_ShowString(90,160,50,16,16,"S1:  ");

			    LCD_ShowNum(115,180,Camera2_Y,6,16);		
		        if(Camera2_Y_Num == 1 && Camera2_Y > 0) 
				LCD_ShowString(90,180,55,16,16,"S2: -");
	        	else LCD_ShowString(90,180,50,16,16,"S2:  ");
				delay_ms(3);
/*			for(flag=0;flag<10;flag++)
			{
			 printf("\r\n%ld\r\n",slave[flag]);//插入换行
			}	   */
			  USART2_RX_STA=0;
			  delay_ms(1);
       
	
		
/*发送camera1和camera2的中心坐标给QT*/
		
		sprintf(b, "STM%ld%04ld%ld%04ld%ld%04ld%ld%04ld\n", *n, x0, *m, y0,  Camera2_X_Num, Camera2_X, Camera2_Y_Num, Camera2_Y);
		printf("%s",b);
		
	   	if(Camera2_X <= 200 && Camera2_Y <=200)
		{
		//	while(1);
		}
  


		   /********************************/ 			
	//		 if(rec==slave[1]) break;//flag=1;		 //不能只从x来判断是否相等，应该设一个误差范围
			/*******************************/
	//		 else rec=slave[1];
		
	//	POINT_COLOR=GREEN;// 设置字体为绿色
	//	flag=0;	 
//       while(1)
//	    {

//	   if(flagxy==0)
//	  {
  /*
       //基于PID的调节方法
		//MoveByPid(&ShiftMotor1);
	  	if(Camera2_X < 100)
	  	{
			//	printf("Camera2_X = %ld\n", Camera2_X);
	  	}
	  	else
	  	{
			 if( Camera2_X_Num == 1 && Camera2_X > 600)
			{
			   //printf("Camera2_X = %ld\n", Camera2_X);
			   MOTOR_Control(1, 3, 250);
			   MOTOR_Control(1, 3, 250);
			 	//  MOTOR_Control(1, 3, 250);
				//   MOTOR_Control(a, 3, 250);
			}
			else if(Camera2_X > 600)
			{	
			   //printf("Camera2_X = %ld\n", Camera2_X);	 
			   MOTOR_Control(0, 3, 250);
			   MOTOR_Control(0, 3, 250);
			 //  MOTOR_Control(0, 3, 250);
			}
			else
			{
				 Camera2_X = Camera2_X * 100 / K;
			 	if(Camera2_X_Num == 1)
				{
					tmp = PIDCale(&ShiftMotor2, ((-1) * Camera2_X) );
				}
				else
		    		tmp = PIDCale(&ShiftMotor2, (Camera2_X) );
			
				if(tmp < 0)
				{
					a = 1; //移动的正反方向
					step = -tmp;
				}
				else
				{														 
					a = 0; 
					step = tmp ;
				}
			
			//	if(step > 100)				
			//	{
				//	printf("step = %d\n", step);
					//step /= K;
					//printf("step / k = %d\n", step);
					while( step > 250)
					{				
						MOTOR_Control(a, 3, 250);
						step -= 250;
					}
				    MOTOR_Control(a, 3, step);
					    step = 0;	
			//	}
			}
	    }	

*/
//			LCD_ShowString(90,50,50,16,16,"pid dir=");
//			LCD_ShowNum(115,50, a ,6,16);
//			LCD_ShowString(90,30,50,16,16,"pid step=");
//			LCD_ShowNum(115,30,step,6,16);


		if(Camera2_X_Num == 1 && Camera2_X >=1000 )//x坐标
		{
		   MOTOR_Control(1,3,200); 
		   //MOTOR_Control(0,2); shift1
		}	     
		else if(Camera2_X_Num == 1 && Camera2_X >= 200)
		MOTOR_Control(1,3,50);
		else if(Camera2_X >= 1000)//
		{
	    	MOTOR_Control(0,3,200);	
			//MOTOR_Control(1,2); -   shift1
		}	 
		else if(Camera2_X >= 200)
		MOTOR_Control(0,3,50);	 
		//MOTOR_Control(1,2); -   shift1

		delay_ms(1000); //试一下能不能去掉

//		flagxy=1;
//		camera_2_refresh();
//		break;
 //	  }
	
//	  if(flagxy==1)
//	   {
/*        if(slave[2+slave[1]]==1&&slave[4+slave[1]]>=10){MOTOR_Control(1,2,200);}	   //MOTOR_Control(1,0); -   y+  右 
		else if(slave[2+slave[1]]==1&&slave[4+slave[1]]>=2)MOTOR_Control(1,2,50); 
		else if(slave[4+slave[1]]>=10){MOTOR_Control(0,2,200);}      //MOTOR_Control(0,0); +   y-  左
		else if(slave[4+slave[1]]>=2)MOTOR_Control(0,2,50);
*/		
//		flagxy=0;
//	   }
//			printf("\r\n%ld%ld%ld%ld\r\n",slave[0],slave[1],slave[2],slave[3]);//插入换行
  //          delay_ms(500); 
 //        } 
/*       while(1){
		   	    LCD_ShowNum(115,160,slave[2],4,16);	
		        if(slave[0]==1&&slave[2]>=1) LCD_ShowString(90,160,50,16,16,"S1: -");
		        else LCD_ShowString(90,160,50,16,16,"S1:  ");
			    LCD_ShowNum(115,180,(slave[4+slave[1]]),4,16);		
		        if(slave[2+slave[1]]==1&&slave[4+slave[1]]>=1) LCD_ShowString(90,180,55,16,16,"S2: -");
	        	else LCD_ShowString(90,180,50,16,16,"S2:  ");
				delay_ms(300);
				 }		 */
//		  uart_init(72,921600);
          camera_refresh(m,n,Four_x,Four_y);   //   因为调节了导轨，所以要重新刷新cmos1
	 //   break;
		    
		  
	
		}


}					

	  
int main(void)
{	
	u16 Four_x=0,Four_y=0;  //用于四象限收数据
	u8 m,n;       //用于判断是否显示负号,-x和-y
	u8 id=0;
	u8 Overflow;  //溢出标志
	u8 Key;       //扫描按键
	u8 Key_Location=0; //选择框的位置
	u8 Loop1=1,Loop2=1,Loop3=1;	//3个while循环的条件
	
		
 	Stm32_Clock_Init(9);	//系统时钟设置
	uart_init(72,36,921600);	 	//串口初始化为921600
	delay_init(72);	   	 	//延时初始化 
//	LED_Init();		  		//初始化与LED连接的硬件接口
 	LCD_Init();			   	//初始化LCD	
	KEY_Init();         	//初始化与按键连接的硬件接口
	CONT_Init();		//初始化与LED连接的硬件接口
	TIM3_Init(100,7199);
	MOTOR_Init();
  	OV7670_Init();

//	PIDInit( &ShiftMotor1 , 0 );
	PIDInit( &ShiftMotor2 , 0 );

	if(lcddev.id==0X6804) 	//强制设置屏幕分辨率为320*240.以支持3.5寸大屏
	{
		lcddev.width=240;
		lcddev.height=320; 
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////


	POINT_COLOR=RED;		//设置字体为红色 

//显示操作界面											      
	LCD_ShowString(40,30,200,16,16,"Motor1");	      
	LCD_ShowString(40,60,200,16,16,"+");	      
	LCD_ShowString(40,90,200,16,16,"-");
	LCD_ShowString(130,30,200,16,16,"Motor2");	      
	LCD_ShowString(130,60,200,16,16,"+");	      
	LCD_ShowString(130,90,200,16,16,"-");
	LCD_ShowString(40,130,200,16,16,"Slideway1");	      
	LCD_ShowString(40,160,200,16,16,"+");	      
	LCD_ShowString(40,190,200,16,16,"-");
	LCD_ShowString(130,130,200,16,16,"Slideway2");	      
	LCD_ShowString(130,160,200,16,16,"+");	      
	LCD_ShowString(130,190,200,16,16,"-");

	
	LCD_ShowString(60,230,200,16,16,"SHOW");
	LCD_ShowString(60,260,200,16,16,"ATUO");
		
//	LCD_ShowS(60,150,200,40,1);
    
	POINT_COLOR=BLUE;		//设置字体为蓝色
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

	while(Loop1)
	{	
	
		Key=KEY_Scan(0);		//得到键值
	   	if(Key)			
		{						   
			switch(Key)
			{				 
				case KEY_UP:	
				
					break;
				case KEY_LEFT:
					{
						if(Key_Location > 0)
							Key_Location -= 1;
						else Key_Location = 9;
						}
					break;
				case KEY_DOWN:		 
					{	
						POINT_COLOR=BLUE;		//设置字体为蓝色

						if(Key_Location==0)	 // 按了 +   y-  光线左移
						{
						  MOTOR_Control(0,Motor1,5);
						  LCD_ShowString(60,250,200,16,16,"1");
						}	     
						else if(Key_Location==1)// 按了  -   y+  光线右移
						{
						  MOTOR_Control(1,Motor1,5);
						  LCD_ShowString(60,250,200,16,16,"2"); 
						} 
						else if(Key_Location==2)  // 按了  +   x+  光线下移
						{
						   MOTOR_Control(0,Motor2,5);
						   LCD_ShowString(60,250,200,16,16,"3");
						}	
						else if(Key_Location==3)   // 按了  -   x-  光线上移
						{
						  MOTOR_Control(1,Motor2,5);
						  LCD_ShowString(60,250,200,16,16,"4");
						}	
						else if(Key_Location==4)
						{
						  MOTOR_Control(0,Motor3,100);
						  LCD_ShowString(60,250,200,16,16,"5");
						}	   
						else if(Key_Location==5)
						{
						  MOTOR_Control(1,Motor3,100);
						  LCD_ShowString(60,250,200,16,16,"6");
					    } 
						else if(Key_Location==6)
						{
						  MOTOR_Control(0,Motor4,100);
						  LCD_ShowString(60,250,200,16,16,"7");
						}	 
						else if(Key_Location==7)
						{
						  MOTOR_Control(1,Motor4,100);
						  LCD_ShowString(60,250,200,16,16,"8");
						}	 
						else if(Key_Location==8)
						{
							Loop1=0;	//结束键盘扫描
						    Loop3=0;   //结束“auto”
						}
						else if(Key_Location==9)
						{
						  Loop1=0;	  //结束键盘扫描
						  Loop2=0; 	  //结束“show”
						}
					}
						break; 
						
				case KEY_RIGHT:	
					{
						if(Key_Location<9)
						   Key_Location += 1;
						else Key_Location=0;
					}
					break;
			}

//因为上面键盘扫描已经给电机发送了命令，下面的判断是刷新图像
	
		if(Key_Location==0)
		{
			POINT_COLOR=BLUE;		//设置字体为蓝色
			//画框
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
			POINT_COLOR=BLUE;		//设置字体为蓝色
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
			POINT_COLOR=BLUE;		//设置字体为蓝色
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
			POINT_COLOR=BLUE;		//设置字体为蓝色			
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
			POINT_COLOR=BLUE;		//设置字体为蓝色
			LCD_DrawRectangle(30,154,98,180);
			
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,84,98,110);			
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250);
			LCD_DrawRectangle(30,254,98,280); 
					
		}
		if(Key_Location==5)
		{
			POINT_COLOR=BLUE;		//设置字体为蓝色
			LCD_DrawRectangle(30,184,98,210);
			
			POINT_COLOR=WHITE;
			LCD_DrawRectangle(30,54,98,80); 
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);			
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,224,98,250);
			LCD_DrawRectangle(30,254,98,280); 
					
		}
		if(Key_Location==6)
		{
			POINT_COLOR=BLUE;		//设置字体为蓝色
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
		if(Key_Location==7)
		{
			POINT_COLOR=BLUE;		//设置字体为蓝色
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
		if(Key_Location==8)
		{
			POINT_COLOR=BLUE;		//设置字体为蓝色
			LCD_DrawRectangle(30,224,98,250);
			
			POINT_COLOR=WHITE; 
			LCD_DrawRectangle(30,54,98,80);
			LCD_DrawRectangle(30,84,98,110);
			LCD_DrawRectangle(30,154,98,180);
			LCD_DrawRectangle(30,184,98,210);
			LCD_DrawRectangle(120,54,188,80);
			LCD_DrawRectangle(120,84,188,110);
			LCD_DrawRectangle(120,154,188,180);
			LCD_DrawRectangle(120,184,188,210);
			LCD_DrawRectangle(30,254,98,280);
			 
					
		}
		if(Key_Location==9)
		{
			POINT_COLOR=BLUE;		//设置字体为蓝色
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
		}else delay_ms(10); 
	}	 



////////////////////////////////////////////////////////////////////////////////////////////////////////

	LCD_Clear(WHITE);	//清屏
			    
 	POINT_COLOR=RED;    //设置字体为红色  
    
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(60,200,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(60,230,239,246,WHITE);
		delay_ms(200);
	}

 	LCD_ShowString(60,200,200,16,16,"OV7670 Init OK");
	delay_ms(1500);	 	   
	TIM6_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断		 									  
	EXTI8_Init();
	OV7670_Window_Set(10,174,240,320);	//设置窗口	  
  	OV7670_CS=0;
							 	 
 	while(Loop2)//show
	{		
		camera_refresh(&m,&n,&Four_x,&Four_y);  //更新显示	  上-x下+x 左-y右+y	
		
		if(Quadrant1>=590000||Quadrant2>=590000||Quadrant3>=590000||Quadrant4>=590000) 
		{
			Overflow=1;
			LCD_Clear(RED);
		}  			
		while(Overflow)   
		{
				POINT_COLOR=RED;// 设置字体为红色
	    		LCD_ShowString(75,120,200,16,16,"overflow!!");
		    	LCD_ShowString(50,140,200,16,16,"Please small laser");
				LCD_ShowString(50,160,200,16,16,"intensity to avoid");
				LCD_ShowString(50,180,200,16,16,"damaging CMOS");
				
				camera_refresh(&m,&n,&Four_x,&Four_y);//更新显示
				
				if(Quadrant1<590000&&Quadrant2<590000&&Quadrant3<590000&&Quadrant4&&590000) 
				{
					Overflow=0;
//					k=0;
				}

		}

		LCD_DrawLine(180,20,210,20) ;
		LCD_ShowString(220,20,200,16,16,"y+");
		LCD_DrawLine(180,20,180,50) ;
		LCD_ShowString(180,60,200,16,16,"x+");

		POINT_COLOR=BLUE;//设置字体为蓝色
		LCD_ShowString(38,100,200,16,16,"Spot center in CMOS");
	    LCD_ShowString(30,120,200,16,16,"emulsion coordinates :");
		POINT_COLOR=RED; //设置字体为红色	 
	 	LCD_ShowNum(115,160,(x0),4,16);	

		if(n==1&&x0>=1)  //用来显示负号，没什么别的作用~~~~~~ 
		LCD_ShowString(90,160,50,16,16,"X: -");
		else 
		LCD_ShowString(90,160,50,16,16,"X:  ");
		LCD_ShowNum(115,180,(y0),4,16);	
			
		if(m==1&&y0>=1) 
		LCD_ShowString(90,180,55,16,16,"Y: -");
		else 
		LCD_ShowString(90,180,50,16,16,"Y:  ");

		LCD_ShowNum(115,200,(Quadrant1),6,16);
		LCD_ShowNum(115,220,(Quadrant3),6,16);
		LCD_ShowNum(115,260,(Quadrant2),6,16);
		LCD_ShowNum(115,240,(Quadrant4),6,16);
		LCD_ShowNum(50,280,(x0),8,16);
		LCD_ShowNum(115,280,(y0),8,16);
		LCD_ShowNum(115,300,(Quadrant1+Quadrant2+Quadrant3+Quadrant4),7,16);
					
		id++;
 		if(id==5)//DS0闪烁	
		{
			id=0;
			LED0=!LED0;
 		}

	}
	while(Loop3)   //auto
	{		
		camera_refresh(&m,&n,&Four_x,&Four_y);//更新显示	//上-x下+x 左-y右+y	
		
		if(Quadrant1>=590000||Quadrant2>=590000||Quadrant3>=590000||Quadrant4>=590000) 
		{
			Overflow=1;
			LCD_Clear(RED);
		}  			
		while(Overflow)
		{
				
				POINT_COLOR=RED;// 设置字体为红色
	    		LCD_ShowString(75,120,200,16,16,"overflow!!");
		    	LCD_ShowString(50,140,200,16,16,"Please small laser");
				LCD_ShowString(50,160,200,16,16,"intensity to avoid");
				LCD_ShowString(50,180,200,16,16,"damaging CMOS");
				
				camera_refresh(&m,&n,&Four_x,&Four_y);//更新显示
				
				if(Quadrant1<590000&&Quadrant2<590000&&Quadrant3<590000&&Quadrant4&&590000) 
				{
					Overflow=0;	//结束循环
//					k=0;
				}

		}

		LCD_DrawLine(180,20,210,20) ;
		LCD_ShowString(220,20,200,16,16,"y+");
		LCD_DrawLine(180,20,180,50) ;
		LCD_ShowString(180,60,200,16,16,"x+");

//////////////////////////////////////////////////        
		camera_2_refresh(&m,&n,&Four_x,&Four_y);
//////////////////////////////////////////////////



		POINT_COLOR=RED;// 设置字体为红色	 
	 	LCD_ShowNum(115,160,(x0),4,16);	

		if(n==1&&x0>=1)
		LCD_ShowString(90,160,50,16,16,"X: -");
		else 
		LCD_ShowString(90,160,50,16,16,"X:  ");


		if(n==1&&x0>=10)
		MOTOR_Control(1,1,3);	     //MOTOR_Control(0,1); +   x+  下 if(n==1&&x0>=10)MOTOR_Control(0,1,3);

		else if(n==1&&x0>=2)
		MOTOR_Control(1,1,1);

		else if(x0>=10)
		MOTOR_Control(0,1,3);	 //MOTOR_Control(1,1); -   x-  上

		else if(x0>=2)
		MOTOR_Control(0,1,1);	 //MOTOR_Control(1,1); -   x-  上
		LCD_ShowNum(115,180,(y0),4,16);	
			
		if(m==1&&y0>=1)
		LCD_ShowString(90,180,55,16,16,"Y: -");
		else 
		LCD_ShowString(90,180,50,16,16,"Y:  ");


		if(m==1&&y0>=10)
		MOTOR_Control(0,Motor1,3);	   //MOTOR_Control(1,0); -   y+  右 
		else if(m==1&&y0>=2)
		MOTOR_Control(0,Motor1,1); 
		else if(y0>=10)
		MOTOR_Control(1,Motor1,3);      //MOTOR_Control(0,0); +   y-  左
		else if(y0>=2)
		MOTOR_Control(1,Motor1,1);
		

		LCD_ShowNum(115,200,(Quadrant1),6,16);
		LCD_ShowNum(115,220,(Quadrant2),6,16);
		LCD_ShowNum(115,240,(Quadrant3),6,16);
		LCD_ShowNum(115,260,(Quadrant4),6,16);
		LCD_ShowNum(50,280,(x0),8,16);
		LCD_ShowNum(115,280,(y0),8,16);
		LCD_ShowNum(115,300,(Quadrant1+Quadrant2+Quadrant3+Quadrant4),7,16);

/*		
//用于单靶校准
		if(x0 <= 2 && y0 <= 2 && (sum1>0||sum2>0||sum3>0||sum4>0))
		{
			while(1);
		}
*/
        delay_ms(1000);					
		id++;
 		if(id==5)//DS0闪烁	
		{
			id=0;
			LED0=!LED0;
 		}

	}	   
}

