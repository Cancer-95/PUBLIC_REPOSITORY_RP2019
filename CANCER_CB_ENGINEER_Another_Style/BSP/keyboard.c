#include "keyboard.h"

kb_ctrl_t kb;
global_task_t global_task;

void global_task_init(void)
{
	memset(&global_task, ENABLE, sizeof(global_task_t));
}
/*--------------------------------------底盘键盘控制--------------------------------------------*/

/*底盘速度控制*/
static void move_spd_ctrl(uint8_t fast, uint8_t slow)
{
	if(fast)
  {
    kb.move_speed = FAST_SPEED;
    kb.x_spd_limit = CHASSIS_KB_MAX_SPEED_X;
    kb.y_spd_limit = CHASSIS_KB_MAX_SPEED_Y;
		kb.w_spd_limit = CHASSIS_KB_MAX_SPEED_R;
  }
  else if(slow)
  {
    kb.move_speed = SLOW_SPEED;
		if(climb_tube.ctrl_mode != PLAIN_MODE)
		{
			kb.x_spd_limit = 0.1f * CHASSIS_KB_MAX_SPEED_X;
			kb.y_spd_limit = 0.1f * CHASSIS_KB_MAX_SPEED_Y;
			kb.w_spd_limit = 0.1f * CHASSIS_KB_MAX_SPEED_R;				
		}
		else
		{
			kb.x_spd_limit = 0.18f * CHASSIS_KB_MAX_SPEED_X;
			kb.y_spd_limit = 0.18f * CHASSIS_KB_MAX_SPEED_Y;
			kb.w_spd_limit = 0.32f * CHASSIS_KB_MAX_SPEED_R;		
		}

  }
  else
  {
    kb.move_speed = NORMAL_SPEED;
    kb.x_spd_limit = 0.8f * CHASSIS_KB_MAX_SPEED_X;
    kb.y_spd_limit = 0.8f * CHASSIS_KB_MAX_SPEED_Y;
		kb.w_spd_limit = 0.8f * CHASSIS_KB_MAX_SPEED_R;
  }
}
/*底盘方向控制*/
static void move_direction_ctrl(bool forward, bool back, bool left, bool right)
{
  if (forward)//前进
  {		
		kb.vx = kb.x_spd_limit * ramp_kb_calc(&fb_ramp, CNT_DOWN, KEY_ACC_TIME/INFO_GET_PERIOD);
  }
  else if (back)//后退
  {
		kb.vx = kb.x_spd_limit * ramp_kb_calc(&fb_ramp, CNT_UP, KEY_ACC_TIME/INFO_GET_PERIOD);			
  }
  else//x方向刹车
  {		
		kb.vx = kb.x_spd_limit * ramp_kb_calc(&fb_ramp, TO_ZERO, KEY_ACC_TIME/INFO_GET_PERIOD);
	}
	
  if (left)//左平移
  {
		kb.vy = kb.y_spd_limit * ramp_kb_calc(&lr_ramp, CNT_UP, KEY_ACC_TIME/INFO_GET_PERIOD);
  }
  else if (right)//右平移
  {
  	kb.vy = kb.y_spd_limit * ramp_kb_calc(&lr_ramp, CNT_DOWN, KEY_ACC_TIME/INFO_GET_PERIOD);
  }
  else//y方向刹车
  {
  	kb.vy = kb.y_spd_limit * ramp_kb_calc(&lr_ramp, TO_ZERO, KEY_ACC_TIME/INFO_GET_PERIOD);
  }
	
	if(chassis.ctrl_mode == CHASSIS_SEPARATE ||
		 chassis.ctrl_mode == VISION_FEED_BOMB ||
	   chassis.ctrl_mode == VISION_CLIMB_TUBE ||
		 flag_gyro_blow_up)//底盘是机械模式或者标记了陀螺仪爆炸
	{
		if(chassis.ass_mode_enable)//车尾模式
		{		  
      if(chassis.ctrl_mode == VISION_FEED_BOMB ||
				 chassis.ctrl_mode == VISION_CLIMB_TUBE)//补弹模式下还得更慢
			{
			  kb.vw = -RC_Ctl.mouse.x;
			}		
      else kb.vw = -RC_Ctl.mouse.x * 10;				
		}
		else //车头模式
		{
			if(chassis.ctrl_mode == VISION_FEED_BOMB ||
			   chassis.ctrl_mode == VISION_CLIMB_TUBE)//补弹模式下还得更慢
			{
			  kb.vw = RC_Ctl.mouse.x * 5;
			}		
      else kb.vw = RC_Ctl.mouse.x * 10;			
		}   
		VAL_LIMIT(kb.vw, -kb.w_spd_limit, kb.w_spd_limit);
	}
	
//	else if(chassis.ctrl_mode == CHASSIS_FOLLOW)
//	{
//	
//	}
}
/*底盘掉头控制*/
bool flag_turn_around;
static void kb_turn_around_ctrl(uint8_t turn_around)
{
	if(turn_around)
	{
		flag_turn_around = 1;
		chassis.position_temp = imu.yaw;
		ramp_init(&turn_around_ramp, TUR_ARO_TIME/INFO_GET_PERIOD);		
	}
	else if(!turn_around && flag_turn_around) 
	{
		/*斜坡开平方根输出 可达到刹车效果虽然不是很明显 可减少掉头时间200ms而不超调*/
		chassis.position_ref = chassis.position_temp + 180 * sqrt(ramp_calc(&turn_around_ramp));    
		if(ramp_calc(&turn_around_ramp) == 1) flag_turn_around = 0;
	}
}

/*底盘左转控制*/
bool flag_turn_left;
static void kb_turn_left_ctrl(uint8_t turn_left)
{
	if(!turn_left && flag_turn_left) //松开按键才开始转
	{ 
		/*这里的斜坡就简单线性输出吧*/
		chassis.position_ref = chassis.position_temp + 90 * ramp_calc(&turn_left_ramp);    
		if(ramp_calc(&turn_left_ramp) == 1) flag_turn_left = 0;
	}
	else if(turn_left)//转完才重新开始检测(失败了)
	{
		flag_turn_left = 1;
		chassis.position_temp = imu.yaw;
		ramp_init(&turn_left_ramp, TUR_LEF_TIME/INFO_GET_PERIOD);		
	}
}

/*底盘右转控制*/
bool flag_turn_right;
static void kb_turn_right_ctrl(uint8_t turn_right)
{
  if(!turn_right && flag_turn_right) //松开按键才开始转
	{
		/*这里的斜坡就简单线性输出吧*/
		chassis.position_ref = chassis.position_temp - 90 * ramp_calc(&turn_right_ramp);    
		if(ramp_calc(&turn_right_ramp) == 1) flag_turn_right = 0;
	}
	else if(turn_right)//转完才重新开始检测(失败了)
	{
		flag_turn_right = 1;
		chassis.position_temp = imu.yaw;
		ramp_init(&turn_right_ramp, TUR_RIG_TIME/INFO_GET_PERIOD);		
	}
}

/*底盘原地打转控制*/
bool flag_spin_around;
static void kb_spin_around_ctrl(uint8_t spin_around)
{
	if(spin_around)
	{
		flag_spin_around = ENABLE;
	}
	else flag_spin_around = DISABLE;
}

/*陀螺仪炸了怎么办*/
bool flag_gyro_blow_up = 0;
static void kb_gyro_blow_up_ctrl(uint8_t gyro_blow_up_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(gyro_blow_up_ctrl && flag_toggle)
	{
		flag_gyro_blow_up = !flag_gyro_blow_up;
		flag_toggle = 0;
	}
	else if(!gyro_blow_up_ctrl) flag_toggle = 1;	
	
//	if(flag_gyro_blow_up)//根据flag判断要做什么
//	{/*就随便写点东西*/
//		chassis.turn_around_disable = 1; //关掉掉头
//	}		
//	else chassis.turn_around_disable = 0;
}

/*底盘键盘操作集合*/
void keyboard_chassis_handler(void)
{ 
	if (kb.kb_enable)
  { 
		/*陀螺仪炸了怎么办*/
		kb_gyro_blow_up_ctrl(GYRO_BLOW_UP_CTRL);
		/*陀螺仪标记为爆炸了之后 任何靠陀螺仪的运动都会失能 如掉头 左右转等 因为底盘都没法进陀螺仪模式*/
		
		/*速度切换*/
		if(chassis.force_slow_enable)
		{
		  move_spd_ctrl(FAST_SPD_DISABLE, SLOW_SPD_ENABLE);	//强制慢速
		}
    else move_spd_ctrl(FAST_SPD, SLOW_SPD);//手动切速
		/*车头车尾模式*/
		if(chassis.ass_mode_enable) 
		{
			move_direction_ctrl(BACK, FORWARD, RIGHT, LEFT);//车尾模式
		}			
    else move_direction_ctrl(FORWARD, BACK, LEFT, RIGHT);//车头模式
		
		/*掉头*/
		if((chassis.ctrl_mode == CHASSIS_SEPARATE) || 
			  chassis.ctrl_mode == CHASSIS_FOLLOW)//底盘可控的情况下才使能掉头(其实条件中可去掉机械模式) 因为进入某些模式需要先掉头
		{
//			kb_spin_around_ctrl(SPIN_AROUND);//SPIN可控
			kb_turn_around_ctrl(TURN_AROUND);//掉头可控
		}
		else 
		{
//			kb_spin_around_ctrl(SPIN_AROUND_DISABLE);//SPIN不可控
			kb_turn_around_ctrl(TURN_AROUND_DISABLE);//掉头不可控			
		}

    /*左转与右转 | 原地打转*/
		if(!climb_tube.climb_enable && 
			 !get_bomb.get_bomb_enable &&
		   !rescue.rescue_enable &&
		   !feed_bomb.feed_bomb_enable &&
		   MOTORS_ALL_READY)//非工作状态下允许
		{
			kb_turn_left_ctrl(TURN_LEFT);    //左转可控
			kb_turn_right_ctrl(TURN_RIGHT);  //右转可控	
			kb_spin_around_ctrl(SPIN_AROUND);//SPIN可控
//			kb_turn_around_ctrl(TURN_AROUND);//掉头可控			
		}
		else
		{
			kb_turn_left_ctrl(TURN_LEFT_DISABLE);    //左转不可控
			kb_turn_right_ctrl(TURN_RIGHT_DISABLE);  //右转不可控		
			kb_spin_around_ctrl(SPIN_AROUND_DISABLE);//SPIN不可控
//			kb_turn_around_ctrl(TURN_AROUND_DISABLE);//掉头不可控
		}
	}
  else
  {
    kb.vx = 0;
    kb.vy = 0;
		kb.vw = 0;
  }
}


/*--------------------------------------爬杆键盘控制--------------------------------------------*/

/*爬杆爪子抓取控制*/
bool flag_tube_claw_catch = 0;
static void kb_tube_claw_catch_ctrl(uint8_t tube_claw_catch_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(tube_claw_catch_ctrl && flag_toggle)
	{
		flag_tube_claw_catch = !flag_tube_claw_catch;
		flag_toggle = 0;
	}
	else if(!tube_claw_catch_ctrl) flag_toggle = 1;	
	
	if(flag_tube_claw_catch)//根据flag判断要做什么
	{
		TUBE_CLAW_CATCH;
	}		
	else TUBE_CLAW_LOOSE;
}

/*爬杆爪子是否视觉控制*/
bool flag_tube_claw_vision = 0;
static void kb_tube_claw_vision_ctrl(uint8_t tube_claw_vision_ctrl) 
{
	static bool flag_toggle = 1;
	if(climb_tube.vision_enable)
	{
		if(tube_claw_vision_ctrl && flag_toggle)
		{
			flag_tube_claw_vision = !flag_tube_claw_vision;
			flag_toggle = 0;
		}
		else if(!tube_claw_vision_ctrl) flag_toggle = 1;

		if(flag_tube_claw_vision)//根据flag判断要做什么
		{
			vision.enable = DISABLE;
		}		
		else vision.enable = ENABLE;
	}	
}

/*爬杆爪子控制集合*/
static void kb_tube_claw_ctrl(uint8_t tube_claw_catch_ctrl,uint8_t tube_claw_vision_ctrl)
{
	if (kb.tube_claw_enable)//键盘模式下且爪子使能
	{
		kb_tube_claw_catch_ctrl(tube_claw_catch_ctrl);
		kb_tube_claw_vision_ctrl(tube_claw_vision_ctrl);
	}
}

/*爬杆动作控制集合*/
//bool flag_force_plain = 0;
static void kb_climb_tube_ctrl(uint8_t climb_up_optic, 
	                             uint8_t climb_down_optic, 
                               uint8_t climb_up_gyro, 
                               uint8_t climb_down_gyro, 
                               uint8_t run_plain)
{	
	if(climb_up_optic && climb_tube.climb_up_optic_enable)//在岛下抬升爬杆框架 ctrl+Q强制
	{
		global_task.feed_bomb_task_enable = DISABLE;
		global_task.get_bomb_task_enable = DISABLE;
		global_task.rescue_task_enable = DISABLE;
		
		climb_tube.climb_down_optic_enable = DISABLE;
		climb_tube.climb_up_gyro_enable = DISABLE;
		climb_tube.climb_down_gyro_enable = DISABLE;
		
		climb_tube.location = ON_GROUND;//强制标记
		climb_tube.climb_enable = ENABLE;
		climb_tube.ctrl_mode = CLIMEB_UP_OPTIC_MODE;/*光电方法上杆*/
	}	
  else if(climb_down_optic && climb_tube.climb_down_optic_enable)//在岛上抬升爬杆框架	ctrl+E强制
	{
		global_task.feed_bomb_task_enable = DISABLE;
		global_task.get_bomb_task_enable = DISABLE;
		global_task.rescue_task_enable = DISABLE;
		
		climb_tube.climb_up_optic_enable = DISABLE;
		climb_tube.climb_up_gyro_enable = DISABLE;
		climb_tube.climb_down_gyro_enable = DISABLE;
		
	  climb_tube.location = ON_ISLAND;//强制标记
		climb_tube.climb_enable = ENABLE;
	  climb_tube.ctrl_mode = CLIMEB_DOWN_OPTIC_MODE;/*光电方法下杆*/
	}
	else if(climb_up_gyro && climb_tube.climb_up_gyro_enable)//在岛下抬升爬杆框架 shift+Q强制
	{
		global_task.feed_bomb_task_enable = DISABLE;
		global_task.get_bomb_task_enable = DISABLE;
		global_task.rescue_task_enable = DISABLE;
		
		climb_tube.climb_up_optic_enable = DISABLE;
		climb_tube.climb_down_optic_enable = DISABLE;
		climb_tube.climb_down_gyro_enable = DISABLE;
		
		climb_tube.location = ON_GROUND;//强制标记
		climb_tube.climb_enable = ENABLE;
		climb_tube.ctrl_mode = CLIMEB_UP_GYRO_MODE;/*陀螺仪方法上杆*/
	}	
  else if(climb_down_gyro && climb_tube.climb_down_gyro_enable)//在岛上抬升爬杆框架	shift+E强制
	{
		global_task.feed_bomb_task_enable = DISABLE;
		global_task.get_bomb_task_enable = DISABLE;
		global_task.rescue_task_enable = DISABLE;
		
		climb_tube.climb_up_optic_enable = DISABLE;
		climb_tube.climb_down_optic_enable = DISABLE;
		climb_tube.climb_up_gyro_enable = DISABLE;
		
	  climb_tube.location = ON_ISLAND;//强制标记
		climb_tube.climb_enable = ENABLE;
	  climb_tube.ctrl_mode = CLIMEB_DOWN_GYRO_MODE;/*陀螺仪方法下杆*/
	}
	else if(run_plain)
	{	
		climb_tube.ctrl_mode = PLAIN_MODE;	//执行完一遍case里的东西就失能掉climb_enable	
//	  flag_force_plain =1;//标记为强制平原
	}	
}
/*爬杆键盘操作集合*/
void keyboard_climb_tube_handler(void)
{
	if(kb.kb_enable)
	{
		if(global_task.climb_tube_task_enable)
		{
			if(climb_tube.climb_enable == ENABLE)
			{
				kb_tube_claw_ctrl(TUBE_CLAW_CATCH_CTRL,
				                  TUBE_CLAW_VISION_CTRL);
			}
			kb_climb_tube_ctrl(CLIMB_UP_OPTIC, 
												 CLIMB_DOWN_OPTIC, 
												 CLIMB_UP_GYRO, 
												 CLIMB_DOWN_GYRO, 
												 RUN_PLAIN);		
		}
	} 
}

/*--------------------------------------取弹键盘控制--------------------------------------------*/

/*取弹爪子夹取控制*/
bool flag_bomb_claw_catch = 0;
static void kb_bomb_claw_catch_ctrl(uint8_t bomb_claw_catch_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(get_bomb.bomb_claw_catch_enable)//在翻转那里使能了才给控制
	{
		if(bomb_claw_catch_ctrl && flag_toggle)
		{
			flag_bomb_claw_catch = !flag_bomb_claw_catch;
			flag_toggle = 0;
		}
		else if(!bomb_claw_catch_ctrl) flag_toggle = 1;		
	}

	if(flag_bomb_claw_catch)//根据flag判断要做什么
	{
		BOMB_CLAW_CATCH;
	}		
	else BOMB_CLAW_LOOSE;
}
/*取弹爪子翻转控制*/
bool flag_bomb_claw_roll = 0;
static void kb_bomb_claw_roll_ctrl(uint8_t bomb_claw_roll_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(bomb_claw_roll_ctrl && flag_toggle)
	{
		flag_bomb_claw_roll = !flag_bomb_claw_roll;
		flag_toggle = 0;
	}
	else if(!bomb_claw_roll_ctrl) flag_toggle = 1;

	if(flag_bomb_claw_roll)//根据flag判断要做什么
	{
		BOMB_CLAW_ROLL_ZERO;
		
		if((abs(bomb_claw.total_angle[L] - bomb_claw.target_zero[L]) < 100) || 
			 (abs(bomb_claw.total_angle[R] - bomb_claw.target_zero[R]) < 100))			
		{
			get_bomb.bomb_claw_catch_enable = ENABLE;			
		}

	}		
	else 
	{
		BOMB_CLAW_ROLL_HALF;		
		get_bomb.bomb_claw_catch_enable = DISABLE;
	}
}

/*取弹爪子推出控制*/
bool flag_bomb_claw_push = 0;
static void kb_bomb_claw_push_ctrl(uint8_t bomb_claw_push_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(bomb_claw_push_ctrl && flag_toggle)
	{
		flag_bomb_claw_push = !flag_bomb_claw_push;
		flag_toggle = 0;
	}
	else if(!bomb_claw_push_ctrl) flag_toggle = 1;

	if(flag_bomb_claw_push)//根据flag判断要做什么
	{
		BOMB_CLAW_PUSH;
	}		
	else BOMB_CLAW_PULL;
}

/*取弹爪子是否三箱控制*/
bool flag_bomb_claw_triple = 0;
static void kb_bomb_claw_triple_ctrl(uint8_t bomb_claw_triple_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(bomb_claw_triple_ctrl && flag_toggle)
	{
		flag_bomb_claw_triple = !flag_bomb_claw_triple;
		flag_toggle = 0;
	}
	else if(!bomb_claw_triple_ctrl) flag_toggle = 1;

	if(flag_bomb_claw_triple)//根据flag判断要做什么
	{
		get_bomb.scan = 0;
		get_bomb.flag_scan_failed = NO;
	  get_bomb.scan_finished = NO;	
	  optic_switch.value_record	= ENABLE;	
		get_bomb.scan_delay_record = ENABLE;				
		flag_bomb_claw_scan = 0; //关掉扫描模式

		get_bomb.once_penta = 0;	
		flag_bomb_claw_penta = 0; //关掉五箱模式
		
//		vision.enable = DISABLE;
//		vision.aim = HAVENT;
//		get_bomb.vision_assist = 0;	
//		flag_bomb_claw_vision = 0; //关掉视觉模式
		
		get_bomb.once_triple = ENABLE;
	}		
	else 
	{
		get_bomb.once_triple = DISABLE;
	}
}

/*取弹爪子是否五箱控制*/
bool flag_bomb_claw_penta = 0;
static void kb_bomb_claw_penta_ctrl(uint8_t bomb_claw_penta_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(bomb_claw_penta_ctrl && flag_toggle)
	{
		flag_bomb_claw_penta = !flag_bomb_claw_penta;
		flag_toggle = 0;
	}
	else if(!bomb_claw_penta_ctrl) flag_toggle = 1;

	if(flag_bomb_claw_penta)//根据flag判断要做什么
	{
		get_bomb.scan = 0;
		get_bomb.flag_scan_failed = NO;
	  get_bomb.scan_finished = NO;	
	  optic_switch.value_record	= ENABLE;	
		get_bomb.scan_delay_record = ENABLE;				
		flag_bomb_claw_scan = 0; //关掉扫描模式

		get_bomb.once_triple = 0;	
		flag_bomb_claw_triple = 0; //关掉三箱模式
				
//		vision.enable = DISABLE;
//		vision.aim = HAVENT;
//		get_bomb.vision_assist = 0;	
//		flag_bomb_claw_vision = 0; //关掉视觉模式
		
		get_bomb.once_penta = ENABLE;
	}		
	else 
	{
		get_bomb.once_penta = DISABLE;
	}
}

/*取弹爪子是否视觉控制*/
bool flag_bomb_claw_vision = 0;
static void kb_bomb_claw_vision_ctrl(uint8_t bomb_claw_vision_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(get_bomb.get_bomb_enable)
	{
		if(bomb_claw_vision_ctrl && flag_toggle)
		{
			flag_bomb_claw_vision = !flag_bomb_claw_vision;
			flag_toggle = 0;
		}
		else if(!bomb_claw_vision_ctrl) flag_toggle = 1;

		if(flag_bomb_claw_vision)//根据flag判断要做什么
		{
	//		get_bomb.scan = 0;
	//	  get_bomb.flag_scan_failed = NO;
	//	  get_bomb.scan_finished = NO;	
	//	  optic_switch.value_record	= ENABLE;	
  //    get_bomb.scan_delay_record = ENABLE;				
	//		flag_bomb_claw_scan = 0; //关掉扫描模式

	//		get_bomb.once_triple = 0;	
	//		flag_bomb_claw_triple = 0; //关掉三箱模式
	//		
	//		get_bomb.once_penta = 0;	
	//		flag_bomb_claw_penta = 0; //关掉五箱模式
			
	//		get_bomb.vision_assist = ENABLE;
			vision.enable = ENABLE;
		}		
		else 
		{
	//		get_bomb.vision_assist = DISABLE;
			vision.enable = DISABLE;
		}	
	}
}

/*取弹爪子扫描控制*/
bool flag_bomb_claw_scan = 0;
static void kb_bomb_claw_scan_ctrl(uint8_t bomb_claw_scan_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(bomb_claw_scan_ctrl && flag_toggle)
	{
		flag_bomb_claw_scan = !flag_bomb_claw_scan;
		flag_toggle = 0;
	}
	else if(!bomb_claw_scan_ctrl) flag_toggle = 1;

	if(flag_bomb_claw_scan)//根据flag判断要做什么
	{
		get_bomb.once_triple = 0;	
		flag_bomb_claw_triple = 0; //关掉三箱模式
		
		get_bomb.once_penta = 0;	
		flag_bomb_claw_penta = 0; //关掉五箱模式
		
//		vision.enable = DISABLE;
//		vision.aim = HAVENT;
//		get_bomb.vision_assist = 0;	
//		flag_bomb_claw_vision = 0; //关掉视觉模式
		
		get_bomb.scan = ENABLE;
	}		
	else 
	{
		get_bomb.scan = DISABLE;
		get_bomb.flag_scan_failed = NO;
		get_bomb.scan_finished = NO;	
	  optic_switch.value_record	= ENABLE;	
    get_bomb.scan_delay_record = ENABLE;			
    flag_bomb_claw_scan = 0;  
		
		/*修正中心值Y轴*/	
		if(bomb_claw.target_half[Y] > bomb_claw.Y_target[M])
		{
			bomb_claw.from_claw_entire[Y_axis] = YES;
			bomb_claw.from_claw_zero[Y_axis] = NO;
		}	
		else if(bomb_claw.target_half[Y] < bomb_claw.Y_target[M])
		{
			bomb_claw.from_claw_entire[Y_axis] = NO;
			bomb_claw.from_claw_zero[Y_axis] = YES;
		}		
		bomb_claw.target_half[Y] = bomb_claw.Y_target[M];
		if(!vision.enable)
		{
			/*修正框架目标值*/ 
			if(lifter.target_bomb_bit[L] != lifter.target_bomb_bit_normal[L] || 
				 lifter.target_bomb_bit[R] != lifter.target_bomb_bit_normal[R])		
			{
				lifter.target_bomb_bit[L] = lifter.target_bomb_bit_normal[L];
				lifter.target_bomb_bit[R] = lifter.target_bomb_bit_normal[R];
				if((lifter.total_angle[L] > lifter.target_bomb_bit[L]) && 
					 (lifter.total_angle[R] < lifter.target_bomb_bit[R]))
				{
					from_lifter_zero = YES;
					from_lifter_bomb_entire = NO; 			
				}
				else if((lifter.total_angle[L] < lifter.target_bomb_bit[L]) && 
					      (lifter.total_angle[R] > lifter.target_bomb_bit[R]))
				{
					from_lifter_zero = NO;
					from_lifter_bomb_entire = YES; 			
				}
	
			}		
		}
	}
}

/*CCTV切换的控制*/
bool flag_bomb_claw_CCTV = 0;
static void kb_bomb_claw_CCTV_ctrl(uint8_t bomb_claw_CCTV_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(bomb_claw_CCTV_ctrl && flag_toggle)
	{
		flag_bomb_claw_CCTV = !flag_bomb_claw_CCTV;
		flag_toggle = 0;
	}
	else if(!bomb_claw_CCTV_ctrl) flag_toggle = 1;

	if(flag_bomb_claw_CCTV)//根据flag判断要做什么
	{
		flag_bomb_claw_CCTV_switch = 1;
	}		
	else flag_bomb_claw_CCTV_switch =0;
}

/*取弹爪子动作控制集合*/
static void kb_bomb_claw_ctrl(uint8_t bomb_claw_catch_ctrl, 
									        		uint8_t bomb_claw_roll_ctrl, 
										        	uint8_t bomb_claw_push_ctrl, 
											        uint8_t bomb_claw_triple_ctrl,
                              uint8_t bomb_claw_penta_ctrl,
                              uint8_t bomb_claw_vision_ctrl,
                              uint8_t bomb_claw_scan_ctrl,
                              uint8_t bomb_claw_CCTV_ctrl)
//                              uint8_t supporting_wheel_ctrl)
{
	if (kb.bomb_claw_enable)//键盘模式下且爪子使能
	{
		kb_bomb_claw_catch_ctrl(bomb_claw_catch_ctrl);
		kb_bomb_claw_roll_ctrl(bomb_claw_roll_ctrl);
	  kb_bomb_claw_push_ctrl(bomb_claw_push_ctrl);
		kb_bomb_claw_triple_ctrl(bomb_claw_triple_ctrl);
		kb_bomb_claw_penta_ctrl(bomb_claw_penta_ctrl);
		kb_bomb_claw_vision_ctrl(bomb_claw_vision_ctrl);
		kb_bomb_claw_scan_ctrl(bomb_claw_scan_ctrl);
		kb_bomb_claw_CCTV_ctrl(bomb_claw_CCTV_ctrl);
//		kb_supporting_wheel_ctrl(supporting_wheel_ctrl);
	}
}

/*取弹模式键盘控制*/
bool flag_get_bomb = 0;
bool get_bomb_clear_record = 0;
static void kb_get_bomb_ctrl(uint8_t get_bomb_ctrl ,
	                           uint8_t get_bomb_clear_ctrl)
{	
	static bool flag_toggle = 1; 
	
	if(get_bomb_ctrl && flag_toggle)
	{
		flag_get_bomb = !flag_get_bomb;
		flag_toggle = 0;
	}
	else if(!get_bomb_ctrl) flag_toggle = 1;	
	
	if(flag_get_bomb)//根据flag判断要做什么
	{
		if(GET_BOMB_NO_TURN_AROUND_CTRL)//如果按的是SHIFT+CTRL+_ 掉头程序不要执行
		{
		  get_bomb.turn_around_enable = DISABLE;
		}
		
		global_task.climb_tube_task_enable = DISABLE;
		global_task.feed_bomb_task_enable = DISABLE;
		global_task.rescue_task_enable = DISABLE;
		
		get_bomb.get_bomb_enable = ENABLE;	
					
		if(get_bomb_clear_ctrl || get_bomb_clear_record)/*纠错重来的控制*/
		{		
			if(get_bomb_clear_ctrl)
			{
				flag_eject_push_Time_record = 1;
			}
			get_bomb_clear_record = 1;
			
		  /*下面这几句话放的位置有讲究*/
			get_bomb.ctrl_mode = SUSPEND_MODE;//强制抢占控制权
			BOMB_CLAW_LOOSE;//强制张开爪子
			BOMB_CLAW_ROLL_HALF;//强制翻转爪子至HALF	
			BOMB_CLAW_DONT_MOVE;//原地停住
			
			if((abs(bomb_claw.total_angle[L] - bomb_claw.target_half[L]) < 100) || 
				 (abs(bomb_claw.total_angle[R] - bomb_claw.target_half[R]) < 100))
      {				
				flag_push_aim_single_init = 1;
				flag_push_aim_multiple_init = 1;
				flag_push_aim_scan_init = 1;
				flag_scan_safe_delay = 0;
				
				get_bomb.get_pour_step = 0;//取弹步骤清零								
				get_bomb.flag_push_aim_single = HAVENT;  //还没完成推出对准模式											
				get_bomb.flag_push_aim_multiple = HAVENT;//还没完成推出对准模式	
				get_bomb.flag_push_aim_scan = HAVENT;    //还没完成推出对准模式	
			
				get_bomb.box_num = 0;		
				
				get_bomb.once_triple = 0;	
				flag_bomb_claw_triple = 0;
				
				get_bomb.once_penta = 0;							
	      flag_bomb_claw_penta = 0;    

				vision.enable = DISABLE;
//				vision.aim = HAVENT;
//				get_bomb.vision_assist = 0;	
				flag_bomb_claw_vision = 0; 			
				
				get_bomb.scan = 0;	
				get_bomb.flag_scan_failed = NO;
				get_bomb.scan_finished = NO;	
        optic_switch.value_record	= ENABLE;
				get_bomb.scan_delay_record = ENABLE;				
				flag_bomb_claw_scan = 0;	

				/*修正中心值Y轴*/	
				if(bomb_claw.target_half[Y] > bomb_claw.Y_target[M])
				{
					bomb_claw.from_claw_entire[Y_axis] = YES;
					bomb_claw.from_claw_zero[Y_axis] = NO;
				}	
				else if(bomb_claw.target_half[Y] < bomb_claw.Y_target[M])
				{
					bomb_claw.from_claw_entire[Y_axis] = NO;
					bomb_claw.from_claw_zero[Y_axis] = YES;
				}		
				bomb_claw.target_half[Y] = bomb_claw.Y_target[M];
				/*修正框架目标值*/ 
				if(lifter.target_bomb_bit[L] != lifter.target_bomb_bit_normal[L] || 
					 lifter.target_bomb_bit[R] != lifter.target_bomb_bit_normal[R])		
				{
					lifter.target_bomb_bit[L] = lifter.target_bomb_bit_normal[L];
					lifter.target_bomb_bit[R] = lifter.target_bomb_bit_normal[R];
					if((lifter.total_angle[L] > lifter.target_bomb_bit[L]) && 
						 (lifter.total_angle[R] < lifter.target_bomb_bit[R]))
					{
						from_lifter_zero = YES;
						from_lifter_bomb_entire = NO; 			
					}
					else if((lifter.total_angle[L] < lifter.target_bomb_bit[L]) && 
									(lifter.total_angle[R] > lifter.target_bomb_bit[R]))
					{
						from_lifter_zero = NO;
						from_lifter_bomb_entire = YES; 			
					}
				}	
				/*修正平台左右目标值*/
				if(bomb_claw.target_entire[Y] !=  bomb_claw.Y_target[R] || 
					 bomb_claw.target_zero[Y] != bomb_claw.Y_target[L])
				{
					bomb_claw.target_entire[Y] =  bomb_claw.Y_target[R];
					bomb_claw.target_zero[Y] = bomb_claw.Y_target[L];
				}
				
				flag_bomb_claw_push = 0;//单箱模式下默认不推出	
        flag_bomb_claw_record  = 0;			
				get_bomb.ctrl_mode = PUSH_AIM_SINGLE_MODE;
			}
			
		}
		else
		{
			if((get_bomb.flag_push_aim_single == FINISHED)||
				 (get_bomb.flag_push_aim_multiple == FINISHED)||
			   (get_bomb.flag_push_aim_scan == FINISHED))
			{
				get_bomb.ctrl_mode = GET_POUR_MODE;	
			}
			else 
			{				
				if(get_bomb.once_triple || get_bomb.once_penta) 
				{
					get_bomb.ctrl_mode = PUSH_AIM_MULTIPLE_MODE;	
				}
				else if(get_bomb.scan)
        {
				  get_bomb.ctrl_mode = PUSH_AIM_SCAN_MODE;
				}
//				else if(get_bomb.vision_assist)
//				{
//					get_bomb.ctrl_mode = VISION_ASSIST_MODE;
//				}
				else get_bomb.ctrl_mode = PUSH_AIM_SINGLE_MODE;	
			}			
		}
		
	}		
	else if(!flag_get_bomb)
	{
		memset(&global_task, ENABLE, sizeof(global_task_t));//开放其他任务
		get_bomb.ctrl_mode = STAND_BY_MODE; //执行完一遍case里的东西就失能掉get_bomb_enable
	}
}

/*取弹键盘操作集合*/
void keyboard_get_bomb_handler(void)
{
	if(kb.kb_enable)
	{	
		if(global_task.get_bomb_task_enable)
		{
			if(get_bomb.get_bomb_enable)
			{
				kb_bomb_claw_ctrl(BOMB_CLAW_CATCH_CTRL, 
													BOMB_CLAW_ROLL_CTRL, 
													BOMB_CLAW_PUSH_CTRL, 
													BOMB_CLAW_TRIPLE_CTRL,
													BOMB_CLAW_PENTA_CTRL,
													BOMB_CLAW_VISION_CTRL,
													BOMB_CLAW_SCAN_CTRL,
													BOMB_CLAW_CCTV_CTRL);
		//		                  SUPPORTING_WHEEL_CTRL);		
			}
			kb_get_bomb_ctrl(GET_BOMB_CTRL|GET_BOMB_NO_TURN_AROUND_CTRL, 
											 GET_BOMB_CLEAR_CTRL);			
		}
	} 
}

/*--------------------------------------救援键盘控制--------------------------------------------*/

/*钩子手动键盘控制*/
bool flag_trailer_hook = 0;
static void kb_trailer_hook_ctrl(uint8_t trailer_hook_ctrl)
{	
	static bool flag_toggle = 1;

	if(rescue.hook_enable)
	{
		if(trailer_hook_ctrl && flag_toggle)
		{
			flag_trailer_hook = !flag_trailer_hook;
			flag_toggle = 0;
		}
		else if(!trailer_hook_ctrl) flag_toggle = 1;	
		
		if(flag_trailer_hook)//根据flag判断要做什么
		{
			TRAILER_HOOK_PUSH;
		}		
		else TRAILER_HOOK_PULL;	
	}
}

/*拖车视角手动键盘控制*/
bool flag_rescue_sight = 0;
static void kb_rescue_sight_ctrl(uint8_t rescue_sight_ctrl)
{	
	static bool flag_toggle = 1;

	if(rescue.sight_enable)
	{
		if(rescue_sight_ctrl && flag_toggle)
		{
			flag_rescue_sight = !flag_rescue_sight;
			flag_toggle = 0;
		}
		else if(!rescue_sight_ctrl) flag_toggle = 1;		
	
		if(flag_rescue_sight)//根据flag判断要做什么
		{
			if(rescue.ctrl_mode == RELEASE_HOOK_MODE)
			{
			  GIM_SERVO_BACK;
			}
			else if(rescue.ctrl_mode == HOOK_CONFIRM_MODE)
			{
				GIM_SERVO_SIDE;
			}
		}		
		else 
		{
			if(rescue.ctrl_mode == RELEASE_HOOK_MODE)
			{
			  GIM_SERVO_SIDE;
			}
			else if(rescue.ctrl_mode == HOOK_CONFIRM_MODE)
			{
			  GIM_SERVO_ZERO;
			}
		}
	}
}

/*钩子确认键盘控制*/
static void kb_hook_confirm_ctrl(uint8_t hook_confirm_ctrl)
{	
	static bool flag_toggle = 1;
	
	if(hook_confirm_ctrl && flag_toggle && rescue.rescue_enable)//在进入拖车模式是才有用
	{
		rescue.flag_hook_confirm = !rescue.flag_hook_confirm;
		flag_toggle = 0;
	}
	else if(!hook_confirm_ctrl) flag_toggle = 1;	
}

/*拖车模式键盘控制*/
bool flag_rescue = 0;
bool rescue_lock = 0;
static void kb_rescue_ctrl(uint8_t rescue_ctrl)
{	
	static bool flag_toggle = 1;
	
	if(rescue_ctrl && flag_toggle)
	{
		flag_rescue = !flag_rescue;
		flag_toggle = 0;
	}
	else if(!rescue_ctrl) flag_toggle = 1;	
	
	if(flag_rescue)//根据flag判断要做什么
	{
		if(RESCUE_NO_TURN_AROUND_CTRL)//如果按的是SHIFT+CTRL+_ 掉头程序不要执行
		{
		  rescue.turn_around_enable = DISABLE;
		}
		
		global_task.climb_tube_task_enable = DISABLE;
		global_task.feed_bomb_task_enable = DISABLE;
		global_task.get_bomb_task_enable = DISABLE;
		
		rescue.rescue_enable = ENABLE;
		
		if(rescue.flag_hook_confirm)
		{
			rescue.ctrl_mode = HOOK_CONFIRM_MODE;	
			rescue_lock = ENABLE;
		}
		else 
		{
		  rescue.ctrl_mode = RELEASE_HOOK_MODE;
			rescue_lock = DISABLE;
		}
	
	}		
	else if(!flag_rescue && rescue_lock)//在确认状态下 失能ctrl+X按键		
	{
		flag_rescue = !flag_rescue;
	}
	else if(!flag_rescue && !rescue_lock)
	{
		memset(&global_task, ENABLE, sizeof(global_task_t));//开放其他任务
		rescue.ctrl_mode = RETRACT_HOOK_MODE; //执行完一遍case里的东西就失能掉get_bomb_enable
	}
}

/*拖车键盘操作集合*/
void keyboard_rescue_handler(void)
{	
	if(kb.kb_enable)
	{		
		if(global_task.rescue_task_enable)
		{
			if(rescue.rescue_enable == ENABLE)
			{
				kb_trailer_hook_ctrl(TRAILER_HOOK_CTRL);
				kb_rescue_sight_ctrl(RESCUE_SIGHT_CTRL);
				kb_hook_confirm_ctrl(HOOK_CONFIRM_CTRL);		
			}	
			kb_rescue_ctrl(RESCUE_CTRL|RESCUE_NO_TURN_AROUND_CTRL);		
		}			
	}
}

/*--------------------------------------补弹键盘控制--------------------------------------------*/

/*弹仓开合键盘控制*/
bool flag_magazine_exit = 0;
static void kb_magazine_exit_ctrl(uint8_t magazine_exit_ctrl)
{	
	static bool flag_toggle = 1;

	if(feed_bomb.exit_enable)
	{
		if(magazine_exit_ctrl && flag_toggle)
		{
			flag_magazine_exit = !flag_magazine_exit;
			flag_toggle = 0;
		}
		else if(!magazine_exit_ctrl) flag_toggle = 1;	
		
		if(flag_magazine_exit)//根据flag判断要做什么
		{
			MAGAZINE_OPEN;
		}		
		else MAGAZINE_CLOSE;	
	}
}
/*弹仓抬升键盘控制*/
bool flag_magazine_lifter = 0;
static void kb_magazine_lifter_ctrl(uint8_t magazine_lifter_ctrl)
{	
	static bool flag_toggle = 1;

	if(feed_bomb.lifter_enable)
	{
		if(magazine_lifter_ctrl && flag_toggle)
		{
			flag_magazine_lifter = !flag_magazine_lifter;
			flag_toggle = 0;
		}
		else if(!magazine_lifter_ctrl) flag_toggle = 1;	
		
		if(flag_magazine_lifter)//根据flag判断要做什么
		{	
//			servo_feed_bomb_lift_up_delay_record = ENABLE;
			
//			if(servo_feed_bomb_lift_down_delay_record)//延时一小段时间让舵机转回来先
//			{					
//				servo_feed_bomb_lift_down_delay_time = millis();
//				servo_feed_bomb_lift_down_delay_record = 0;
//				GIM_SERVO_BACK;
//				flag_feed_bomb_sight = 0;
//				feed_bomb.sight_enable = DISABLE;
//			}
//			else if(millis() - servo_feed_bomb_lift_down_delay_time > 0)
//			{
				lifter.ctrl_mode = LIFTER_ZERO;
						
//				if((abs(lifter.target_zero[L] - lifter.total_angle[L]) < 100) || 
//			    (abs(lifter.target_zero[R] - lifter.total_angle[R]) < 100))//抬升到目标角度
//				{
//			  	feed_bomb.sight_enable = ENABLE;
//				}
				
//			}
		}		
		else 
		{
//			servo_feed_bomb_lift_down_delay_record = ENABLE;
			
//			if(servo_feed_bomb_lift_up_delay_record)//延时一小段时间让舵机转回来先
//			{					
//				servo_feed_bomb_lift_up_delay_time = millis();
//				servo_feed_bomb_lift_up_delay_record = 0;
//				GIM_SERVO_BACK;
//				flag_feed_bomb_sight = 0;
//				feed_bomb.sight_enable = DISABLE;
//			}
//			else if(millis() - servo_feed_bomb_lift_up_delay_time > 400)
//			{
				lifter.ctrl_mode = FEED_LIFTER_ENTIRE;
				
//			  if((abs(lifter.target_feed_entire[L] - lifter.total_angle[L]) < 100) || 
//			     (abs(lifter.target_feed_entire[R] - lifter.total_angle[R]) < 100))//抬升到目标角度
//				{
//					feed_bomb.sight_enable = ENABLE;
//				}
//			}
		
		}
	}
}

/*补弹是否视觉控制*/
bool flag_feed_bomb_vision = 0;
static void kb_feed_bomb_vision_ctrl(uint8_t feed_bomb_vision_ctrl) 
{
	static bool flag_toggle = 1;
//	if(feed_bomb.feed_bomb_enable)
//	{
		if(feed_bomb.vision_enable)
		{
			if(feed_bomb_vision_ctrl && flag_toggle)
			{
				flag_feed_bomb_vision = !flag_feed_bomb_vision;
				flag_toggle = 0;
			}
			else if(!feed_bomb_vision_ctrl) flag_toggle = 1;

			if(flag_feed_bomb_vision)//根据flag判断要做什么
			{
				vision.enable = DISABLE;
			}		
			else vision.enable = ENABLE;//默认打开视觉
		}	
//	}
}

/*补弹视角手动控制*/
bool flag_feed_bomb_sight = 0;
static void kb_feed_bomb_sight_ctrl(uint8_t feed_bomb_sight_ctrl) 
{
	static bool flag_toggle = 1;
	
	if(feed_bomb.sight_enable)
	{
		if(feed_bomb_sight_ctrl && flag_toggle)
		{
			flag_feed_bomb_sight = !flag_feed_bomb_sight;
			flag_toggle = 0;
		}
		else if(!feed_bomb_sight_ctrl) flag_toggle = 1;

		if(flag_feed_bomb_sight)//根据flag判断要做什么
		{
			GIM_SERVO_BACK;
		}		
		else GIM_SERVO_SIDE;
	}
}

/*补弹模式键盘控制*/
bool flag_feed_bomb = 0;
static void kb_feed_bomb_ctrl(uint8_t feed_bomb_ctrl)
{	
	static bool flag_toggle = 1;
	
	if(feed_bomb_ctrl && flag_toggle)
	{
		flag_feed_bomb = !flag_feed_bomb;
		flag_toggle = 0;
	}
	else if(!feed_bomb_ctrl) flag_toggle = 1;	
	
	if(flag_feed_bomb)//根据flag判断要做什么
	{
	  if(FEED_BOMB_NO_TURN_AROUND_CTRL)//如果按的是SHIFT+CTRL+_ 掉头程序不要执行
		{
		  feed_bomb.turn_around_enable = DISABLE;
		}
		
		global_task.climb_tube_task_enable = DISABLE;
		global_task.get_bomb_task_enable = DISABLE;
		global_task.rescue_task_enable = DISABLE;
		
    feed_bomb.feed_bomb_enable = ENABLE;	

    feed_bomb.ctrl_mode = AIM_FEED_MODE;
	}		
	else 
	{
		memset(&global_task, ENABLE, sizeof(global_task_t));//开放其他任务
		feed_bomb.ctrl_mode = QUIT_FEED_MODE; //执行完一遍case里的东西就失能掉
	}
}

/*补弹键盘操作集合*/
void keyboard_feed_bomb_handler(void)
{	
	if(kb.kb_enable)
	{		
		if(global_task.feed_bomb_task_enable)
		{
			if(feed_bomb.feed_bomb_enable)
			{
				kb_magazine_exit_ctrl(MAGAZINE_EXIT_CTRL);
				kb_magazine_lifter_ctrl(MAGAZINE_LIFTER_CTRL);
				kb_feed_bomb_sight_ctrl(FEED_BOMB_SIGHT_CTRL);
				kb_feed_bomb_vision_ctrl(FEED_BOMB_VISION_CTRL);		
			}
			kb_feed_bomb_ctrl(FEED_BOMB_CTRL|FEED_BOMB_NO_TURN_AROUND_CTRL);		
		}
	}
}
