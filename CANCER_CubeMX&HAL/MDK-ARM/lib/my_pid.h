#ifndef __MY_PID_H
#define __MY_PID_H	 
#include "bsp_sys.h"

typedef struct {
	float set;//设定值
	float get;//实际值
	
	float err;//偏差
	float err_last;//上次偏差
	
	float p,i,d;//比例、积分、微分
	
	float out;//输出
	float integral;//积分值
	float maxout;//最大输出限幅
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

/*陀螺仪温度*/
extern pidckz_t pid_imu_tmp;
/*底盘PID*/
extern pidckz_t pid_wheel_speed[4];
/*底盘角度*/
extern pidckz_t pid_chassis_angle;
/*底盘速度*/
extern pidckz_t pid_chassis_speed;   
	
	
	
///*云台PID*/
//extern pidckz_t pid_yaw_G_angle;
//extern pidckz_t pid_yaw_G_speed;

/*辅助摩擦轮PID*/
extern pidckz_t pid_fric_speed;
extern pidckz_t pid_fric_position;

/*拖车电机PID*/
extern pidckz_t pid_trailer_speed;
extern pidckz_t pid_trailer_position;

/*抬升电机PID*/
extern pidckz_t pid_lifter_speed[2];
extern pidckz_t pid_lifter_position[2];
 
/*取弹处几个电机的PID*/
extern pidckz_t pid_bomb_claw_speed[4];
extern pidckz_t pid_bomb_claw_position[4];


#endif

