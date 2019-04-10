#ifndef __ASSIST_WHEL_H__
#define __ASSIST_WHEL_H__

#include "system.h"


typedef enum
{
	WHEEL_RELAX           = 0,//轮子泻力	
	WHEEL_FORWARD 		    = 1,//轮子正转
	WHEEL_REVERSE        	= 2,//轮子反转
	WHEEL_FORWARD_SLOW 		= 3,//轮子正转
	WHEEL_REVERSE_SLOW    = 4,//轮子反转
	WHEEL_HOLD            = 5,//轮子刹车
	
} assist_wheel_mode_e;

typedef __packed struct
{
	assist_wheel_mode_e  ctrl_mode;
	
	uint16_t offset_ecd; //电机初始角度偏差
	
	uint16_t cur_ecd;    //电机当前时刻读数
  uint16_t las_ecd;    //电机上一时刻读数
	
	int32_t  round_cnt;
	int32_t  total_ecd;   //总读数
	int32_t  total_angle; //总角度换算值
	
  int32_t  target_angle;
	
	int16_t  wheel_speed_fdb;
  int16_t  wheel_speed_ref;
	int16_t  wheel_angle_fdb;
  int16_t  wheel_angle_ref;
  int16_t  current;
	
} assist_wheel_t;

void assist_wheel_param_init(void);
void assist_wheel_data_handler(void);
void assist_wheel_task(void);


extern assist_wheel_t assist_wheel;
#endif

