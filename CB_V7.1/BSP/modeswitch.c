#include "modeswitch.h"

SYSTEM_MODE system_mode;


void mode_switch_task()//拨杆S1的模式切换
{
	static bool once_task_init = ENABLE;	
	/*系统模式选择*/
	switch(RC_Ctl.rc.s1)
  {
		case RC_SW_UP:		
		{
			system_mode = RC_MACHINE_MODE;
		}break;
		
		case RC_SW_MID:		
		{
			system_mode = RC_GYRO_MODE;
		}break;
		
		case RC_SW_DOWN:	//键盘模式
		{
			system_mode = KB_MODE;
			
		}break;
		
		default:					
		{
	    system_mode = SAFETY_MODE;
		}break;
	}
			
	switch(RC_Ctl.rc.s2)//主要用于赛前一波骚操作
  {
		case RC_SW_UP:		
		{
			if(check.step_run_once && check.next_step_enable)
			{
				check.step_run_once = DISABLE;
				check.check_step ++;
				if(check.check_step > 6) check.check_step = 1;
			}
			check.check_enable = ENABLE;
			climb_tube.climb_enable = DISABLE;
			get_bomb.get_bomb_enable = DISABLE;
			rescue.rescue_enable = DISABLE;
			feed_bomb.feed_bomb_enable = DISABLE;			

		}break;
		
		case RC_SW_MID:		
		{
      check.step_run_once = ENABLE;
			check.check_enable = ENABLE;
			check.check_CCTV_enable = ENABLE;
			
			climb_tube.climb_enable = DISABLE;
			get_bomb.get_bomb_enable = DISABLE;
			rescue.rescue_enable = DISABLE;
			feed_bomb.feed_bomb_enable = DISABLE;			
		  once_task_init = ENABLE;
		}break;
		
		case RC_SW_DOWN:
		{
			check.check_enable = DISABLE;
			if(once_task_init)
			{
				climb_tube.climb_enable = ENABLE;
				get_bomb.get_bomb_enable = ENABLE;	
				rescue.rescue_enable = ENABLE;
				feed_bomb.feed_bomb_enable = ENABLE;	

				once_task_init = DISABLE;
			}	
		}break;
		
		default:					
		{
	    system_mode = SAFETY_MODE;
		}break;
	}
	
	/*系统模式都要干些什么*/
	switch(system_mode)
	{
	  case RC_MACHINE_MODE://遥控机械模式
		{
		  kb.kb_enable = DISABLE; //失能键盘
			rc.rc_enable = ENABLE;  //使能遥控器
			
			chassis.ctrl_mode = CHASSIS_SEPARATE;
		}break;
		
		case RC_GYRO_MODE://遥控陀螺仪模式
		{
			kb.kb_enable = DISABLE; //失能键盘
			rc.rc_enable = ENABLE;  //使能遥控器	
			
			chassis.ctrl_mode = CHASSIS_FOLLOW;
			flag_gyro_blow_up = 0;//标记陀螺仪没炸
		}break;		
		
		case KB_MODE://键盘模式
		{			
			kb.kb_enable = ENABLE; //使能键盘
			rc.rc_enable = DISABLE;//失能遥控器

		}break;
		
		case SAFETY_MODE://安全模式
		{		
			ducted.ctrl_mode = DUCTED_ALL_OFF;	      //两个涵道都关掉
			assist_wheel.ctrl_mode = WHEEL_RELAX;     //摩擦轮无输出			
			optic_switch.ctrl_mode = DETECT_ENABLE;   //正常强制置1
			climb_tube.climb_enable = DISABLE;        //失能爬杆(把它里面所有的控制都不干)
			get_bomb.get_bomb_enable = DISABLE;
			rescue.rescue_enable = DISABLE;
			feed_bomb.feed_bomb_enable = DISABLE;			
		
			rc.rc_enable = DISABLE;                   //失能遥控器
			kb.kb_enable = DISABLE;                   //失能键盘		
			kb.tube_claw_enable = DISABLE;            //爪子不可控
			
			check.check_enable = DISABLE;             //系统检查失能
			
		}break;
		
		default:		
		break;
	
	}
	

	
	
	
	
}

