#ifndef __CLIMB_TUBE_H
#define __CLIMB_TUBE_H

#include "system.h"

#define LAUNCH_TRIGGER    ((KEY_F_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+F 自动程序触发

typedef enum
{	
	PLAIN_MODE             = 0,//平原模式(这个放在枚举第一位 使上电时的初始状态是平原模式)
	CLIMEB_DOWN_OPTIC_MODE = 1,//下杆模式 (光电方法)
	CLIMEB_UP_OPTIC_MODE   = 2,//爬杆模式 (光电方法)
	CLIMEB_DOWN_GYRO_MODE  = 3,//下杆模式 (陀螺仪方法)
	CLIMEB_UP_GYRO_MODE    = 4,//爬杆模式 (陀螺仪方法)

}climb_tube_mode_e;

typedef enum
{	
	ON_GROUND          = 0,//在平地
	ON_ISLAND          = 1,//在岛上
	IN_DANGER          = 2,//危险

}climb_tube_location_e;

typedef struct
{
	climb_tube_mode_e  ctrl_mode;
	
	climb_tube_location_e location;              
	
	bool climb_enable;
	bool climb_up_optic_enable;
	bool climb_down_optic_enable;
	bool climb_up_gyro_enable;
	bool climb_down_gyro_enable;
	
  bool fiag_launch;             //发射标志位 可以理解为开始转了
	
	bool yaw_record_enable;       //要不要标定角度
	bool twist_calc_enable;       //要不要做差值计算
	
  float yaw_record;             //记录的标定角度
	float twisted_angle;          //转过的角度
	
} climb_tube_t;

extern climb_tube_t climb_tube;

void climb_tube_param_init(void);
void climb_tube_task(void);

#endif

