#ifndef __GIMBALCTRL_H
#define __GIMBALCTRL_H

#include "system.h"

#define Pitch_M_MIN					3300
#define Pitch_M_MID					3800
#define Pitch_M_MAX				  4300

#define Yaw_M_MIN						2200
#define Yaw_M_MID 					3750
#define Yaw_M_MAX						5400

#define Pitch_G_MIN					-20         //角度
#define Pitch_G_MID					0
#define Pitch_G_MAX				  25

#define Yaw_G_MIN						-45         //角度
#define Yaw_G_MID 					0
#define Yaw_G_MAX						45

#define MEC					     	0
#define GYO 					    1

typedef struct
{
  /* position loop */
  float yaw_angle_ref;
  float pit_angle_ref;
  float yaw_angle_fdb;
  float pit_angle_fdb;
  /* speed loop */
  float yaw_speed_ref;
  float pit_speed_ref;
  float yaw_speed_fdb;
  float pit_speed_fdb;
	/*relative_angle*/
	float pit_relative_angle;
  float yaw_relative_angle;
	
	
} gimbal_t;

extern gimbal_t gim;
extern float  Pitch_Target[2], Yaw_Target[2];//主要是提供给外部做云台强制控制如补弹时云台水平


void Gimbal_Init(void);
void Get_MEC_Target(void);
void Get_GYO_Target(void);
void Gimbal_M_Control_Outter(void);
void Gimbal_M_Control_Inner(void);
void Gimbal_G_Control_Outter(void);
void Gimbal_G_Control_Inner(void);
void Gimbal_M_Control(void);
void Gimbal_G_Control(void);

void Gimbal_Output_Stop(void);
void Gimbal_Turn_Around(void);//键盘按C掉头

void  Gimbal_Control(void);



#endif

