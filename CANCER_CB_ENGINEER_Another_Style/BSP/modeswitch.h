#ifndef __MODESWITH_H
#define __MODESWITH_H
#include "system.h"




typedef enum
{
	SAFETY_MODE       = 0,//安全模式
	RC_MACHINE_MODE   = 1,//遥控机械模式
	RC_GYRO_MODE      = 2,//遥控陀螺仪模式
	KB_MODE           = 3,//键盘模式(底盘只有陀螺仪模式)

} SYSTEM_MODE;//系统模式






extern SYSTEM_MODE system_mode;


void mode_switch_task(void);


#endif 

