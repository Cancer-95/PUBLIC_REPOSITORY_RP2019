#ifndef __CAN1_H
#define __CAN1_H

#include "system.h"

#define M1						0
#define M2  					1
#define M3 						2
#define M4						3

#define ANGLE			    0
#define SPEED			    1

#define S			        0
#define R			        0
#define L			        1
#define M			        2
#define X			        2
#define Y			        3

void CAN1_init(void);
void CAN1_send(uint32_t Equipment_ID,int16_t Data0,int16_t Data1,int16_t Data2,int16_t Data3);
bool CAN1_motor_offline_check(void);

extern int16_t Motor_chassis[4][2];

#endif

