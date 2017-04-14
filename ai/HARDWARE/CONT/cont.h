#ifndef __CONT_H
#define	__CONT_H

#include "sys.h"

#define PUL1 PAout(4)
#define DIR1 PAout(5)
#define PUL2 PAout(6)
#define DIR2 PAout(7)

#define IDIR1 PAout(11)
#define IDIR2 PAout(12)
#define IPUL1 PBout(13)
#define IPUL2 PBout(14)

#define LED0 PBout(5)
#define LED1 PEout(5)

void CONT_Init(void);

#endif 
