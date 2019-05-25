#include "keyboard_ctrl.h"

/* get information task period time (ms) */
#define INFO_GET_PERIOD 1 //放在哪个时间戳

/* key acceleration time */
#define KEY_ACC_TIME     500  //ms

/* turn around time */
#define TUR_ARO_TIME     1500  //ms 掉头时间

kb_ctrl_t kb;

/*底盘速度控制*/
static void move_spd_ctrl(uint8_t fast, uint8_t slow)
{
	if (fast)
  {
    kb.move_speed = FAST_SPEED;
    kb.x_spd_limit = CHASSIS_KB_MAX_SPEED_X;
    kb.y_spd_limit = CHASSIS_KB_MAX_SPEED_Y;
		kb.w_spd_limit = CHASSIS_KB_MAX_SPEED_R;
  }
  else if (slow)
  {
    kb.move_speed = SLOW_SPEED;
    kb.x_spd_limit = 0.1f * CHASSIS_KB_MAX_SPEED_X;
    kb.y_spd_limit = 0.1f * CHASSIS_KB_MAX_SPEED_Y;
		kb.w_spd_limit = 0.3f * CHASSIS_KB_MAX_SPEED_R;
  }
  else
  {
    kb.move_speed = NORMAL_SPEED;
    kb.x_spd_limit =  CHASSIS_KB_MAX_SPEED_X;
    kb.y_spd_limit =  CHASSIS_KB_MAX_SPEED_Y;
		kb.w_spd_limit =  CHASSIS_KB_MAX_SPEED_R;
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
	
	/*键盘不加机械模式的话可以把下面的注释掉*/
	if(chassis.ctrl_mode == CHASSIS_SEPARATE)//底盘是机械模式
	{
		if(chassis.ass_mode_enable)//车尾模式
		{
		  kb.vw = -rc.mouse.x * 10;		
		}
		else //车头模式
		{
			kb.vw = rc.mouse.x * 10;		
		}
    VAL_LIMIT(kb.vw, -kb.w_spd_limit, kb.w_spd_limit);
	}
	else if(chassis.ctrl_mode == CHASSIS_FOLLOW)
	{
	
	}
}
/*底盘掉头控制*/
static void kb_turn_around_ctrl(uint8_t turn_around)
{
	static bool flag_around;
	
	if(turn_around)
	{
		flag_around = 1;
		chassis.position_temp = imu.yaw;
		ramp_init(&turn_around_ramp, TUR_ARO_TIME/INFO_GET_PERIOD);		
	}
	else if(!turn_around && flag_around) 
	{
		chassis.position_ref = chassis.position_temp + 180 * ramp_calc(&turn_around_ramp);    
		if(ramp_calc(&turn_around_ramp) == 1) flag_around = 0;
	}
}
/*底盘键盘操作集合*/
void keyboard_chassis_handler(void)
{ 

	if (kb.kb_enable)
  { /*掉头*/
		if(chassis.turn_around_disable)
		{
			kb_turn_around_ctrl(TURN_AROUND_DISABLE);//失能掉头
		}
		else kb_turn_around_ctrl(TURN_AROUND);//掉头可控
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
  }
  else
  {
    kb.vx = 0;
    kb.vy = 0;
		kb.vw = 0;
  }
}


///*--------------------------------------------------------------------------------*/

///*爬杆爪子抓取控制*/
//bool flag_tube_claw_catch = 0;
//static void kb_tube_claw_catch_ctrl(uint8_t tube_claw_catch_ctrl) 
//{
//	static bool flag_toggle = 1;
//	
//	if(tube_claw_catch_ctrl && flag_toggle)
//	{
//		flag_tube_claw_catch = !flag_tube_claw_catch;
//		flag_toggle = 0;
//	}
//	else if(!tube_claw_catch_ctrl) flag_toggle = 1;	
//	
//	if(flag_tube_claw_catch)//根据flag判断要做什么
//	{
//		TUBE_CLAW_CATCH;
//	}		
//	else TUBE_CLAW_LOOSE;
//}
///*爬杆爪子控制集合*/
//static void kb_tube_claw_ctrl(uint8_t tube_claw_catch_ctrl)
//{
//	if (kb.tube_claw_enable)//键盘模式下且爪子使能
//	{
//		kb_tube_claw_catch_ctrl(tube_claw_catch_ctrl);
//	}
//}

///*爬杆动作控制集合*/
//static void kb_climb_tube_ctrl(uint8_t lift_up, uint8_t run_plain)
//{	
//	if(lift_up && climb_tube.location == ON_GROUND)//在岛下抬升爬杆框架
//	{
//		climb_tube.climb_enable = ENABLE;
//		climb_tube.ctrl_mode = CLIMEB_UP_MODE;
//	}	
//  else if(lift_up && climb_tube.location == ON_ISLAND)//在岛上抬升爬杆框架
//	{
//		climb_tube.climb_enable = ENABLE;
//	  climb_tube.ctrl_mode = CLIMEB_DOWN_MODE;	
//	}
//	else if(run_plain)
//	{
//		climb_tube.ctrl_mode = PLAIN_MODE;	//执行完一遍case里的东西就失能掉climb_enable
//	}	
//}
///*爬杆键盘操作集合*/
//void keyboard_climb_tube_handler(void)
//{
//	if(kb.kb_enable)
//	{
//		kb_tube_claw_ctrl(TUBE_CLAW_CATCH_CTRL);
//	  kb_climb_tube_ctrl(LIFT_UP, RUN_PLAIN);
//	} 
//}


///*--------------------------------------------------------------------------------*/

///*取弹爪子夹取控制*/
//bool flag_bomb_claw_catch = 0;
//static void kb_bomb_claw_catch_ctrl(uint8_t bomb_claw_catch_ctrl) 
//{
//	static bool flag_toggle = 1;
//	
//	if(bomb_claw_catch_ctrl && flag_toggle)
//	{
//		flag_bomb_claw_catch = !flag_bomb_claw_catch;
//		flag_toggle = 0;
//	}
//	else if(!bomb_claw_catch_ctrl) flag_toggle = 1;	
//	
//	if(flag_bomb_claw_catch)//根据flag判断要做什么
//	{
//		BOMB_CLAW_CATCH;
//	}		
//	else BOMB_CLAW_LOOSE;
//}
///*取弹爪子翻转控制*/
//bool flag_bomb_claw_roll = 0;
//static void kb_bomb_claw_roll_ctrl(uint8_t bomb_claw_roll_ctrl) 
//{
//	static bool flag_toggle = 1;
//	
//	if(bomb_claw_roll_ctrl && flag_toggle)
//	{
//		flag_bomb_claw_roll = !flag_bomb_claw_roll;
//		flag_toggle = 0;
//	}
//	else if(!bomb_claw_roll_ctrl) flag_toggle = 1;

//	if(flag_bomb_claw_roll)//根据flag判断要做什么
//	{
//		BOMB_CLAW_ROLL_ZERO;
//	}		
//	else BOMB_CLAW_ROLL_HALF;
//}

///*取弹爪子推出控制*/
//bool flag_bomb_claw_push = 0;
//static void kb_bomb_claw_push_ctrl(uint8_t bomb_claw_push_ctrl) 
//{
//	static bool flag_toggle = 1;
//	
//	if(bomb_claw_push_ctrl && flag_toggle)
//	{
//		flag_bomb_claw_push = !flag_bomb_claw_push;
//		flag_toggle = 0;
//	}
//	else if(!bomb_claw_push_ctrl) flag_toggle = 1;

//	if(flag_bomb_claw_push)//根据flag判断要做什么
//	{
//		BOMB_CLAW_PUSH;
//	}		
//	else BOMB_CLAW_PULL;
//}

///*取弹爪子是否三箱控制*/
//bool flag_bomb_claw_triple = 0;
//static void kb_bomb_claw_triple_ctrl(uint8_t bomb_claw_triple_ctrl) 
//{
//	static bool flag_toggle = 1;
//	
//	if(bomb_claw_triple_ctrl && flag_toggle)
//	{
//		flag_bomb_claw_triple = !flag_bomb_claw_triple;
//		flag_toggle = 0;
//	}
//	else if(!bomb_claw_triple_ctrl) flag_toggle = 1;

//	if(flag_bomb_claw_triple)//根据flag判断要做什么
//	{
//		get_bomb.once_triple = ENABLE;
//	}		
//	else 
//	{
//		get_bomb.once_triple = DISABLE;
//	}
//}

///*取弹爪子动作控制集合*/
//static void kb_bomb_claw_ctrl(uint8_t bomb_claw_catch_ctrl, 
//											 uint8_t bomb_claw_roll_ctrl, 
//											 uint8_t bomb_claw_push_ctrl, 
//											 uint8_t bomb_claw_triple_ctrl)
//{
//	if (kb.bomb_claw_enable)//键盘模式下且爪子使能
//	{
//		kb_bomb_claw_catch_ctrl(bomb_claw_catch_ctrl);
//		kb_bomb_claw_roll_ctrl(bomb_claw_roll_ctrl);
//	  kb_bomb_claw_push_ctrl(bomb_claw_push_ctrl);
//		kb_bomb_claw_triple_ctrl(bomb_claw_triple_ctrl);
//	}
//}

///*取弹模式键盘控制*/
//bool flag_get_bomb = 0;
//static void kb_get_bomb_ctrl(uint8_t get_bomb_ctrl)
//{	
//	static bool flag_toggle = 1;
//	
//	if(get_bomb_ctrl && flag_toggle)
//	{
//		flag_get_bomb = !flag_get_bomb;
//		flag_toggle = 0;
//	}
//	else if(!get_bomb_ctrl) flag_toggle = 1;	
//	
//	if(flag_get_bomb)//根据flag判断要做什么
//	{
//    get_bomb.get_bomb_enable = ENABLE;	
//		
//		if((get_bomb.flag_push_aim_single == FINISHED)||(get_bomb.flag_push_aim_triple == FINISHED))
//		{
//			get_bomb.ctrl_mode = GET_POUR_MODE;	
//		}
//		else 
//		{
//			if(get_bomb.once_triple) 
//			{
//				get_bomb.ctrl_mode = PUSH_AIM_TRIPLE_MODE;	
//			}
//			else get_bomb.ctrl_mode = PUSH_AIM_SINGLE_MODE;	
//		}	
//	}		
//	else if(!flag_get_bomb)
//	{
//		get_bomb.ctrl_mode = STAND_BY_MODE; //执行完一遍case里的东西就失能掉get_bomb_enable
//	}
//}

///*取弹键盘操作集合*/
//void keyboard_get_bomb_handler(void)
//{
//	if(kb.kb_enable)
//	{
//		kb_bomb_claw_ctrl(BOMB_CLAW_CATCH_CTRL, 
//											BOMB_CLAW_ROLL_CTRL, 
//											BOMB_CLAW_PUSH_CTRL, 
//											BOMB_CLAW_TRIPLE_CTRL);
//	  kb_get_bomb_ctrl(GET_BOMB_CTRL);
//	} 
//}


///*--------------------------------------------------------------------------------*/

///*钩子手动键盘控制*/
//bool flag_trailer_hook = 0;
//static void kb_trailer_hook_ctrl(uint8_t trailer_hook_ctrl)
//{	
//	static bool flag_toggle = 1;

//	if(rescue.hook_enable)
//	{
//		if(trailer_hook_ctrl && flag_toggle)
//		{
//			flag_trailer_hook = !flag_trailer_hook;
//			flag_toggle = 0;
//		}
//		else if(!trailer_hook_ctrl) flag_toggle = 1;	
//		
//		if(flag_trailer_hook)//根据flag判断要做什么
//		{
//			TRAILER_HOOK_PUSH;
//		}		
//		else TRAILER_HOOK_PULL;	
//	}
//}

///*钩子确认键盘控制*/
//static void kb_hook_confirm_ctrl(uint8_t hook_confirm_ctrl)
//{	
//	static bool flag_toggle = 1;
//	
//	if(hook_confirm_ctrl && flag_toggle && rescue.rescue_enable)//在进入拖车模式才是有用
//	{
//		rescue.flag_hook_confirm = !rescue.flag_hook_confirm;
//		flag_toggle = 0;
//	}
//	else if(!hook_confirm_ctrl) flag_toggle = 1;	
//}

///*拖车模式键盘控制*/
//bool flag_rescue = 0;
//static void kb_rescue_ctrl(uint8_t rescue_ctrl)
//{	
//	static bool flag_toggle = 1;
//	
//	if(rescue_ctrl && flag_toggle)
//	{
//		flag_rescue = !flag_rescue;
//		flag_toggle = 0;
//	}
//	else if(!rescue_ctrl) flag_toggle = 1;	
//	
//	if(flag_rescue)//根据flag判断要做什么
//	{
//		rescue.rescue_enable = ENABLE;
//		
//		if(rescue.flag_hook_confirm)
//		{
//			rescue.ctrl_mode = HOOK_CONFIRM_MODE;		
//		}
//		else rescue.ctrl_mode = RELEASE_HOOK_MODE;
//	
//	}		
//	else if(!flag_rescue)
//	{
//		rescue.ctrl_mode = RETRACT_HOOK_MODE; //执行完一遍case里的东西就失能掉get_bomb_enable
//	}
//}

///*拖车键盘操作集合*/
//void keyboard_rescue_handler(void)
//{	
//	if(kb.kb_enable)
//	{		
//		kb_trailer_hook_ctrl(TRAILER_HOOK_CTRL);
//		kb_hook_confirm_ctrl(HOOK_CONFIRM_CTRL);
//		kb_rescue_ctrl(RESCUE_CTRL);
//	}
//}

///*--------------------------------------------------------------------------------*/

///*弹仓开合键盘控制*/
//bool flag_magazine = 0;
//static void kb_magazine_ctrl(uint8_t magazine_ctrl)
//{	
//	static bool flag_toggle = 1;

//	if(feed_bomb.exit_enable)
//	{
//		if(magazine_ctrl && flag_toggle)
//		{
//			flag_magazine = !flag_magazine;
//			flag_toggle = 0;
//		}
//		else if(!magazine_ctrl) flag_toggle = 1;	
//		
//		if(flag_magazine)//根据flag判断要做什么
//		{
//			MAGAZINE_OPEN;
//		}		
//		else MAGAZINE_CLOSE;	
//	}
//}


///*补弹模式键盘控制*/
//bool flag_feed_bomb = 0;
//static void kb_feed_bomb_ctrl(uint8_t feed_bomb_ctrl)
//{	
//	static bool flag_toggle = 1;
//	
//	if(feed_bomb_ctrl && flag_toggle)
//	{
//		flag_feed_bomb = !flag_feed_bomb;
//		flag_toggle = 0;
//	}
//	else if(!feed_bomb_ctrl) flag_toggle = 1;	
//	
//	if(flag_feed_bomb)//根据flag判断要做什么
//	{
//    feed_bomb.feed_bomb_enable = ENABLE;	

//    feed_bomb.ctrl_mode = AIM_FEED_MODE;
//	}		
//	else feed_bomb.ctrl_mode = QUIT_FEED_MODE; //执行完一遍case里的东西就失能掉get_bomb_enable
//}

///*补弹键盘操作集合*/
//void keyboard_feed_bomb_handler(void)
//{	
//	if(kb.kb_enable)
//	{		
//		kb_magazine_ctrl(MAGAZINE_CTRL);
//		kb_feed_bomb_ctrl(FEED_BOMB_CTRL);
//	}
//}
