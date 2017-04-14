  #include <stdio.h>
  #include "pid.h"
  #include <string.h>
extern void PIDInit(pid * PID);
extern int PIDCale(pid *PID , int setpoint);//, int NextPoint);
extern int GetNowPoint( pid PID );
//extern int  MoveByPid(pid * PID);

void PIDInit(pid *PID)
{
    memset(PID,0, sizeof(pid));		//初始化为0
    PID->Proportion = 8;
    PID->Integral = 6;
    PID->Derivative = 0;
    PID->ChangePoint = 0;
}


int PIDCale(pid *PID , int Setpoint)//, int NowPoint)
{
	int Increment;
	PID->SetPoint = Setpoint;
	PID->Error = PID->SetPoint - PID->NowPoint;
	Increment = PID->Proportion * (PID->Error - PID->LastError) + \
				PID->Integral * (PID->Error) + \
				PID->Derivative * (PID->Error - 2*PID->LastError + PID->LLastError);
	PID->ChangePoint += Increment;				   
	PID->LLastError = PID->LastError;
	PID->LastError = PID->Error;
	return PID->ChangePoint;



}


	/*基于PID的调节方法*/
/*
int  MoveByPid(pid * PID)
{



		if(Camera2_X_Num == 1)
		{
			int dir=0, step = 0, tmp = 0;
			tmp = PIDCale(&ShiftMotor1, y_point );
			LCD_ShowString(90,50,50,16,16,"pid a=");
			LCD_ShowNum(115,50, dir ,4,16);
			LCD_ShowString(90,30,50,16,16,"pid step=");
			LCD_ShowNum(115,30,step,4,16);
			if(tmp < 0)
			{
				dir = 1; //移动的正反方向
				step = (-1)*tmp;
			}
			else 
				step = tmp;
  			MOTOR_Control(dir, Motor4, step); //竖直方向的滑轨
		}
		return 0;
}
*/

