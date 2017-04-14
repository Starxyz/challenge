#ifndef __CONT_H
#define	__CONT_H

#include "sys.h"

#define PUL1 PAout(4)
#define DIR1 PAout(5)	  //S1上面的电机，控制水平偏转
#define PUL2 PAout(6)
#define DIR2 PAout(7)	 //S2转动
#define IDIR1 PAout(11)	   //已拆 
#define IDIR2 PAout(12)	  //S2平动

#define IPUL1 PBout(13)
#define IPUL2 PBout(14)

#define LED0 PBout(5)
#define LED1 PEout(5)

void CONT_Init(void);

#endif 
