#include "feed_bomb.h"

feed_bomb_t feed_bomb;

void feed_bomb_param_init(void)
{
  feed_bomb.turn_around_record = ENABLE;
	feed_bomb.turn_around_enable = ENABLE;
	
	feed_bomb.exit_enable = DISABLE;
	MAGAZINE_CLOSE;
}

void feed_bomb_task(void)
{
	keyboard_feed_bomb_handler();//keyboard	
		
	if(feed_bomb.feed_bomb_enable == ENABLE)	
	{
		switch(feed_bomb.ctrl_mode)
		{	
			case QUIT_FEED_MODE://退出喂弹模式
			{
				feed_bomb.turn_around_record = ENABLE;
				feed_bomb.turn_around_enable = ENABLE;
				
				feed_bomb.sight_enable = DISABLE;
				GIM_SERVO_ZERO;	
				flag_feed_bomb_sight = 0;
				
//				servo_feed_bomb_start_delay_record = ENABLE;	
//				servo_feed_bomb_lift_up_delay_record = ENABLE;	
				chassis.ass_mode_enable = DISABLE;       //关闭车尾模式	
				chassis.ctrl_mode = CHASSIS_FOLLOW;  //底盘机械模式
       		
				feed_bomb.vision_enable = DISABLE;
				vision.enable = DISABLE;
			  flag_feed_bomb_vision = 0;//下次默认打开视觉
				
        feed_bomb.lifter_enable = DISABLE;	
        flag_magazine_lifter = 0;//下次默认抬起来			
				
				feed_bomb.exit_enable = DISABLE;				
				MAGAZINE_CLOSE;
				
//				/*岛下的时候延时一下让云台舵机转过来后再降框架 岛上就不用管了*/
				if(climb_tube.location == ON_GROUND)
				{
//					if(servo_feed_bomb_end_delay_record)//延时一小段时间让舵机转回来先
//					{					
//						servo_feed_bomb_end_delay_time = millis();
//						servo_feed_bomb_end_delay_record = 0;
//					}
//					else if(millis() - servo_feed_bomb_end_delay_time > 1000)
//					{
						lifter.ctrl_mode = LIFTER_ZERO;	   //框架回标定的位置					
						feed_bomb.feed_bomb_enable = DISABLE;	
//					}				
				}
				else feed_bomb.feed_bomb_enable = DISABLE;	
				
			}break;	
			
			case AIM_FEED_MODE://对准喂弹模式
			{	
//				servo_feed_bomb_end_delay_record = 1;	
				chassis.ass_mode_enable = ENABLE;       //开启车尾模式	
				
				if(feed_bomb.turn_around_record && feed_bomb.turn_around_enable)		//模拟按键
				{		
					flag_turn_around = 1;
					chassis.position_temp = imu.yaw;
					ramp_init(&turn_around_ramp, TUR_ARO_TIME/INFO_GET_PERIOD);		
					feed_bomb.turn_around_record = DISABLE;
				}
				
				feed_bomb.sight_enable = ENABLE;        //可控云台 默认看小电视 可手动看这个主要用于找目标

					
//				/*岛上的时候框架不抬升 岛下的时候框架抬升*/
//				if(servo_feed_bomb_start_delay_record)//延时一小段时间让舵机转回来先
//				{					
//					servo_feed_bomb_start_delay_time = millis();
//					servo_feed_bomb_start_delay_record = 0;
//				}
//				else if(millis() - servo_feed_bomb_start_delay_time > 1000)
//				{				
					if(climb_tube.location == ON_GROUND)
					{
						feed_bomb.lifter_enable = ENABLE;//使能框架抬升
						
						if(((abs(lifter.target_feed_entire[L] - lifter.total_angle[L]) < 100) || 
			          (abs(lifter.target_feed_entire[R] - lifter.total_angle[R]) < 100)) && 
						    (ramp_calc(&turn_around_ramp) == 1))//抬升到目标角度//抬起来才允许视觉且在调完头后
						{
							feed_bomb.vision_enable = ENABLE;
							
							if(vision.enable == DISABLE)
							{
								chassis.ctrl_mode = CHASSIS_SEPARATE;  //底盘机械模式
							}
							else if(vision.captured)//抓到图像了
							{
								chassis.ctrl_mode = VISION_FEED_BOMB;
						  }	
							else chassis.ctrl_mode = CHASSIS_SEPARATE;  //底盘机械模式
						}
						else 
						{
							feed_bomb.vision_enable = DISABLE;
							vision.enable = DISABLE;
							flag_feed_bomb_vision = 0; //下次默认打开视觉
						}
						
						if(lifter.total_angle[R] > lifter.target_bomb_bit[R] || 
							 lifter.total_angle[L] < lifter.target_bomb_bit[L])
						{//抬到一定高度就可以使能出口控制了
							feed_bomb.exit_enable = ENABLE;
						}	

//					}			
				}	
				else //尽量下来补弹
				{
					feed_bomb.vision_enable = DISABLE;//岛上就不开视觉了吧 
					vision.enable = DISABLE;
			    flag_feed_bomb_vision = 0; //下次默认打开视觉
					
					feed_bomb.sight_enable = ENABLE;
				  feed_bomb.exit_enable = ENABLE;
				}	
			}break;			
			
			default: break;
		}
	}
}

