#ifndef __MODESWITH_H
#define __MODESWITH_H
#include "system.h"




typedef enum
{
	SAFETY_MODE       = 0,//安全模式
	RC_MACHINE_MODE   = 1,//遥控机械模式
	RC_GYRO_MODE      = 2,//遥控陀螺仪模式
  RC_DODGE_MODE     = 5,//遥控扭腰(这个其实没有用到)
	KEY_MACHINE_MODE  = 3,//键盘机械模式
	KEY_GYRO_MODE     = 4,//键盘陀螺仪模式
	KEY_DODGE_MODE    = 5,//键盘扭腰
	

} SYSTEM_MODE;//系统模式


typedef enum
{
	GIMBAL_STO      = 0,//云台不输出
	GIMBAL_MEC      = 1,//云台机械模式
	GIMBAL_GYO      = 2,//云台陀螺仪模式

} GIMBAL_MODE;//云台模式

typedef enum
{
	CHASSIS_STOP 						= 0,//底盘不输出
	MANUAL_FOLLOW_CHASSIS 	= 1,//底盘云台分离
	MANUAL_FOLLOW_GIMBAL 		= 2,//底盘云台跟随
	DODGE_MODE      				= 3,//摇摆
	
} CHASSIS_MODE;//底盘模式

typedef enum
{
	FRIC_WHEEL_OFF 		  		= 0,//摩擦轮停
	FRIC_WHEEL_RUN         	= 1,//摩擦轮开
	
} FRIC_MODE;//摩擦轮模式

typedef enum
{
	SERVO_CLOSE 		  		= 0,//弹仓关
	SERVO_OPEN            = 1,//弹仓开
	
} SERVO_MODE;//弹仓舵机模式


extern CHASSIS_MODE chassis_mode;
extern GIMBAL_MODE gimbal_mode;
extern FRIC_MODE fric_mode;
extern SYSTEM_MODE system_mode;



void Ctrl_Mode_Select(void);


#endif 

