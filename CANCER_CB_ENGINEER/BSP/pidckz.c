#include "pidckz.h"

/*陀螺仪温度*/
pidckz_t pid_imu_tmp            = {0};   //陀螺仪温度
/*底盘PID*/
pidckz_t pid_wheel_speed[4]     = {0};   // 轮子速度
/*底盘角度*/
pidckz_t pid_chassis_angle      = {0};   //yaw轴pid外环
/*底盘速度*/
pidckz_t pid_chassis_speed      = {0};   //yaw轴pid内环
/*视觉对位*/
pidckz_t pid_vision_chassis_vy  = {0};   //视觉下底盘vy

///*云台PID*/
//pidckz_t pid_yaw_G_angle         = {0};
//pidckz_t pid_yaw_G_speed         = {0};

/*辅助摩擦轮PID*/
pidckz_t pid_fric_speed            = {0};
pidckz_t pid_fric_position         = {0};

/*辅助摩擦轮PID*/
pidckz_t pid_trailer_speed         = {0};
pidckz_t pid_trailer_position      = {0};

/*抬升PID*/
pidckz_t pid_lifter_speed[2]       = {0};
pidckz_t pid_lifter_position[2]    = {0};

/*取弹处几个电机的PID*/
pidckz_t pid_bomb_claw_speed[4]    = {0};
pidckz_t pid_bomb_claw_position[4] = {0};

void abs_limit(float *a, float ABS_MAX)
{
  if (*a > ABS_MAX)
      *a = ABS_MAX;
  if (*a < -ABS_MAX)
      *a = -ABS_MAX;
}

void pidckz_reset(pidckz_t *pid)
{
  memset(pid, 0, sizeof(pidckz_t));
}

void pidckz_param_set(pidckz_t *pid, float maxout, float maxintergral, float kp, float ki, float kd)
{
  pid->p = kp;
	pid->i = ki;
	pid->d = kd;
	pid->maxout = maxout;
	pid->maxintegral = maxintergral;
}

void pidckz_param_roboremo(pidckz_t *pid, float kp, float ki, float kd)
{
  pid->p = kp;
	pid->i = ki;
	pid->d = kd;
}

float pidckz_calc(pidckz_t *pid, float get, float set)
{
	pid->set = set;//设置设定值	
	pid->get = get;//读取

	pid->err = pid->set-pid->get;//计算偏差
	 
	pid->integral += pid->i * pid->err;//计算积分值	
	abs_limit(&pid->integral, pid->maxintegral);	 //积分限幅
		
	pid->pout = pid->p * pid->err;
	pid->iout = pid->integral;	
	pid->dout = pid->d * (pid->err - pid->err_last);
	
	pid->out = pid->pout + pid->iout + pid->dout;
	abs_limit(&pid->out, pid->maxout);//PID输出限幅  

	pid->err_last = pid->err;//将当前偏差存做上一次偏差

	return pid->out;
}

void pidckz_integral_handle(pidckz_t *pid, float tolerance_err)
{
	/* 误差大于一定范围的时候不输出积分项 */
	if(fabs(pid->err) >= tolerance_err)
	{
		pid->out = pid->out - pid->iout;
		pid->iout = 0;
	}
}
