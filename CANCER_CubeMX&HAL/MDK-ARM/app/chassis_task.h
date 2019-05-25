#ifndef __CHASSIS_TASK_H
#define __CHASSIS_TASK_H

#include "bsp_sys.h"

#define PI                 3.14159265358979f //用于扭腰
/******************************************************/
/* 标准化的遥控器比例*/
#define RC_RESOLUTION     660.0f


/*************************底盘设置*******************************/
/* 遥控器控制底盘时的速度限制 */
/* remote mode chassis move speed limit */
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

/* 键盘控制底盘时的速度限制 */
/* keyboard mode speed limit */
/* left and right speed (mm/s) */
#define CHASSIS_KB_MAX_SPEED_X  3300.0f 
#define CHASSIS_KB_MOVE_RATIO_X 1.0f
/* back and forward speed (mm/s) */
#define CHASSIS_KB_MAX_SPEED_Y  3300.0f
#define CHASSIS_KB_MOVE_RATIO_Y 1.0f
/* chassis rotation speed (deg/s) */
#define CHASSIS_KB_MAX_SPEED_R 400.0f
#define CHASSIS_KB_MOVE_RATIO_R 1.0f

/************************ 底盘的参数 ****************************/
/* 麦轮半径(mm) */
#define RADIUS     76
/* 麦轮边长(mm) */
#define PERIMETER  478

/* 左右轮距(mm) */
#define WHEELTRACK 643
/* 前后轴距(mm) */
#define WHEELBASE  564

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

typedef enum
{
	CHASSIS_SEPARATE        = 0, //底盘机械模式
	CHASSIS_FOLLOW          = 1, //底盘陀螺仪模式
  CHASSIS_RELAX           = 2,
  CHASSIS_STOP            = 3,
  MOVE_FORWARD            = 4,

} chassis_mode_e;


typedef struct
{
	bool            turn_around_disable;//掉头是否失能 正常情况下是开着的
	bool            ass_mode_enable;//车尾模式使能位
	bool            force_slow_enable;//车速强制降低(用于取弹的时候)
	
  float           vx; // forward/back
  float           vy; // left/right
  float           vw; // 
	

	chassis_mode_e  ctrl_mode;
	
  int16_t         wheel_motor_angle[4];
	int16_t         wheel_spd_fdb[4];
  int16_t         wheel_spd_ref[4];
  int16_t         current[4];
  
	float           position_ref;
	float           position_temp;//记录的暂时的位置 用于掉头

	  /* position loop */
  float yaw_angle_ref;
  float yaw_angle_fdb;

  /* speed loop */
  float yaw_speed_ref;
  float yaw_speed_fdb;

	/*relative_angle*/
  float yaw_relative_angle;
	

} chassis_t;

extern chassis_t chassis;

void chassis_param_init(void);
void chassis_task(void);

static void mecanum_calc(float vx, float vy, float vw, int16_t Wheel_speed[]);

static void chassis_stop_handler(void);
static void chassis_forward_handler(void);
static void chassis_separate_handler(void);
static void chassis_follow_handler(void);

static bool chassis_is_controllable(void);

void chassis_yaw_target(void);
void chassis_outter_loop(void);
void chassis_inner_loop(void);
void chassis_yaw_handler(void);
#endif
