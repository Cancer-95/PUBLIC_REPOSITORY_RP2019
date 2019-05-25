#ifndef __CAN2_H__
#define __CAN2_H__

#include "system.h"

#define TRI_ANGLE				  	0
#define TRI_SPEED						1

void CAN2_Init(void);
void CAN2_Send(uint32_t Equipment_ID,int16_t Data);

extern int16_t Trigger_Motor[2];

#endif 
