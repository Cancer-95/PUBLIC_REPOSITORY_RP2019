#ifndef __SHOOT_CTRL_H__
#define __SHOOT_CTRL_H__

#include "system.h"

#define Bomb_Angle 1620  //360*36/8 (360/速比/拨盘格数)


#define ON			1
#define OFF			0

typedef struct
{
	uint16_t offset_ecd; //电机初始角度偏差
	
	uint16_t cur_ecd;    //电机当前时刻读数
  uint16_t las_ecd;    //电机上一时刻读数
	
  int16_t  speed_rpm;  //
	
	int32_t  round_cnt;
	int32_t  total_ecd;   //总读数
	int32_t  total_angle; //总角度换算值
	
} Tri_moto_t;


//typedef struct
//{
//	
//  uint8_t      fric_wheel_run; //run or not
//	
//} Fri_whee_t;




void Shoot_Init(void);

void Trigger_Remote_Control(void);
void Arsenal_Control(void);

void Tri_Data_Handle(void);
void Tri_Cas_Ctrl(u32 Target_Angle);
void Tri_Spe_Ctrl(int16_t speed);

void Block_Bullet_Handle(void);

void Fric_Ctrl(uint16_t pwm_L, uint16_t pwm_R);
void Servo_Ctrl( int16_t pwm );

void Shoot_Remote_Ctrl(void);


#endif
