#include "climb_tube.h"

/*------------------------------------------
关于launch位的功能解释：
功能一:触发功能
功能二:与爪子的配合

详细解释功能二：
当我们按了F车子起飞了 但是没转过去 这个时候我们按ctrl+Z键回平原模式照理是会强制打开爪子的 会直接掉下来
若配合了launch位 该位恰巧记录的是我们是否按下了F键 要是按下了 回平原模式的时候爪子还是强制合上的
若是没按下F键只按了ctrl+Q或者是登岛完了 我都将launch位清掉了 他们再会平原模式的时候就会强制张开爪子
这个时候恢复平原模式检测光电判断车子吊在什么位置 这决定再按ctrl+Q的时候 框架展多少

注：不按下ctrl+Q键的时候 爪子都是不可控的 这样做是为了与取弹爪子共用一个按键
------------------------------------------*/

/* get information task period time (ms) */
#define LANDING_ANGLE_PERIOD     1 //放在哪个时间戳 calc放在了哪个时间戳

/* key acceleration time */
#define LANDING_ANGLE_ACC_TIME   1444  //ms

climb_tube_t climb_tube;
void climb_tube_param_init(void)
{				
	climb_tube.location = ON_GROUND;         //默认在平地
//  kb.tube_claw_enable = ENABLE;            //爪子使能	
	kb.tube_claw_enable = DISABLE;           //爪子失能
	TUBE_CLAW_LOOSE;                         //爪子复位是打开
//	TUBE_BRAKE_PULL;                         //刹车回收
	
	ducted.ctrl_mode = DUCTED_ALL_OFF;       //关涵道
	optic_switch.ctrl_mode = DETECT_ENABLE;  //光电开关开启检测 
//  assist_wheel.ctrl_mode = WHEEL_HOLD;     //摩擦轮锁住			
	lifter.ctrl_mode = LIFTER_ZERO;	   //框架回标定的位置
						
	climb_tube.fiag_launch = 0;              //launch位 置0			
	climb_tube.twisted_angle = 0;            //已经转过的角度清0
	climb_tube.yaw_record = 0;               //记录的角度清0
	climb_tube.yaw_record_enable = ENABLE;   //记录角度使能
	climb_tube.twist_calc_enable = DISABLE;  //角度计算失能	
	
	climb_tube.landing_angle_record = ENABLE; //记录着陆角度
	climb_tube.landing_angle_delay_time_record = ENABLE;//着陆后延时
	ramp_init(&landing_angle_ramp, LANDING_ANGLE_ACC_TIME / LANDING_ANGLE_PERIOD);

	climb_tube.climb_up_optic_enable = ENABLE;
	climb_tube.climb_down_optic_enable = ENABLE;
	climb_tube.climb_up_gyro_enable = ENABLE;
	climb_tube.climb_down_gyro_enable = ENABLE;
		
//	climb_tube.climb_down_delay_time_record = ENABLE;
	
/*注释掉下面这句就意味着开电必须是S1上中下依次*/	
//			  climb_tube.climb_enable = ENABLE;        //让下面那个平原模式跑一遍(主要是为了要是初始就掰在S1下能有个陀螺仪模式)		
}

void climb_tube_ctrl(void)
{
	static bool LAUNCH_RECORD = DISABLE;            //让F键长按的标志位
	
	keyboard_climb_tube_handler();//keyboard	
		
	if(climb_tube.climb_enable == ENABLE)	
	{
		switch(climb_tube.ctrl_mode)
		{
			case PLAIN_MODE://平原模式
			{
				memset(&global_task, ENABLE, sizeof(global_task_t)); //让其他任务也能执行

				LAUNCH_RECORD = DISABLE;                        //自动保持位 置0								
 				kb.tube_claw_enable = DISABLE;                  //爪子失能		
        
				if(chassis.ass_mode_enable)
				{
					GIM_SERVO_BACK;	
				}
				else GIM_SERVO_ZERO;

//				if(climb_tube.location == ON_GROUND)GIM_SERVO_ZERO;	
//				else GIM_SERVO_BACK;
		
        chassis.ctrl_mode = CHASSIS_FOLLOW;             //底盘陀螺仪模式
				
				/*万一出现战损 涵道不能使了 按下了F键涵道莫得反应*/
				if(climb_tube.fiag_launch) 
				{
				  TUBE_CLAW_CATCH;                       //爪子强制扣着			
				  chassis.ctrl_mode = CHASSIS_SEPARATE;  //底盘不要乱动 这时可以让队友撞一下啊什么的
					climb_tube.climb_enable = ENABLE;      //只让这个case一直跑
					
					if(optic_switch.value[S] == 0)// && optic_switch.value[R] == 0)//光电检测到都悬空 判定为平地上
					{
						climb_tube.location = ON_GROUND;
					}
					else if(optic_switch.value[S] == 1)// && optic_switch.value[R] == 1)
					{
						climb_tube.location = ON_ISLAND;//光电检测到都着地 判定为岛上
					}
					else climb_tube.location = IN_DANGER;//其他情况判定为危险 什么也做不了 按ctrl+Q 并不能进入其他part
				}
				else
				{
					TUBE_CLAW_LOOSE;                       //爪子强制张开 			
					climb_tube.climb_enable = DISABLE;     //只让这个case跑一次				
				}				
        /*-----------------------------------------------*/
				
//				TUBE_BRAKE_PULL;                         //刹车回收
		
				optic_switch.ctrl_mode = DETECT_ENABLE;  //光电开关开启检测 这句话是为跳出这个case写的
				lifter.ctrl_mode = LIFTER_ZERO;	         //框架回标定的位置
//			  assist_wheel.ctrl_mode = WHEEL_HOLD;     //摩擦轮锁住			
			  ducted.ctrl_mode = DUCTED_ALL_OFF;       //关涵道
					
				climb_tube.twisted_angle = 0;            //已经转过的角度清0
				climb_tube.yaw_record = 0;               //记录的角度清0
			  climb_tube.yaw_record_enable = ENABLE;   //记录角度使能
				climb_tube.twist_calc_enable = DISABLE;  //角度计算失能	
				
				climb_tube.landing_angle_record = ENABLE; //记录着陆角度
				climb_tube.landing_angle_delay_time_record = ENABLE;//着陆后延时
				ramp_init(&landing_angle_ramp, LANDING_ANGLE_ACC_TIME / LANDING_ANGLE_PERIOD);
				
				climb_tube.climb_up_optic_enable = ENABLE;
				climb_tube.climb_down_optic_enable = ENABLE;
				climb_tube.climb_up_gyro_enable = ENABLE;
				climb_tube.climb_down_gyro_enable = ENABLE;
	
	  		climb_tube.vision_enable = DISABLE;//关掉视觉
				vision.enable = DISABLE;		  
				flag_tube_claw_vision = 0;
				
//				climb_tube.climb_down_delay_time_record = ENABLE;
				
			}break;
			  
			case CLIMEB_UP_OPTIC_MODE://爬杆模式 (光电方法)
			{ 
				chassis.ass_mode_enable = DISABLE;//默认不开车尾模式 要开的话在后面ENABLE
				
				if((LAUNCH_TRIGGER && flag_tube_claw_catch) || LAUNCH_RECORD)
				{
//					TRAILER_HOOK_CAPTURE;//拖车收起来咯
					GIM_SERVO_BACK;
					LAUNCH_RECORD = ENABLE;					
					
					if(climb_tube.twist_calc_enable)
					{
						critical_value_treatment(imu.yaw, &climb_tube.yaw_record, 0, 360);   //临界值处理	
						climb_tube.twisted_angle = climb_tube.yaw_record - imu.yaw;          //转过的角度						
					}
					
					if(climb_tube.fiag_launch == 0 && optic_switch.value[S] == 1)// && optic_switch.value[R] == 1)//一按F就起飞
					{
						chassis.ctrl_mode = MOVE_CLOCKWISE;                      //底盘机械模式
//						ducted.ctrl_mode = DUCTED_L_RUN;                           //左涵道吹
						ducted.ctrl_mode = DUCTED_ALL_RUN;                           //全涵道吹
						kb.tube_claw_enable = DISABLE;                                
            TUBE_CLAW_CATCH;                                           //爪子强制抓住
						 
						lifter.ctrl_mode = LIFTER_ZERO;		                         //框架回标定的位置
						optic_switch.ctrl_mode = DETECT_ENABLE;		                 //启动光电检测
					}	
					else if(climb_tube.fiag_launch == 0 && optic_switch.value[S] == 0)// && optic_switch.value[R] == 0)//
					{
//						ducted.ctrl_mode = DUCTED_L_RUN;                           //左涵道吹
//						assist_wheel.ctrl_mode = WHEEL_FORWARD;                    //摩擦轮转
						climb_tube.fiag_launch = 1;					                       //发射位置1
					}
					else if(climb_tube.fiag_launch == 1 && 
					       ((abs(lifter.total_angle[L] - lifter.target_zero[L]) < 200) || 
					       (abs(lifter.total_angle[R] - lifter.target_zero[R]) < 200)) && 
					        optic_switch.value[S] == 0)// && optic_switch.value[R] == 0)//
					{
//						assist_wheel.ctrl_mode = WHEEL_FORWARD_SLOW;               //摩擦轮慢转
//						ducted.ctrl_mode = DUCTED_R_RUN;                           //右涵道吹
						
			  		climb_tube.twisted_angle = 0;                               //已经转过的角度清0
				  	climb_tube.yaw_record = 0;                                  //记录的角度清0
				    climb_tube.yaw_record_enable = ENABLE;                      //记录角度使能
				    climb_tube.twist_calc_enable = DISABLE;                     //角度计算失能
					}				
					else if(climb_tube.yaw_record_enable == ENABLE && climb_tube.fiag_launch == 1 && 
					        ((abs(lifter.total_angle[L] - lifter.target_zero[L]) < 200) || 
					        (abs(lifter.total_angle[R] - lifter.target_zero[R]) < 200)) && 
					        optic_switch.value[S] == 1)// && optic_switch.value[R] == 1)//光电都检测到台阶&抬升到了位置
					{
//						TUBE_BRAKE_PUSH;		                                         //刹车弹出
//						ducted.ctrl_mode = DUCTED_R_RUN;                           //右涵道吹
						climb_tube.yaw_record = imu.yaw;                             //获取陀螺仪角度作为当前角度
						climb_tube.yaw_record_enable = DISABLE;                      //只记录一次
						climb_tube.twist_calc_enable = ENABLE;                       //开启计算
					}					
					else if((abs(climb_tube.twisted_angle - 38) < 1) && climb_tube.fiag_launch == 1 && 
						      optic_switch.value[S] == 1)// && optic_switch.value[R] == 1)//陀螺仪转到了位置
					{
						climb_tube.twisted_angle = 38;                               //强制让它到了这个角度
						climb_tube.twist_calc_enable = DISABLE;                      //不要计算了
						
						ducted.ctrl_mode = DUCTED_ALL_OFF;                           //关涵道
//						assist_wheel.ctrl_mode = WHEEL_HOLD;                         //摩擦轮hold住
						lifter.ctrl_mode = TUBE_LIFTER_BIT;                         //框架抬升一点点
						optic_switch.ctrl_mode = FORCE_ONE;                          //光电开关强制置1	保险用
						if((abs(lifter.target_tube_bit[L] - lifter.total_angle[L]) < 120) || 
							 (abs(lifter.target_tube_bit[R] - lifter.total_angle[R]) < 120))//抬升到目标角度
						{
							TUBE_CLAW_LOOSE;		                                       //爪子松开
//              TUBE_BRAKE_PULL;									                         //刹车收回
							chassis.ctrl_mode = MOVE_FORWARD;                          //底盘向前跑
              climb_tube.location = ON_ISLAND;/*提前一点点标记为岛上 不知道有什么问题*/	
							
							if(FORWARD || BACK || GET_BOMB_CTRL)
							{
								climb_tube.fiag_launch = 0;              //launch位 置0			
								climb_tube.ctrl_mode = PLAIN_MODE;	//前后左右切回平原模式
                chassis.ass_mode_enable = ENABLE;			
								climb_tube.location = ON_ISLAND;    //已经在岛上咯					
		 					}
						}
					}			
				}
				else 	
				{
					kb.tube_claw_enable = ENABLE;                                //爪子可控
					
					lifter.ctrl_mode = TUBE_LIFTER_ENTIRE;		                   //框架抬升 抬到目标之后保持
					
					if(flag_tube_claw_catch)
					{
//				   	TRAILER_HOOK_RELEASE;                                         //钩子气缸弹下辅助一下
						chassis.ctrl_mode = CHASSIS_SEPARATE;  //底盘机械模式(绕柱的)
						
						climb_tube.vision_enable = DISABLE;//关掉视觉
						vision.enable = DISABLE;		  
						flag_tube_claw_vision = 0;//下次默认打开视觉
					}
					else 
					{
//					  TRAILER_HOOK_CAPTURE;   
						if(lifter.total_angle[R] > lifter.target_bomb_entire[R] || 
							 lifter.total_angle[L] < lifter.target_bomb_entire[L])
						{
							climb_tube.vision_enable = ENABLE;//打开视觉可控
						}	
						
						if(vision.enable == DISABLE)
						{
							chassis.ctrl_mode = CHASSIS_SEPARATE;//CHASSIS_FOLLOW; //底盘陀螺仪
						}
						else if(vision.captured)//抓到图像了
						{
					  	chassis.ctrl_mode = VISION_CLIMB_TUBE;
						}
						else chassis.ctrl_mode = CHASSIS_SEPARATE;//CHASSIS_FOLLOW; //底盘陀螺仪

					}
					
					optic_switch.ctrl_mode = FORCE_ONE;                          //光电开关强制置1  框架抬升起来后光电先全部置1 以保证能起飞
				  
//					if(lifter.total_angle[R] > lifter.target_bomb_bit[R] || 
//						 lifter.total_angle[L] < lifter.target_bomb_bit[L])
//					{
				  	GIM_SERVO_SIDE;
//					}
					
					climb_tube.fiag_launch = 0;                                  //launch位 置0			
				}
			}break;
			
			case CLIMEB_DOWN_OPTIC_MODE://下杆模式 (光电方法)
			{ 
				chassis.ass_mode_enable = DISABLE;//默认不开车尾模式 要开的话在后面ENABLE
				
				if((LAUNCH_TRIGGER && flag_tube_claw_catch) || LAUNCH_RECORD)
				{
//					TRAILER_HOOK_CAPTURE;//拖车收起来咯					
					LAUNCH_RECORD = ENABLE;
					GIM_SERVO_ZERO;
					
//					if(climb_tube.climb_down_delay_time_record)//
//					{
//						climb_tube.climb_down_delay_time = millis();
//						climb_tube.climb_down_delay_time_record = DISABLE;
//					}
//					else if(millis() - climb_tube.climb_down_delay_time > 500)
//					{
						if(climb_tube.twist_calc_enable)
						{						
							critical_value_treatment(imu.yaw, &climb_tube.yaw_record, 0, 360);   //临界值处理	
							climb_tube.twisted_angle = imu.yaw - climb_tube.yaw_record;          //转过的角度						
						}
					
						if(climb_tube.fiag_launch == 0 && optic_switch.value[S] == 1)// && optic_switch.value[R] == 1)//其实这里一按下F就起飞了
						{
							chassis.ctrl_mode = MOVE_ANTICLOCKWISE;                         //底盘机械模式
							
							kb.tube_claw_enable = DISABLE;                                
							TUBE_CLAW_CATCH;                                              //爪子强制抓住
							
							lifter.ctrl_mode = LIFTER_ZERO;		                            //框架回标定的位置
							optic_switch.ctrl_mode = DETECT_ENABLE;                       //光电开关开放检测	
							ducted.ctrl_mode = DUCTED_ALL_OFF;                            //全涵道关					
	//						ducted.ctrl_mode = DUCTED_R_RUN;                              //右涵道吹
	//						assist_wheel.ctrl_mode = WHEEL_REVERSE;                     //摩擦轮转
							climb_tube.fiag_launch = 1;					                          //发射位置1
						}
	//					else if(optic_switch.value[L] == 1 && optic_switch.value[R] == 0 && climb_tube.fiag_launch == 1 && ((abs(lifter.total_angle[L] - lifter.target_zero[L]) < 200) || (abs(lifter.total_angle[R] - lifter.target_zero[R]) < 200)))//
						else if(climb_tube.fiag_launch == 1 && optic_switch.value[S] == 1)// && optic_switch.value[R] == 0)
						{
	// 						assist_wheel.ctrl_mode = WHEEL_REVERSE_SLOW;                   //摩擦轮慢转
	//						ducted.ctrl_mode = DUCTED_L_RUN;                               //左涵道吹
							
							climb_tube.twisted_angle = 0;                                 //已经转过的角度清0
							climb_tube.yaw_record = 0;                                    //记录的角度清0
							climb_tube.yaw_record_enable = ENABLE;                        //记录角度使能
							climb_tube.twist_calc_enable = DISABLE;                       //角度计算失能
						}				

	//					else if(optic_switch.value[L] == 0 && optic_switch.value[R] == 0 && climb_tube.yaw_record_enable == ENABLE && climb_tube.fiag_launch == 1 && ((abs(lifter.total_angle[L] - lifter.target_zero[L]) < 200) || (abs(lifter.total_angle[R] - lifter.target_zero[R]) < 200)))//光电都检测到悬空&抬升到了位置
						else if(climb_tube.yaw_record_enable == ENABLE && climb_tube.fiag_launch == 1 && 
										optic_switch.value[S] == 0)// && optic_switch.value[R] == 0)				
						{
	//						TUBE_BRAKE_PUSH;		                                           //刹车弹出
	//						ducted.ctrl_mode = DUCTED_L_RUN;                               //左涵道吹
							
							climb_tube.yaw_record = imu.yaw;                               //获取陀螺仪角度作为当前角度
							climb_tube.yaw_record_enable = DISABLE;                        //只记录一次 
							climb_tube.twist_calc_enable = ENABLE;                         //开启计算
						}					
						else if((abs(climb_tube.twisted_angle - 65) < 1) && climb_tube.fiag_launch == 1 && 
										optic_switch.value[S] == 0)// && optic_switch.value[R] == 0)//陀螺仪转到了位置
						{
							climb_tube.twisted_angle = 65;                                  //强制让它到了这个角度
							climb_tube.twist_calc_enable = DISABLE;                         //不要计算了
							
							ducted.ctrl_mode = DUCTED_ALL_OFF;                              //关涵道
	//						assist_wheel.ctrl_mode = WHEEL_HOLD;                           //摩擦轮hold住
							lifter.ctrl_mode = TUBE_LIFTER_ENTIRE;                         //框架抬升
	//						lifter.ctrl_mode = TUBE_LIFTER_BIT;                          //框架抬升
							optic_switch.ctrl_mode = FORCE_ZERO;                            //光电开关强制置0 让它落地也能跑这个elseif里的程序
							chassis.ctrl_mode = CHASSIS_SEPARATE;                      //底盘机械模式
							if((abs(lifter.target_tube_entire[L] - lifter.total_angle[L]) < 120) || 
								 (abs(lifter.target_tube_entire[R] - lifter.total_angle[R]) < 120))//抬升到目标角度
							{
								TUBE_CLAW_LOOSE;                                              //爪子松开
	//							TUBE_BRAKE_PULL;									                            //刹车收回
								chassis.ctrl_mode = MOVE_FORWARD;                             //底盘向前跑
	//							GIM_SERVO_ZERO;
								if(FORWARD || BACK || LEFT || RIGHT)  
								{
									climb_tube.fiag_launch = 0;              //launch位 置0			
									climb_tube.ctrl_mode = PLAIN_MODE;	//前后左右切回平原模式
									chassis.ass_mode_enable = DISABLE;  //车尾模式关闭		    
									climb_tube.location = ON_GROUND;    //已经在平地上咯		  								
								}
							}
						}							
//					}	
				}
				else
				{
					kb.tube_claw_enable = ENABLE;                                //爪子可控
										
					if(flag_tube_claw_catch)
					{
//				   	TRAILER_HOOK_RELEASE;                                         //钩子气缸弹下辅助一下
						chassis.ctrl_mode = CHASSIS_SEPARATE;                      //底盘机械模式
					}
					else 
					{
//						TRAILER_HOOK_CAPTURE;
						chassis.ctrl_mode = CHASSIS_FOLLOW;                        //底盘陀螺仪模式
					}
					
					optic_switch.ctrl_mode = FORCE_ONE;                          //光电开关强制置1  框架抬升起来后光电先全部置1 以保证能起飞										
					lifter.ctrl_mode = TUBE_LIFTER_BIT;		                       //框架抬升一点点 抬到目标之后保持
					GIM_SERVO_SIDE;
					climb_tube.fiag_launch = 0;              //launch位 置0			
				}				
			}break;
			
			case CLIMEB_UP_GYRO_MODE://爬杆模式 (陀螺仪方法)
			{ 
				chassis.ass_mode_enable = DISABLE;//默认不开车尾模式 要开的话在后面ENABLE
				
				if((LAUNCH_TRIGGER && flag_tube_claw_catch) || LAUNCH_RECORD)
				{
					GIM_SERVO_BACK;
					LAUNCH_RECORD = ENABLE;					
					
					if((((abs(lifter.total_angle[L] - lifter.target_zero[L]) < 200) || 
					     (abs(lifter.total_angle[R] - lifter.target_zero[R]) < 200)) && 
					     (abs(climb_tube.twisted_angle - 149) < 1)) || climb_tube.fiag_launch)//
					{					
		        climb_tube.fiag_launch = 1;
						climb_tube.twisted_angle = 149;                             //强制让它到了这个角度											
						ducted.ctrl_mode = DUCTED_ALL_OFF;                          //关涵道
						lifter.ctrl_mode = TUBE_LIFTER_BIT;                         //框架抬升一点点
						
						if((abs(lifter.target_tube_bit[L] - lifter.total_angle[L]) < 120) || (abs(lifter.target_tube_bit[R] - lifter.total_angle[R]) < 120))//抬升到目标角度
						{
							TUBE_CLAW_LOOSE;		                                       //爪子松开
							chassis.ctrl_mode = MOVE_FORWARD;                          //底盘向前跑
							climb_tube.location = ON_ISLAND;/*提前一点点标记为岛上 不知道有什么问题*/	

							
							if(FORWARD || BACK || GET_BOMB_CTRL)
							{	
								climb_tube.fiag_launch = 0;
								climb_tube.ctrl_mode = PLAIN_MODE;	//前后左右切回平原模式
                chassis.ass_mode_enable = ENABLE;			
								climb_tube.location = ON_ISLAND;    //已经在岛上咯					
		 					}
						}
					}	
					else 
					{
						critical_value_treatment(imu.yaw, &climb_tube.yaw_record, 0, 360);   //临界值处理	
						climb_tube.twisted_angle = climb_tube.yaw_record - imu.yaw;          //转过的角度						
					
						chassis.ctrl_mode = MOVE_CLOCKWISE;                          //底盘机械模式
						ducted.ctrl_mode = DUCTED_ALL_RUN;                           //全涵道吹
						kb.tube_claw_enable = DISABLE;                                
            TUBE_CLAW_CATCH;                                           //爪子强制抓住					 
						lifter.ctrl_mode = LIFTER_ZERO;		                         //框架回标定的位置
					}	
				}
				else 	
				{
					kb.tube_claw_enable = ENABLE;                                //爪子可控
				
					lifter.ctrl_mode = TUBE_LIFTER_ENTIRE;		                   //框架抬升 抬到目标之后保持
									
					if(flag_tube_claw_catch)
					{
						chassis.ctrl_mode = CHASSIS_SEPARATE;                      //底盘机械模式
					
						climb_tube.vision_enable = DISABLE;//关掉视觉
						vision.enable = DISABLE;		  
						flag_tube_claw_vision = 0;//下次默认打开视觉
					}
					else 
					{
						if(lifter.total_angle[R] > lifter.target_bomb_entire[R] || 
							 lifter.total_angle[L] < lifter.target_bomb_entire[L])
						{
							climb_tube.vision_enable = ENABLE;//打开视觉可控 默认打开
						}
						
						if(vision.enable == DISABLE)
						{
							chassis.ctrl_mode = CHASSIS_SEPARATE;//CHASSIS_FOLLOW; //底盘陀螺仪
						}
						else if(vision.captured)
						{
					  	chassis.ctrl_mode = VISION_CLIMB_TUBE;
						}
						else chassis.ctrl_mode =CHASSIS_SEPARATE;// CHASSIS_FOLLOW; //底盘陀螺仪
					}		
					
				  
//					if(lifter.total_angle[R] > lifter.target_bomb_bit[R] || 
//						 lifter.total_angle[L] < lifter.target_bomb_bit[L])
//					{
				  	GIM_SERVO_SIDE;
//					}		
          climb_tube.yaw_record = imu.yaw;                             //获取陀螺仪角度作为当前角度				
					climb_tube.fiag_launch = 0;                                  //launch位 置0			
				}
			}break;
			
					
			case CLIMEB_DOWN_GYRO_MODE://下杆模式 (陀螺仪方法)
			{ 
				chassis.ass_mode_enable = DISABLE;//默认不开车尾模式 要开的话在后面ENABLE
				
				if((LAUNCH_TRIGGER && flag_tube_claw_catch) || LAUNCH_RECORD)
				{	
					LAUNCH_RECORD = ENABLE;
	
					if(abs(climb_tube.twisted_angle - 135) < 1)//陀螺仪转到了位置
					{
						climb_tube.twisted_angle = 135;                                 //强制让它到了这个角度
						climb_tube.twist_calc_enable = DISABLE;                         //不要计算了
						
						ducted.ctrl_mode = DUCTED_ALL_OFF;                              //关涵道
						lifter.ctrl_mode = TUBE_LIFTER_ENTIRE;                         //框架抬升
//						lifter.ctrl_mode = TUBE_LIFTER_BIT;                             //框架抬升
						chassis.ctrl_mode = CHASSIS_SEPARATE;                          //底盘机械模式
						
						if((abs(lifter.target_tube_entire[L] - lifter.total_angle[L]) < 120) || 
							 (abs(lifter.target_tube_entire[R] - lifter.total_angle[R]) < 120))//抬升到目标角度
						{
              TUBE_CLAW_LOOSE;                                              //爪子松开
							chassis.ctrl_mode = MOVE_FORWARD;                             //底盘向前跑
							GIM_SERVO_ZERO;
							if(FORWARD || BACK || LEFT || RIGHT)  
							{
								climb_tube.fiag_launch = 0;              //launch位 置0			
								climb_tube.ctrl_mode = PLAIN_MODE;	//前后左右切回平原模式
								chassis.ass_mode_enable = DISABLE;  //车尾模式关闭		    
	              climb_tube.location = ON_GROUND;    //已经在平地上咯		  								
							}
						}
					}
					else 
					{
						critical_value_treatment(imu.yaw, &climb_tube.yaw_record, 0, 360);   //临界值处理	
						climb_tube.twisted_angle = imu.yaw - climb_tube.yaw_record;          //转过的角度						

						chassis.ctrl_mode = MOVE_ANTICLOCKWISE;                              //底盘机械模式					
						kb.tube_claw_enable = DISABLE;                                
            TUBE_CLAW_CATCH;                                                     //爪子强制抓住
						
						lifter.ctrl_mode = LIFTER_ZERO;		                                   //框架回标定的位置
						ducted.ctrl_mode = DUCTED_ALL_OFF;                                   //全涵道关			
					}					
				}
				else
				{
					kb.tube_claw_enable = ENABLE;                                //爪子可控
										
					if(flag_tube_claw_catch)
					{
						chassis.ctrl_mode = CHASSIS_SEPARATE;                      //底盘机械模式
					}
					else 
					{
						chassis.ctrl_mode = CHASSIS_FOLLOW;                        //底盘陀螺仪模式
					}					
					lifter.ctrl_mode = TUBE_LIFTER_BIT;		                       //框架抬升一点点 抬到目标之后保持
					GIM_SERVO_SIDE;
					
					climb_tube.yaw_record = imu.yaw;                             //获取陀螺仪角度作为当前角度
					climb_tube.fiag_launch = 0;                                  //launch位 置0			
				}				
			}break;


			default: break;
		}
	}
}

void climb_tube_task(void)
{
//	if(lifter.reset)
//	{
		climb_tube_ctrl();
//	}

}

