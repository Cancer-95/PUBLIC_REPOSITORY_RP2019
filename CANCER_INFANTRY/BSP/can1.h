#ifndef __CAN1_H
#define __CAN1_H

#include "system.h"

#define M1						0
#define M2  					1
#define M3 						2
#define M4						3

#define YAW 					0
#define PIT						1

void CAN1_Init(void);
void CAN1_Send(uint32_t Equipment_ID,int16_t Data0,int16_t Data1,int16_t Data2,int16_t Data3);

extern int16_t Motor_chassis[4][2],Motor_angle[2];

#endif

