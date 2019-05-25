#include "chassis_task.h"

/* chassis task global parameter */
chassis_t chassis;

void chassis_param_init()
{
	memset(&chassis, 0, sizeof(chassis_t));
	
	pidckz_reset(&pid_chassis_speed);
	pidckz_reset(&pid_chassis_angle);
	pidckz_param_set(&pid_chassis_speed, 400, 300, 0.2f, 0.0f, 0.0f);   //陀螺仪模式YAW内环参数设定
	pidckz_param_set(&pid_chassis_angle, 4000, 50, 130.0f, 0.0f, 0.0f);	//陀螺仪模式YAW外环参数设定
	
	for(uint8_t k = 0; k < 4; k++)
	{
	  pidckz_reset(&pid_wheel_speed[k]);		
  	pidckz_param_set(&pid_wheel_speed[k], 10000, 1000, 9.0f, 0.4f, 16.0f);	//for底盘常规
	}
}

void chassis_task()
{
	get_chassis_info();
	
	switch (chassis.ctrl_mode)
	{
		case CHASSIS_STOP://速度零锁住
		{	
      chassis_stop_handler();
		}break;
			
		case MOVE_FORWARD://向前走
		{	
      chassis_forward_handler();
		}break;
		
		case CHASSIS_SEPARATE://机械模式
		{	
      chassis_separate_handler();
		}break;
						
		case CHASSIS_FOLLOW://陀螺仪模式
    {
      chassis_follow_handler();
    }break;	
		
		default://速度零锁住																																								
		{
      chassis_stop_handler();
		}break;
	}
	mecanum_calc(chassis.vx, chassis.vy, chassis.vw, chassis.wheel_spd_ref);    //将底盘三个速度值分解得到四个轮子的目标值
		
  for(uint8_t k = 0; k < 4; k++)
	chassis.current[k] = pidckz_calc(&pid_wheel_speed[k], chassis.wheel_spd_fdb[k], chassis.wheel_spd_ref[k]); //PID计算
	
//		if (system_mode == SAFETY_MODE)
//	  {
//      memset(chassis.current, 0, sizeof(chassis.current));
//  	}	
}

/**
  * @brief mecanum chassis velocity decomposition
  * @param input : ?=+vx(mm/s)  ?=+vy(mm/s)  ccw=+vw(deg/s)
  *        output: every wheel speed(rpm)
  * @note  1=FR 2=FL 3=BL 4=BR
  */
int rotation_center_gimbal = 0;//云台是否在底盘中心
void mecanum_calc(float vx, float vy, float vw, int16_t Wheel_speed[])//麦轮公式计算
{
  static float rotate_ratio_fr;
  static float rotate_ratio_fl;
  static float rotate_ratio_bl;
  static float rotate_ratio_br;
  static float wheel_rpm_ratio;
	int16_t wheel_rpm[4];
  float   max = 0;
  /* 云台不在正中心则需要进行补偿*/
  if (rotation_center_gimbal)
  { 
		rotate_ratio_fr = ((WHEELBASE + WHEELTRACK) / 2.0f - GIMBAL_X_OFFSET + GIMBAL_Y_OFFSET)/RADIAN_COEF;
    rotate_ratio_fl = ((WHEELBASE + WHEELTRACK) / 2.0f - GIMBAL_X_OFFSET - GIMBAL_Y_OFFSET)/RADIAN_COEF;
    rotate_ratio_bl = ((WHEELBASE + WHEELTRACK) / 2.0f + GIMBAL_X_OFFSET - GIMBAL_Y_OFFSET)/RADIAN_COEF;
    rotate_ratio_br = ((WHEELBASE + WHEELTRACK) / 2.0f + GIMBAL_X_OFFSET + GIMBAL_Y_OFFSET)/RADIAN_COEF;
  }
  else
  {
    rotate_ratio_fr = ((WHEELBASE + WHEELTRACK) / 2.0f) / RADIAN_COEF;
    rotate_ratio_fl = rotate_ratio_fr;
    rotate_ratio_bl = rotate_ratio_fr;
    rotate_ratio_br = rotate_ratio_fr;
  }
  wheel_rpm_ratio = 60.0f/(PERIMETER*CHASSIS_DECELE_RATIO);		 	//	60/周长*减速比
  
	VAL_LIMIT(vx, -MAX_CHASSIS_VX_SPEED, MAX_CHASSIS_VX_SPEED);   //mm/s
  VAL_LIMIT(vy, -MAX_CHASSIS_VY_SPEED, MAX_CHASSIS_VY_SPEED);  	//mm/s
  VAL_LIMIT(vw, -MAX_CHASSIS_VR_SPEED, MAX_CHASSIS_VR_SPEED);  	//deg/s
  
	/*四个电机转速*/   //默认排序从左上角开始顺时针0123 可根据实际情况改
  wheel_rpm[0] = (-vx - vy + vw * rotate_ratio_fr) * wheel_rpm_ratio;
  wheel_rpm[1] = ( vx - vy + vw * rotate_ratio_fl) * wheel_rpm_ratio;
  wheel_rpm[2] = ( vx + vy + vw * rotate_ratio_bl) * wheel_rpm_ratio;
  wheel_rpm[3] = (-vx + vy + vw * rotate_ratio_br) * wheel_rpm_ratio;

  /*找到四个电机*/
  for (uint8_t i = 0; i < 4; i++)
  {
    if (abs(wheel_rpm[i]) > max)
      max = abs(wheel_rpm[i]);
  }
  /* equal proportion */
  if (max > MAX_WHEEL_RPM)
  {
    float rate = MAX_WHEEL_RPM / max;
    for (uint8_t i = 0; i < 4; i++)
      wheel_rpm[i] *= rate;
  }
  memcpy(Wheel_speed, wheel_rpm, 4*sizeof(int16_t));
}

void chassis_stop_handler(void)//底盘速度环锁住
{
  chassis.vy = 0;
  chassis.vx = 0;
  chassis.vw = 0;
	
	chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_forward_handler(void)//底盘向前跑
{
  chassis.vy = 0;
  chassis.vx = 200;
  chassis.vw = 0;
	
  chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_separate_handler(void)//机械模式
{
	chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
	
  chassis.vy = rm.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;
  chassis.vx = rm.vx * CHASSIS_RC_MOVE_RATIO_X + kb.vx * CHASSIS_KB_MOVE_RATIO_X;
	
  if(chassis.ass_mode_enable)//车尾模式
	{
    chassis.vw = -(rm.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R);
	}
	else//车头模式
	{
    chassis.vw = rm.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R;
	}
}

void chassis_follow_handler(void)//陀螺仪模式
{  
  chassis.vy = rm.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;
  chassis.vx = rm.vx * CHASSIS_RC_MOVE_RATIO_X + kb.vx * CHASSIS_KB_MOVE_RATIO_X;
  chassis_yaw_handler();//YAW轴处理得出vw值
}

void chassis_yaw_target(void)//获取Y轴的目标值
{	 
//	if(chassis.ass_mode_enable)//车尾模式
//	{
//		if(rc.rc_enable) chassis.position_ref += (RC_Ctl.rc.ch0-RC_CH_VALUE_OFFSET)*0.0005;//遥控器vw	
//		else if(kb.kb_enable) chassis.position_ref += RC_Ctl.mouse.x * 0.01;//键盘vw
//	}
//	else//车头模式
//	{
//		if(rc.rc_enable) chassis.position_ref += -(RC_Ctl.rc.ch0-RC_CH_VALUE_OFFSET)*0.0005;//遥控器vw	
//		else if(kb.kb_enable) chassis.position_ref += -RC_Ctl.mouse.x * 0.01;//键盘vw
//	}	
	
	if(rc.rc_enable) 
	{
		chassis.position_ref += -rc.ch1*0.0005;//遥控器vw			
	}
	else if(kb.kb_enable) 
	{
		chassis.position_ref += -rc.mouse.x * 0.01;//键盘vw
	}
	/*YAW轴目标值确保越过临界值*/	
	if(chassis.position_ref >= 180) 
	{
		chassis.position_ref = chassis.position_ref  - 360;
	}
	else if(chassis.position_ref <= -180)
	{
		chassis.position_ref = 360 + chassis.position_ref;
	}		
}

void chassis_outter_loop(void)//陀螺仪模式外环
{
	//yaw串级
  chassis.yaw_angle_ref = chassis.position_ref;                                    //外环(角度值)目标值
//	chassis.yaw_angle_fdb = imu.yaw;                                                 //获取陀螺仪角速度作为当前角度
	critical_value_treatment(chassis.yaw_angle_fdb, &chassis.yaw_angle_ref,0,360);   //临界值处理
	pidckz_calc(&pid_chassis_angle, chassis.yaw_angle_fdb, chassis.yaw_angle_ref);   //PID计算的得到外环输出(以角速度形式输出)	
}

void chassis_inner_loop(void)//陀螺仪模式内环
{
	//yaw串级
	chassis.yaw_speed_ref = pid_chassis_angle.out;                                 //外环输出作为目标值
//	chassis.yaw_speed_fdb = imu.gz;                                                //读取陀螺仪的角速度值
	pidckz_calc(&pid_chassis_speed, chassis.yaw_speed_ref, chassis.yaw_speed_fdb); //PID计算得到最终输出(注意谁减谁)
	chassis.vw = pid_chassis_speed.out;
}

void chassis_yaw_handler(void)
{
	chassis_yaw_target();
	chassis_outter_loop();
	chassis_inner_loop();
}

