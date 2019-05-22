#include "sys_check.h"

check_t check;

void sys_check_init(void)
{
	memset(&check, 0, sizeof(check_t));
	check.next_step_enable = ENABLE;
//	check.step_run_once = ENABLE;
	
	check.motor_check_time_record = ENABLE;
	check.motor_relax_time_record = ENABLE;
}

static void sensor_check(void)
{
	if(optic_switch.value[S]) 
	{
		LED_RED_OFF;
	  LED_BLUE_OFF;
	}
	else 
	{
		LED_RED_ON;
		LED_BLUE_ON;	
	}
	
	if(optic_switch.value[M])
	{
	  LED_ORANGE_ON;
	  LED_GREEN_ON;
	}
	else 
	{
		LED_ORANGE_OFF;
		LED_GREEN_OFF;	
	}
}

static void actuator_check(void) //一波操作猛如虎
{
	kb.tube_claw_enable = DISABLE;           //抱杆爪子失能
	kb.bomb_claw_enable = DISABLE;           //取弹爪子失能
	
	switch(check.check_step)
	{
		case 2:
		{
			check.next_step_enable = DISABLE;
			lifter.ctrl_mode = TUBE_LIFTER_ENTIRE;	    //框架抬升  
			CCTV_FEED_RESCUE;
			
			if((abs(lifter.target_tube_entire[L] - lifter.total_angle[L]) < 100) || 
			   (abs(lifter.target_tube_entire[R] - lifter.total_angle[R]) < 100))    //抬升到目标角度
			{
				GIM_SERVO_BACK;
				TUBE_CLAW_CATCH;                          //爬杆爪子强制合上		
				MAGAZINE_OPEN;                           	//弹仓盖打开
			  trailer.ctrl_mode = TRAILER_PUSH;           //拖车推出  				
//				if(abs(trailer.total_angle - trailer.target_entire) < 100) 
//				{
					TRAILER_HOOK_RELEASE; 
					TRAILER_HOOK_PUSH; 
					check.next_step_enable = ENABLE;
//				}					
			}
		}break;
		
		case 3:
		{
			check.next_step_enable = DISABLE;
	    lifter.ctrl_mode = LIFTER_ZERO;            //框架回标定的位置
			TUBE_CLAW_LOOSE;                           //爬杆爪子强制张开	
			MAGAZINE_CLOSE;                            //弹仓盖合上
			GIM_SERVO_ZERO;	
			trailer.ctrl_mode = TRAILER_PULL;                                //拖车收回		
      TRAILER_HOOK_CAPTURE;                                               //钩子气缸回收		
			TRAILER_HOOK_PULL;                                               //钩子气缸回收		
			CCTV_CLIMB_TUBE;
			if((abs(lifter.target_zero[L] - lifter.total_angle[L]) < 100) || 
			   (abs(lifter.target_zero[R] - lifter.total_angle[R]) < 100))    //抬升到目标角度
			{
				check.next_step_enable = ENABLE;
				
			}
		}break;
		
		case 4:
		{			
			check.next_step_enable = DISABLE;
			CCTV_GET_BOMB;
			if((abs(lifter.target_bomb_entire[L] - lifter.total_angle[L]) < 100) || 
			   (abs(lifter.target_bomb_entire[R] - lifter.total_angle[R]) < 100))    //抬升到目标角度
			{
		
				if((abs(bomb_claw.target_half[L] - bomb_claw.total_angle[L]) < 100) || 
					 (abs(bomb_claw.target_half[R] - bomb_claw.total_angle[R]) < 100))   //取弹爪子翻转到了位置
				{
					ducted.ctrl_mode = DUCTED_ALL_RUN;      //开涵道	
					BOMB_CLAW_MOVE_RIGHT;
					if(abs(bomb_claw.target_entire[Y] - bomb_claw.total_angle[Y]) < 100) //取弹爪子挪到了位置
					{
						BOMB_CLAW_PUSH;                         //推出取弹爪子	
            BOMB_CLAW_LOOSE;                        //取弹爪子开					
					  BOMB_EJECT_PUSH;				                //取弹弹射开							
						ducted.ctrl_mode = DUCTED_ALL_OFF;      //关涵道		
			    	check.next_step_enable = ENABLE;						
					}
				}	
				else BOMB_CLAW_ROLL_HALF; 
			}
			else lifter.ctrl_mode = BOMB_LIFTER_ENTIRE;	 //框架抬升		
		}break;
		
		case 5:
		{
			check.next_step_enable = DISABLE;
			BOMB_CLAW_MOVE_LEFT;
			if(abs(bomb_claw.target_zero[Y] - bomb_claw.total_angle[Y]) < 100) //取弹爪子挪到了位置
			{ 
				BOMB_CLAW_ROLL_ZERO;                     //取弹爪子翻转归零	
//				if((abs(bomb_claw.target_zero[L] - bomb_claw.total_angle[L]) < 100) || 
//					 (abs(bomb_claw.target_zero[R] - bomb_claw.total_angle[R]) < 100))   //取弹爪子翻转到了位置
				if(bomb_claw.zero_correct == FINISHED)
				{
					BOMB_CLAW_CATCH;                         //取弹爪子抓
					BOMB_EJECT_PULL;				                 //取弹弹射收回		
					check.next_step_enable = ENABLE;	
				}					
			}
		}break;
		
		case 6:
		{
			check.next_step_enable = DISABLE;
			CCTV_CLIMB_TUBE;
			BOMB_CLAW_MOVE_MID;
			BOMB_CLAW_ROLL_ENTIRE; 
			BOMB_CLAW_PULL;                          //收回取弹爪子
      lifter.ctrl_mode = LIFTER_ZERO;            //框架回标定的位置	
			if((abs(lifter.target_zero[L] - lifter.total_angle[L]) < 100) || 
			   (abs(lifter.target_zero[R] - lifter.total_angle[R]) < 100))    //抬升到目标角度
			{
				check.next_step_enable = ENABLE;
			}
		}break;
		
		default:break;	
	}
}
/*电机掉线检查*/
void motor_offline_check(void)
{
	CAN1_motor_offline_check();
	CAN2_motor_offline_check();
}
/*电机堵转检查*/
void motor_stall_check(void)
{
/*检测到20s内连续堵转次数超过8次*/
	if(check.motor_check_time_record)//每十秒 检查一下堵转次数 清一次堵转次数
	{	
    trailer.stall_times = 0;
		
		memset(lifter.stall_times, 0, sizeof(lifter.stall_times));
		
		memset(bomb_claw.stall_times, 0, sizeof(bomb_claw.stall_times));
		
		check.motor_check_time = millis();
		check.motor_check_time_record = DISABLE;
	}
	else if(millis() - check.motor_check_time > 6000)//6s内有两次堵转
	{
		check.motor_check_time_record = ENABLE;		
		
		if((trailer.stall_times > 1)      ||
			 (lifter.stall_times[0] > 1)    ||
		   (lifter.stall_times[1] > 1)    ||
			 (bomb_claw.stall_times[0] > 1) ||
		   (bomb_claw.stall_times[1] > 1) ||
		   (bomb_claw.stall_times[2] > 1) ||
		   (bomb_claw.stall_times[3] > 1))
		{
			check.motor_serious_stall = YES;
		}
		else check.motor_serious_stall = NO;
	}
	
//	else if(trailer.stall_times > 4)
//	{
//	  check.motor_serious_stall = YES;
//	}
//	else 
//	{
//		check.motor_serious_stall = NO;
//	}
	
}

void sys_check(void)
{
	if(check.check_enable)
	{
		sensor_check();//传感器检查
	  actuator_check();//执行元件检查
	}
	else if(check.step_run_once) //S2拨杆下拨状态复原
	{
		CCTV_CLIMB_TUBE;
		MAGAZINE_CLOSE;                          //弹仓盖合上
		GIM_SERVO_ZERO;	
		TUBE_CLAW_LOOSE;                         //爬杆爪子强制张开	
		
		BOMB_CLAW_MOVE_MID;
	  BOMB_CLAW_ROLL_ENTIRE;        
    BOMB_CLAW_PULL;                          //收回取弹爪子	
		BOMB_CLAW_CATCH;                         //取弹爪子抓
		BOMB_EJECT_PULL;				                 //取弹弹射收回				
		trailer.ctrl_mode = TRAILER_PULL;        //拖车收回		
    TRAILER_HOOK_CAPTURE;                       //钩子气缸回收		
		TRAILER_HOOK_PULL;                       //钩子气缸回收		
    ducted.ctrl_mode = DUCTED_ALL_OFF;       //关涵道			
    lifter.ctrl_mode = LIFTER_ZERO;          //框架回标定的位置	
		
    climb_tube_param_init();//爬杆参数初始化		
		
		memset(&check, 0, sizeof(check_t));
		check.next_step_enable = ENABLE;
//		check.step_run_once = DISABLE;//只跑一次
//		check.check_step = 0;
	}
	
  motor_offline_check();//运行！
	motor_stall_check();//运行！
}
