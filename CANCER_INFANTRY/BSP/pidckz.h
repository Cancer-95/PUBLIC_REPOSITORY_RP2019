#ifndef __PIDCKZ_H
#define __PIDCKZ_H	 
#include "system.h"

typedef struct {
	float Set;//设定值
	float Get;//实际值
	
	float err;//偏差
	float err_last;//上次偏差
	
	float p,i,d;//比例、积分、微分
	
	float Out;//PID输出
	float Integral;//积分值
	float MaxOut;//PID最大输出
	float MaxIntegral;//最大积分值限幅
	
	float PTERM;
	float ITERM;
	float DTERM;

	}PIDCKZ_t;



void abs_limit(float *a, float ABS_MAX);
void PIDCKZ_param_clear(PIDCKZ_t *pid);
void PIDCKZ_param_set(PIDCKZ_t *pid, float maxout, float maxintergral, float kp, float ki, float kd);
void PIDCKZ_param_roboremo(PIDCKZ_t *pid, float kp, float ki, float kd);//接收Roboremo调PID
float PIDCKZ_Calc(PIDCKZ_t *pid,float Get,float Set);

/*底盘PID*/
extern PIDCKZ_t pid_chassis_speed[4];
/*云台PID*/
extern PIDCKZ_t pid_yaw_M_angle;
extern PIDCKZ_t pid_pit_M_angle;
extern PIDCKZ_t pid_yaw_M_speed;
extern PIDCKZ_t pid_pit_M_speed;
extern PIDCKZ_t pid_yaw_G_angle;
extern PIDCKZ_t pid_pit_G_angle;
extern PIDCKZ_t pid_yaw_G_speed;
extern PIDCKZ_t pid_pit_G_speed;
/*发射PID*/
extern PIDCKZ_t pid_trigger_speed;
extern PIDCKZ_t pid_trigger_position;
/*底盘角度*/
extern PIDCKZ_t pid_chassis_angle;




#endif

