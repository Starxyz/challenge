#ifndef __PID_H
#define __PID_H

typedef struct pidClass
{
  int SetPoint;
  int Proportion;
  int Integral;
  int Derivative;
  int LastError;
  int PreError;
  int SumError;
  int NowPoint;
  int ChangePoint;
}pid;

void PIDInit(pid * PID, int setpoint);
int  PIDCale(pid * PID, int NextPoint);
int  GetNowPoint( pid PID );
//int  MoveByPid(pid * PID);

#endif

