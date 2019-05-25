#include "chassis_ctrl.h"

/* chassis task global parameter */
chassis_t chassis;

void Chassis_Init()
{
	  PIDCKZ_param_set(&pid_chassis_angle, 300, 50, 8.0f, 0.0f, 10.0f);	//for扭腰
	
	  for(u8 k = 0; k < 4; k++)
  	PIDCKZ_param_set(&pid_chassis_speed[k], 10000, 1000, 8.0f, 0.2f, 20.0f);	//for底盘常规
}

void Chassis_Control()
{
	switch (chassis_mode)
	{
		case CHASSIS_STOP://STOP(赋值但在下面不进行PID计算)
		{	
			chassis.vy = 0;chassis.vx = 0;chassis.vw = 0;
			chassis.current[M1] = chassis.current[M2] = chassis.current[M3] = chassis.current[M4] = 0;
		}break;
		
		case MANUAL_FOLLOW_CHASSIS://云台跟随底盘 FOR 机械模式
		{	
			if(system_mode == KEY_MACHINE_MODE)
			{
				chassis.vw =   RC_Ctl.mouse.x * 10;
				Chassis_WSAD_Keyboard_Ctrl(3, 3, 20, 3000);			
			}
			else if(system_mode == RC_MACHINE_MODE)
			{
				chassis.vx = -(RC_Ctl.rc.ch3 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_X;				//遥控器值到底盘x轴速度的转换
				chassis.vy = -(RC_Ctl.rc.ch2 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_Y;				//遥控器值到底盘y轴速度的转换
				chassis.vw =  (RC_Ctl.rc.ch0 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_R;				//遥控器值到底盘w轴速度的转换
			}
		}break;
		
		case MANUAL_FOLLOW_GIMBAL://底盘跟随云台 FOR 陀螺仪模式
		{	
			if(system_mode == KEY_GYRO_MODE)
			{
				Chassis_WSAD_Keyboard_Ctrl(3, 3, 20, 3000);	
				chassis.vw =  (Yaw_M_MID - Motor_angle[YAW]) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_R;			         //陀螺仪模式下的w轴速度换算
			}
			else if(system_mode == RC_GYRO_MODE)
			{
				chassis.vx = -(RC_Ctl.rc.ch3 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_X;				//遥控器值到底盘x轴速度的转换
				chassis.vy = -(RC_Ctl.rc.ch2 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_Y;				//遥控器值到底盘y轴速度的转换
				chassis.vw =  (Yaw_M_MID - Motor_angle[YAW]) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_R;		        	//陀螺仪模式下的w轴速度换算
			}
		}break;
		
		case DODGE_MODE://摇摆模式 UNDER 陀螺仪模式
		{	
      Chassis_twist_handle();
		}break;

		default://																																									//default无输出
		{
			chassis.vx = 0;chassis.vy = 0;chassis.vw = 0;
			chassis.current[M1] = chassis.current[M2] = chassis.current[M3] = chassis.current[M4] = 0;
		}break;
	}
	
	if(chassis_mode == CHASSIS_STOP)
	{
	  CAN1_Send(0x200,chassis.current[M1],chassis.current[M2],chassis.current[M3],chassis.current[M4]);    //直接输出不计算
	}
	else
	{
	  mecanum_calc(chassis.vx, chassis.vy, chassis.vw, chassis.wheel_speed_ref);													//将底盘三个速度的目标值分解得到四个轮子的目标值
	
//		/*Roboremo调参，不用时请注释掉*/
//	  Roboremo_PID_tuning(&roboremo_pid.Inner_p, &roboremo_pid.Inner_i, &roboremo_pid.Inner_d, &roboremo_pid.Outter_p, &roboremo_pid.Outter_i, &roboremo_pid.Outter_d);//PID调参函数(这里可以拿来调底盘)
//    for(u8 k = 0; k < 4; k++)
//	  PIDCKZ_param_roboremo(&pid_chassis_speed[k], roboremo_pid.Inner_p, roboremo_pid.Inner_i, roboremo_pid.Inner_d);
//    /*Roboremo调参，不用时请注释掉*/
	
	  for(u8 k = 0; k < 4; k++)
	  chassis.current[k]=PIDCKZ_Calc(&pid_chassis_speed[k],chassis.wheel_speed_fdb[k],chassis.wheel_speed_ref[k]);                            //PID计算
	
//		/*Roboremo调参，不用时请注释掉*/
//	  Roboremo_Power_tuning(&roboremo_power.Total_Limit, &roboremo_power.Power_Threshold);//功率调参函数
//		Chassis_power_handle(roboremo_power.Total_Limit,roboremo_power.Power_Threshold);
//    /*Roboremo调参，不用时请注释掉*/		
		
		Chassis_power_handle(36000,60);
		
	  CAN1_Send(0x200,chassis.current[M1],chassis.current[M2],chassis.current[M3],chassis.current[M4]);    //can1发送电流值	
	}		

}

int rotation_center_gimbal = 0;//云台是否在底盘中心
void mecanum_calc(float vx, float vy, float vw, int16_t Wheel_speed[])
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
  wheel_rpm_ratio = 60.0f/(PERIMETER*CHASSIS_DECELE_RATIO);		 																							//	60/周长*减速比
  
	VAL_LIMIT(vx, -MAX_CHASSIS_VX_SPEED, MAX_CHASSIS_VX_SPEED);  																							//mm/s
  VAL_LIMIT(vy, -MAX_CHASSIS_VY_SPEED, MAX_CHASSIS_VY_SPEED);  																							//mm/s
  VAL_LIMIT(vw, -MAX_CHASSIS_VR_SPEED, MAX_CHASSIS_VR_SPEED);  																							//deg/s
  
	/*四个电机转速*/   //默认排序从左上角开始顺时针0123 可根据实际情况改
  wheel_rpm[0] = (-vx - vy + vw * rotate_ratio_fr) * wheel_rpm_ratio;
  wheel_rpm[1] = ( vx - vy + vw * rotate_ratio_fl) * wheel_rpm_ratio;
  wheel_rpm[3] = ( vx + vy + vw * rotate_ratio_bl) * wheel_rpm_ratio;
  wheel_rpm[2] = (-vx + vy + vw * rotate_ratio_br) * wheel_rpm_ratio;

  /*找到四个电机*/
  for (u8 i = 0; i < 4; i++)
  {
    if (abs(wheel_rpm[i]) > max)
      max = abs(wheel_rpm[i]);
  }
  /* equal proportion */
  if (max > MAX_WHEEL_RPM)
  {
    float rate = MAX_WHEEL_RPM / max;
    for (u8 i = 0; i < 4; i++)
      wheel_rpm[i] *= rate;
  }
  memcpy(Wheel_speed, wheel_rpm, 4*sizeof(int16_t));
}


/*键盘模式下的前后左右平移(包含扭腰同时移动的处理)*/
void Chassis_WSAD_Keyboard_Ctrl(float Acc_For_AD, float Acc_For_WS, float Acc_Brake, u16 Chassis_Speed)
{
		if(KEY_W_PRESSED) 
		{
			chassis.dx -= Acc_For_AD;
			chassis.dx = constrain(chassis.dx, -Chassis_Speed, Chassis_Speed);
		}
		else if(KEY_S_PRESSED)
		{
			chassis.dx += Acc_For_AD;
			chassis.dx = constrain(chassis.dx, -Chassis_Speed, Chassis_Speed);
		}
		else
		{
			if(chassis.dx > 0)
			{
				chassis.dx -= Acc_Brake;
				if(chassis.dx < 0) chassis.dx = 0;
			}
			else if(chassis.dx < 0)
			{
				chassis.dx += Acc_Brake;
				if(chassis.dx > 0) chassis.dx = 0;
			}
		  else chassis.dx = 0;
		}
		
		if(KEY_D_PRESSED) 
		{
			chassis.dy -= Acc_For_WS;
			chassis.dy = constrain(chassis.dy, -Chassis_Speed, Chassis_Speed);
		}
		else if(KEY_A_PRESSED)
		{
			chassis.dy += Acc_For_WS;
			chassis.dy = constrain(chassis.dy, -Chassis_Speed, Chassis_Speed);
		}
		else
		{
			if(chassis.dy > 0)
			{
				chassis.dy -= Acc_Brake;
				if(chassis.dy < 0) chassis.dy = 0;
			}
			else if(chassis.dy < 0)
			{
				chassis.dy += Acc_Brake;
				if(chassis.dy > 0) chassis.dy = 0;
			}
		  else chassis.dy = 0;
		}
		
		if(system_mode == KEY_DODGE_MODE)//扭腰模式需要坐标变换
		{
			chassis.vx =  chassis.dy * sin( gim.yaw_relative_angle / RADIAN_COEF) 
	                 +chassis.dx * cos( gim.yaw_relative_angle / RADIAN_COEF);
	
			chassis.vy =  chassis.dy * cos( gim.yaw_relative_angle / RADIAN_COEF) 
	                 -chassis.dx * sin( gim.yaw_relative_angle / RADIAN_COEF);
		}
		else//除了扭腰模式 原样输出
		{
      chassis.vx = chassis.dx;
			chassis.vy = chassis.dy;
		}
}


/*扭腰程序*/
uint32_t twist_count;
void Chassis_twist_handle()
{
  static int16_t twist_period = 1000;
  static int16_t twist_angle  = 40;

	twist_count++; 
	
  chassis.position_ref =  twist_angle * sin(2 * PI / twist_period * twist_count);

	chassis.vw = PIDCKZ_Calc(&pid_chassis_angle, gim.yaw_relative_angle, chassis.position_ref);	
	
	if(system_mode == KEY_DODGE_MODE)
	{
		Chassis_WSAD_Keyboard_Ctrl(3, 3, 20, 500);
	}
	else//除了键盘模式剩下的就是遥控器咯
	{
		chassis.vx = -((RC_Ctl.rc.ch2 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_Y) * sin( gim.yaw_relative_angle / RADIAN_COEF) 
	               -((RC_Ctl.rc.ch3 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_X) * cos( gim.yaw_relative_angle / RADIAN_COEF);
	
	  chassis.vy = -((RC_Ctl.rc.ch2 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_Y) * cos( gim.yaw_relative_angle / RADIAN_COEF) 
	               +((RC_Ctl.rc.ch3 - RC_CH_VALUE_OFFSET) / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_X) * sin( gim.yaw_relative_angle / RADIAN_COEF);
	}
}


/*功率限制*/
void Chassis_power_handle(float Total_Limit, float Power_Threshold)
{
	chassis.total_current = abs(chassis.current[M1]) + abs(chassis.current[M2]) + abs(chassis.current[M3]) + abs(chassis.current[M4]);//底盘总电流
	
	if(!PowerProtect.Judge_connect)//接不到电流电压信息 强制限制电流
	{
		chassis.total_current_limit = 10000;
	}
	else
	{
		if (PowerProtect.Judge_power_remain < Power_Threshold)
		{
			chassis.total_current_limit = (float)(PowerProtect.Judge_power_remain / 60.0f)*(float)(PowerProtect.Judge_power_remain / 60.0f) * Total_Limit;
		}
		else chassis.total_current_limit = Total_Limit;
	}
	
	if(chassis.total_current > chassis.total_current_limit)
	{
		chassis.current[M1] = chassis.current[M1] / chassis.total_current * chassis.total_current_limit;
		chassis.current[M2] = chassis.current[M2] / chassis.total_current * chassis.total_current_limit;
		chassis.current[M3] = chassis.current[M3] / chassis.total_current * chassis.total_current_limit;
		chassis.current[M4] = chassis.current[M4] / chassis.total_current * chassis.total_current_limit;
//		chassis.total_current = chassis.total_current_limit;
	}
}


