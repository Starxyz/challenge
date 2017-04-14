#ifndef __PID_H
#define __PID_H

typedef struct pidClass
{
  int SetPoint;  
  int NowPoint;
  int Proportion,Integral,Derivative;
  int LastError,LLastError;
  int Error;

  int SumError;

  int ChangePoint;
}pid;

void PIDInit(pid *PID);
int PIDCale(pid *PID ,int setpoint);//, int NextPoint);
int  GetNowPoint( pid PID );
//int  MoveByPid(pid * PID);

#endif

