#ifndef __TRAILER_H__
#define __TRAILER_H__

#include "system.h"

typedef enum
{	
	TRAILER_PULL    = 0,      //拖车收回
	TRAILER_PUSH    = 1,      //拖车推出
	TRAILER_RELAX   = 2,      //relax
	
} trailer_mode_e;

typedef __packed struct
{

	bool              reset;
	int16_t           reset_speed;       //复位速度
	int16_t           target_speed;      //temp

	bool              across_stall_protect;
	bool              stall_protect_record; 
  uint32_t          stall_protect_time;   //
	
	bool              stall_time_record; //堵转时间标记
	uint32_t          stall_time;        //堵转时间
	uint16_t          stall_times;       //堵转次数
//	bool              serious_stall;     //判定是否严重堵转
	
	trailer_mode_e  ctrl_mode;
	
	uint16_t offset_ecd;  //电机初始角度偏差
	
	uint16_t cur_ecd;     //电机当前时刻读数
  uint16_t las_ecd;     //电机上一时刻读数
  bool     ecd_record;  //单次记录误差标志位
	
	int32_t  round_cnt;
	int32_t  total_ecd;   //总读数
	int32_t  total_angle; //总角度换算值
	
  int32_t  target_angle;
	int32_t  target_entire;
  int32_t  target_zero;
	
	int16_t  speed_fdb;
  int16_t  speed_ref;
	int16_t  angle_fdb;
  int16_t  angle_ref;
  int16_t  current;
	
} trailer_t;

void trailer_param_init(void);
void trailer_data_handler(void);
void trailer_task(void);


extern trailer_t trailer;
#endif

