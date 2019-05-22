#include "chassis.h"

/* chassis task global parameter */
chassis_t chassis;

void chassis_param_init()
{
	memset(&chassis, 0, sizeof(chassis_t));
	
	chassis.spin_brake_finished = YES;
	
	pidckz_reset(&pid_chassis_speed);
	pidckz_reset(&pid_chassis_angle);
	pidckz_param_set(&pid_chassis_speed, 400, 300, 0.2f, 0.0f, 0.0f);    //陀螺仪模式YAW内环参数设定
	pidckz_param_set(&pid_chassis_angle, 4000, 50, 130.0f, 0.0f, 0.0f);	 //陀螺仪模式YAW外环参数设定
	
	pidckz_reset(&pid_vision_chassis_vy);
//	pidckz_param_set(&pid_vision_chassis_vy, 500, 100, 2.0f, 0.0045f, 1.8f);   

	
	for(u8 k = 0; k < 4; k++)
	{
	  pidckz_reset(&pid_wheel_speed[k]);		
		pidckz_param_set(&pid_wheel_speed[k], 10000, 1000, 9.0f, 0.4f, 16.0f);	//for底盘常规
	}
}

void chassis_task()
{
	keyboard_chassis_handler();
	remote_ctrl_chassis_handler();
	
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
		
		case MOVE_BACKWARD://向后走
		{	
      chassis_backward_handler();
		}break;
		
		case VISION_GET_BOMB://视觉对位(取弹)位置环模式
		{	
      chassis_vision_get_bomb_handler();
		}break;
			
		case VISION_FEED_BOMB://视觉对位(补弹)
		{	     
			chassis_vision_feed_bomb_handler();
		}break;
		
		case VISION_CLIMB_TUBE://视觉对位(爬杆)
		{	    			
			chassis_vision_climb_tube_handler();
		}break;
		
		case MOVE_CLOCKWISE://抱柱顺时针
		{	
      chassis_clockwise_handler();
		}break;
		
		case MOVE_ANTICLOCKWISE://抱柱逆时针
		{	
      chassis_anticlockwise_handler();
		}break;
		
		case CHASSIS_SEPARATE://机械模式
		{	
      chassis_separate_handler();
		}break;
						
		case CHASSIS_FOLLOW://陀螺仪模式
    {
			if(flag_gyro_blow_up)//陀螺仪炸了执行机械模式
	    {
				chassis_separate_handler();
			}
			else chassis_follow_handler();
    }break;	
		
		default://速度零锁住																																									
		{
      chassis_stop_handler();
		}break;
	}
	
	  mecanum_calc(chassis.vx, chassis.vy, chassis.vw, chassis.wheel_speed_ref);    //将底盘三个速度的目标值分解得到四个轮子的目标值
		
	  for(u8 k = 0; k < 4; k++)
		{
			chassis.current[k] = pidckz_calc(&pid_wheel_speed[k], chassis.wheel_speed_fdb[k], chassis.wheel_speed_ref[k]);                            //PID计算
		
			if((abs(chassis.current[k]) > 7000) || chassis.across_stall_protect[k])
			{
				if(chassis.stall_protect_record[k])
				{
					chassis.stall_protect_time[k] = millis();
					chassis.stall_protect_record[k] = DISABLE;
				} 
				if((millis() - chassis.stall_protect_time[k] > 2000) &&
					 (millis() - chassis.stall_protect_time[k] < 2040))
				{
					chassis.across_stall_protect[k] = ENABLE;
					
					if(chassis.current[k] > 0)
					{
						chassis.current[k] = 3000;
					}
					else chassis.current[k] = -3000;
				}
				else if(millis() - chassis.stall_protect_time[k] > 2040)
				{
					chassis.stall_protect_record[k] = ENABLE;
					chassis.across_stall_protect[k] = DISABLE;
				}				
			}
			else 
			{
				chassis.stall_protect_record[k] = ENABLE;
				chassis.across_stall_protect[k] = DISABLE;
			}
		}

		if (system_mode == SAFETY_MODE)
	  {
      memset(chassis.current, 0, sizeof(chassis.current));
  	}	
}

/**
  * @brief mecanum chassis velocity decomposition
  * @param input : ?=+vx(mm/s)  ?=+vy(mm/s)  ccw=+vw(deg/s)
  *        output: every wheel speed(rpm)
  * @note  1=FR 2=FL 3=BL 4=BR
  */
void mecanum_calc(float vx, float vy, float vw, int16_t Wheel_speed[])//麦轮公式计算
{
  static bool rotation_center_gimbal = 0;//云台是否在底盘中心的标志位
  static float rotate_ratio_fr;
  static float rotate_ratio_fl;
  static float rotate_ratio_bl;
  static float rotate_ratio_br;
  static float wheel_rpm_ratio;
	int16_t wheel_rpm[4];
  float   max = 0;
	
	/*绕柱走的旋转判定*/
//	if(chassis.ctrl_mode == MOVE_CLOCKWISE || 
//		 chassis.ctrl_mode == MOVE_ANTICLOCKWISE || flag_tube_claw_catch) 
	if(flag_tube_claw_catch) 
	{
		rotation_center_gimbal = 1;
	}
	else rotation_center_gimbal = 0;
	
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
  wheel_rpm[2] = ( vx + vy + vw * rotate_ratio_bl) * wheel_rpm_ratio;
  wheel_rpm[3] = (-vx + vy + vw * rotate_ratio_br) * wheel_rpm_ratio;

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

/*--------------------------------底盘特殊控制---------------------------------*/
uint16_t speed_dbg;
uint16_t section_dbg;

void chassis_stop_handler(void)//底盘速度环锁住
{	
  chassis.vy = chassis.vx = chassis.vw = 0;
	
	if(chassis.force_push_in_enable)//这个给取弹推进用
	{
	  chassis.vx = 170;
	}
  
	chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}


void chassis_forward_handler(void)//底盘向前跑
{
	if(flag_gyro_blow_up)//陀螺仪炸了照理来说是不能登岛了 但还是写着吧
	{
		chassis.vy = chassis.vw = 0;
		chassis.vx = 300;
	}
	else//陀螺仪没炸
	{
		/*w速度处理*/
		if(climb_tube.ctrl_mode == CLIMEB_UP_OPTIC_MODE ||
			 climb_tube.ctrl_mode == CLIMEB_UP_GYRO_MODE)//上岛使用
		{
			if(climb_tube.landing_angle_delay_time_record)
			{
				climb_tube.landing_angle_delay_time = millis();
				climb_tube.landing_angle_delay_time_record = DISABLE;
			}
			else if(millis() - climb_tube.landing_angle_delay_time > 500)//延时半秒钟稳住陀螺仪
			{
				if(climb_tube.landing_angle_record)
				{
					climb_tube.landing_angle = chassis.position_ref = imu.yaw;
					climb_tube.landing_angle_record = DISABLE;
				}
				chassis_yaw_handler();//这里面会有角度矫正
			}	
			else //延时过程往前跑一段
			{
				chassis.vw = 0; 
				chassis.position_ref = imu.yaw;		
			}			
		}
		else //下岛使用
		{
			chassis.vw = 0; 
			chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用	
		}
		
		/*x速度处理*/
		if(ramp_calc(&landing_angle_ramp) == 1)//那个斜坡算到1了
		{
			chassis.vx = 120;//矫正完了角度
		}
		else if(climb_tube.ctrl_mode == CLIMEB_UP_OPTIC_MODE ||
						climb_tube.ctrl_mode == CLIMEB_UP_GYRO_MODE)
		{
			chassis.vx = 950;//边矫正角度边往前冲
		}
		else 
		{/*下岛的时候 延时一段时间*/
//			if(climb_tube.landing_angle_delay_time_record)
//			{
//				climb_tube.landing_angle_delay_time = millis();
//				climb_tube.landing_angle_delay_time_record = DISABLE;
//			}
//			else if(millis() -climb_tube.landing_angle_delay_time > 600)//延时半秒钟稳住陀螺仪
//			{
				chassis.vx = 500;//加快一点防止张不开爪子
//			}
		}
		
		/*y速度处理*/
		if(climb_tube.ctrl_mode == CLIMEB_UP_OPTIC_MODE ||
			 climb_tube.ctrl_mode == CLIMEB_UP_GYRO_MODE)
		{
			chassis.vy = rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;
		}
		else chassis.vy = 0;//下岛的时候
	}
}

void chassis_backward_handler(void)//底盘向后跑
{
  chassis.vy = chassis.vw = 0;
  chassis.vx = -200;
  	
  chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_vision_get_bomb_handler(void)//位置环底盘视觉对位(取弹)  
{
  pidckz_calc(&pid_vision_chassis_vy, vision.y_ref, vision.y_fdb);
	
	if(pid_vision_chassis_vy.err > 100)
	{
		if(climb_tube.location == ON_GROUND) chassis.vy = 300;
		else chassis.vy = 300;
	}
	else if(pid_vision_chassis_vy.err < -100)
	{
	 if(climb_tube.location == ON_GROUND) chassis.vy = -300;
	 else chassis.vy = -300;
	}
	else chassis.vy = pid_vision_chassis_vy.out;
	
  chassis.vx = 170;
  chassis.vw = 0;
  chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_vision_feed_bomb_handler(void)//底盘视觉对位(补弹)(不完全机械模式)
{
	pidckz_calc(&pid_vision_chassis_vy, vision.y_ref, vision.y_fdb);
	
	if(pid_vision_chassis_vy.err > 30)
	{
		chassis.vy = 200 + rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;
	}
	else if(pid_vision_chassis_vy.err < -30)
	{
		chassis.vy = -200 + rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;
	}
	else chassis.vy = pid_vision_chassis_vy.out + rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;
	
//	chassis.vy = 0;
	
  chassis.vx = rc.vx * CHASSIS_RC_MOVE_RATIO_X + kb.vx * CHASSIS_KB_MOVE_RATIO_X;
	
  if(chassis.ass_mode_enable)//车尾模式
	{
    chassis.vw = -(rc.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R);
	}
	else//车头模式
	{
    chassis.vw = rc.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R;
	}
  chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_vision_climb_tube_handler(void)//底盘视觉对位(爬杆)(不完全机械模式)
{
	pidckz_calc(&pid_vision_chassis_vy, vision.y_ref, vision.y_fdb);
	
	if(pid_vision_chassis_vy.err > 40)
	{
		chassis.vy = -280 -(rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y);
	}
	else if(pid_vision_chassis_vy.err < -40)
	{
		chassis.vy = 280 -(rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y);
	}
	else chassis.vy = - pid_vision_chassis_vy.out -(rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y);
	
//	chassis.vy = 0;
	
  chassis.vx = rc.vx * CHASSIS_RC_MOVE_RATIO_X + kb.vx * CHASSIS_KB_MOVE_RATIO_X;
	
  chassis.vw = -(rc.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R);
	
  chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_clockwise_handler(void)//底盘绕柱顺时针
{
  chassis.vy = chassis.vx = 0;  
  chassis.vw = 150;
	
  chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_anticlockwise_handler(void)//底盘绕柱逆时针
{
  chassis.vy = chassis.vx = 0;
  chassis.vw = -150;
	
  chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

/*--------------------------------底盘机械模式和陀螺仪模式---------------------------------*/

void chassis_separate_handler(void)//机械模式
{
  chassis.vx = rc.vx * CHASSIS_RC_MOVE_RATIO_X + kb.vx * CHASSIS_KB_MOVE_RATIO_X;
	
	if(flag_watch_CCTV && flag_mirror_CCTV)
	{
		chassis.vy = -(rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y);	
	}
	else chassis.vy = rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;	

	
	if(chassis.force_push_in_enable)//这个给取弹推进用
	{
	  chassis.vx += 170 ;
	}
	
  if(chassis.ass_mode_enable)//车尾模式
	{
		if(flag_watch_CCTV && flag_mirror_CCTV)
		{
			chassis.vw = rc.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R;
		}
		else chassis.vw = -(rc.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R);
	}
	else//车头模式
	{
		if(flag_watch_CCTV && flag_mirror_CCTV)
		{
			chassis.vw = -(rc.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R);
		}
    else chassis.vw = rc.vw * CHASSIS_RC_MOVE_RATIO_R + kb.vw * CHASSIS_KB_MOVE_RATIO_R;
	}
	if(flag_spin_around)
	{
		chassis.vw = 240;
		chassis.spin_brake_finished = NO;
		ramp_init(&spin_brake_ramp, SPIN_BRAKE_TIME/INFO_GET_PERIOD);		
	}
	else
	{
		if(!chassis.spin_brake_finished)//还没刹完车
		{
			chassis.vw = 240 * (1 - ramp_calc(&spin_brake_ramp));
			if(ramp_calc(&spin_brake_ramp) == 1)
			{
			  chassis.spin_brake_finished  = YES;
			}
			chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
		}
	}
	
	chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
}

void chassis_follow_handler(void)//陀螺仪模式
{  
	chassis.vx = rc.vx * CHASSIS_RC_MOVE_RATIO_X + kb.vx * CHASSIS_KB_MOVE_RATIO_X;
	
	if(flag_watch_CCTV && flag_mirror_CCTV)
	{
		chassis.vy = -(rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y);
	}
	else chassis.vy = rc.vy * CHASSIS_RC_MOVE_RATIO_Y + kb.vy * CHASSIS_KB_MOVE_RATIO_Y;
	
	if(chassis.force_draft_out_enable)//这个给拖车拔出钩子用的
	{
		chassis.vx = -200 + chassis.vx;
	}
	
	if(flag_spin_around)
	{
		chassis.vw = 240;
		chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用	
		chassis.spin_brake_finished = NO;
		ramp_init(&spin_brake_ramp, SPIN_BRAKE_TIME/INFO_GET_PERIOD);		
	}
	else //关掉自转的时候添加斜坡刹车
	{
		if(!chassis.spin_brake_finished)//还没刹完车
		{
			chassis.vw = 240 * (1 - ramp_calc(&spin_brake_ramp));
			if(ramp_calc(&spin_brake_ramp) == 1)
			{
			  chassis.spin_brake_finished  = YES;
			}
			chassis.position_ref = imu.yaw;//记录陀螺仪的角度给陀螺仪模式用
		}
		else chassis_yaw_handler();//YAW轴处理得出vw值 
	}
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
		chassis.position_ref += -(RC_Ctl.rc.ch0-RC_CH_VALUE_OFFSET)*0.0007;//遥控器vw			
	}
	else if(kb.kb_enable) 
	{
		if(kb.move_speed == SLOW_SPEED) 
		{
			if(flag_watch_CCTV && flag_mirror_CCTV)
			{
				chassis.position_ref += RC_Ctl.mouse.x * 0.0033;//键盘vw
			}
		  else chassis.position_ref += -RC_Ctl.mouse.x * 0.0033;//键盘vw
		}
		else
		{
			if(flag_watch_CCTV && flag_mirror_CCTV)
			{
				chassis.position_ref += RC_Ctl.mouse.x * 0.018;//键盘vw
			}
		  else chassis.position_ref += -RC_Ctl.mouse.x * 0.018;//键盘vw
		}	
		
		if(chassis.ctrl_mode == MOVE_FORWARD)
		{
			chassis.position_ref =  climb_tube.landing_angle - 25 * ramp_calc(&landing_angle_ramp);//
		}
		
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
  chassis.yaw_angle_ref = chassis.position_ref;                                      //外环(角度环)目标值
	chassis.yaw_angle_fdb = imu.yaw;                                                   //获取陀螺仪角度作为当前角度
	critical_value_treatment(chassis.yaw_angle_fdb, &chassis.yaw_angle_ref,0,360);   //临界值处理
	pidckz_calc(&pid_chassis_angle, chassis.yaw_angle_fdb, chassis.yaw_angle_ref);      //PID计算得到外环输出(以角速度形式输出)	
}

void chassis_inner_loop(void)//陀螺仪模式内环
{
	//yaw串级
	chassis.yaw_speed_ref = pid_chassis_angle.out;                                //外环输出作为目标值
	chassis.yaw_speed_fdb = imu.gz;                                             //读取陀螺仪的角速度值
	pidckz_calc(&pid_chassis_speed, chassis.yaw_speed_ref, chassis.yaw_speed_fdb);    //PID计算得到最终输出(注意谁减谁)
	chassis.vw = pid_chassis_speed.out;
}

void chassis_yaw_handler(void)
{
	chassis_yaw_target();
	chassis_outter_loop();
	chassis_inner_loop();
}

