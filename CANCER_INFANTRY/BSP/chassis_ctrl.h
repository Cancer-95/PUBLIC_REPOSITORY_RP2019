#ifndef __CHASSISCTRL_H
#define __CHASSISCTRL_H

#include "system.h"

#define PI                 3.14159265358979f //用于扭腰
/**********************?????***************************/
/* 标准化的遥控器比例 */
#define RC_RESOLUTION     660.0f


/*************************底盘设置*******************************/
/* 遥控器控制底盘时的速度限制 */
/* left and right speed (mm/s) */
#define CHASSIS_RC_MAX_SPEED_X  3500.0f
#define CHASSIS_RC_MOVE_RATIO_X 1.0f
/* back and forward speed (mm/s) */
#define CHASSIS_RC_MAX_SPEED_Y  3500.0f
#define CHASSIS_RC_MOVE_RATIO_Y 1.0f
/* chassis rotation speed (deg/s) */
/* used only chassis open loop mode */
#define CHASSIS_RC_MAX_SPEED_R 400.0f
#define CHASSIS_RC_MOVE_RATIO_R 1.0f

/* keyboard mode speed limit */
/* left and right speed (mm/s) */
#define CHASSIS_KB_MAX_SPEED_X  3500.0f 
#define CHASSIS_KB_MOVE_RATIO_X 1.0f
/* back and forward speed (mm/s) */
#define CHASSIS_KB_MAX_SPEED_Y  3500.0f
#define CHASSIS_KB_MOVE_RATIO_Y 1.0f

/************************ 底盘的参数 ****************************/
/* 麦轮半径(mm) */
#define RADIUS     76
/* 麦轮周长(mm) */
#define PERIMETER  478

/* 左右轮距(mm) */
#define WHEELTRACK 360
/* 前后轴距(mm) */
#define WHEELBASE  300

/* 云台相对底盘中心的X偏移(mm) */
#define GIMBAL_X_OFFSET 0
/* 云台相对底盘中心的Y偏移(mm) */
#define GIMBAL_Y_OFFSET 0

/* 电机减速比 */
#define CHASSIS_DECELE_RATIO (1.0f/19.0f)
/* 单个电机最大转速(rpm) */
#define MAX_WHEEL_RPM        8500  //8347rpm = 3500mm/s
/* 底盘最大速度, unit is mm/s */
#define MAX_CHASSIS_VX_SPEED 3500  //8000rpm
#define MAX_CHASSIS_VY_SPEED 3500
/* 底盘最大旋转速度, unit is degree/s */
#define MAX_CHASSIS_VR_SPEED 400   //5000rpm


/************************** 云台的参数 *****************************/
/* radian coefficient */
#define RADIAN_COEF 57.3f
/*电机返回值转换成角度的比值*/
#define ENCODER_ANGLE_RATIO    (8192.0f/360.0f)




#define VAL_LIMIT(val, min, max) \
do {\
if((val) <= (min))\
{\
  (val) = (min);\
}\
else if((val) >= (max))\
{\
  (val) = (max);\
}\
} while(0)\



typedef struct
{
  float           vx; // forward/back
  float           vy; // left/right
  float           vw; // 
	
  float           dx; // 扭腰程序里未坐标变换的速度
  float           dy; // 扭腰程序里未坐标变换的速度
	
  float           total_current; //底盘总电流
  float           total_current_limit; // 底盘总限制电流
	
//  float           gyro_angle;
//  float           gyro_palstance;

  int16_t         wheel_motor_angle[4];
	int16_t         wheel_speed_fdb[4];
  int16_t         wheel_speed_ref[4];
  int16_t         current[4];
  
  int16_t         position_ref;//底盘方位(用于扭腰)
//	float         	X_ref;
//	float         	Y_ref;
} chassis_t;

extern chassis_t chassis;

void Chassis_Init(void);
void Chassis_Control(void);
void mecanum_calc(float vx, float vy, float vw, int16_t Wheel_speed[]);

void Chassis_WSAD_Keyboard_Ctrl(float Acc_For_AD, float Acc_For_WS, float Acc_Brake, u16 Chassis_Speed);
void Chassis_twist_handle(void);
void Chassis_power_handle(float Total_Limit, float Power_Threshold);





#endif
