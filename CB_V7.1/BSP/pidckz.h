#ifndef __pidckz_H
#define __pidckz_H	 
#include "system.h"

typedef struct {
	float set;//设定值
	float get;//实际值
	
	float err;//偏差
	float err_last;//上次偏差
	
	float p,i,d;//比例、积分、微分
	
	float out;//PID输出
	float integral;//积分值
	float maxout;//PID最大输出
	float maxintegral;//最大积分值限幅
	
	float pout;
	float iout;
	float dout;

	}pidckz_t;

void abs_limit(float *a, float ABS_MAX);
	
void pidckz_reset(pidckz_t *pid);
	
void pidckz_param_set(pidckz_t *pid, float maxout, float maxintergral, float kp, float ki, float kd);
void pidckz_param_roboremo(pidckz_t *pid, float kp, float ki, float kd);//接收Roboremo调PID

float pidckz_calc(pidckz_t *pid, float get, float set);

void pidckz_integral_handle(pidckz_t *pid, float tolerance_err);
	
/*陀螺仪温度*/
extern pidckz_t pid_imu_tmp;	
/*底盘PID*/
extern pidckz_t pid_wheel_speed[4];
/*底盘角度*/
extern pidckz_t pid_chassis_angle;
/*底盘速度*/
extern pidckz_t pid_chassis_speed;   
/*视觉下底盘vy*/
extern pidckz_t pid_vision_chassis_vy;   
	
///*云台PID*/
//extern pidckz_t pid_yaw_G_angle;
//extern pidckz_t pid_yaw_G_speed;

/*发射PID*/
extern pidckz_t pid_fric_speed;
extern pidckz_t pid_fric_position;

/*拖车PID*/
extern pidckz_t pid_trailer_speed;
extern pidckz_t pid_trailer_position;

/*抬升PID*/
extern pidckz_t pid_lifter_speed[2];
extern pidckz_t pid_lifter_position[2];
 
/*取弹处几个电机的PID*/
extern pidckz_t pid_bomb_claw_speed[4];
extern pidckz_t pid_bomb_claw_position[4];


#endif

