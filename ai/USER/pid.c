  #include <stdio.h>
  #include "pid.h"
  #include <string.h>

extern void PIDInit(pid * PID, int setpoint);
extern int PIDCale(pid *PID, int NextPoint);
extern int GetNowPoint( pid PID );
//extern int  MoveByPid(pid * PID);

void PIDInit(pid *PID , int setpoint)
{
    memset(PID,0, sizeof(pid));
    PID->Proportion = 8;
    PID->Integral = 6;
    PID->Derivative = 0;
    PID->SetPoint = setpoint;
    PID->NowPoint = 0;
    PID->ChangePoint = 0;
}


int PIDCale(pid *PID, int NextPoint)
{
    int dERROR, ERROR;
    ERROR = PID->SetPoint - NextPoint;  //e(k)
    PID->SumError += ERROR;             //e(i) i from 0 to k
    dERROR = ERROR - PID->LastError;    //e(k-1)
    PID->PreError = PID->LastError;     //e(k-2)
    PID->LastError = ERROR;
    PID->ChangePoint = PID->Proportion * ERROR; // + PID->Integral * PID->SumError;// + PID->Derivative * dERROR;
    //if(PID->ChangePoint >= 1 || PID->ChangePoint <= -100)
    //{
        PID->ChangePoint = (PID->ChangePoint / 10);
    //}
	PID->NowPoint += PID->ChangePoint;
    return PID->ChangePoint;

}


int GetNowPoint( pid PID )
{
    return PID.NowPoint;
}

	/*基于PID的调节方法*/
/*
int  MoveByPid(pid * PID)
{



		if(Camera2_X_Num == 1)
		{
			int a=0, step = 0, tmp = 0;
			tmp = PIDCale(&ShiftMotor1, (Camera2_X) );
			LCD_ShowString(90,50,50,16,16,"pid a=");
			LCD_ShowNum(115,50, a ,4,16);
			LCD_ShowString(90,30,50,16,16,"pid step=");
			LCD_ShowNum(115,30,step,4,16);
			if(tmp < 0)
			{
				a = 1; //移动的正反方向
				step = (-1)*tmp;
			}
			else 
				step = tmp;
  			MOTOR_Control(a, 3, step);
		}
		return 0;
}
*/

