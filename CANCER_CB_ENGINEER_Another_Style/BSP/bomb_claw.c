#include "bomb_claw.h"

/* get information task period time (ms) */
#define BOMB_CLAW_RESET_SPD_PERIOD 3 //放在哪个时间戳

/* key acceleration time */
#define BOMB_CLAW_RESET_ACC_TIME   100  //ms

bomb_claw_t bomb_claw;

void bomb_claw_param_init(void)
{		
  memset(&bomb_claw, 0, sizeof(bomb_claw_t));

	ramp_init(&reset_bomb_claw_spd_ramp, BOMB_CLAW_RESET_ACC_TIME/BOMB_CLAW_RESET_SPD_PERIOD);
	
	for(u8 k = 0; k < 4; k++)//四个电机一套PID
	{
		pidckz_reset(&pid_bomb_claw_position[k]);
		pidckz_reset(&pid_bomb_claw_speed[k]);
	
		pidckz_param_set(&pid_bomb_claw_position[k], 8000, 1000, 7.5f, 0.02f, 0.0f);            //拨盘电机位置环
		pidckz_param_set(&pid_bomb_claw_speed[k], 8000, 1000, 15.2f, 0.0f, 0.0f);	            //拨盘电机速度环
	
	}
	
  /*复位堵转时间记录标志位*/		
  bomb_claw.LR_stall_time_record = ENABLE;
	bomb_claw.Y_stall_time_record = ENABLE;
	
	bomb_claw.reset_start_time_record = ENABLE;
  /*复位速度初始化*/
	bomb_claw.reset_speed[L] = 1200;
	bomb_claw.reset_speed[R] = -1200;
	bomb_claw.reset_speed[Y] = -3300;
//	/*复位上电是Y轴平移至中间以及R轴翻转回来所用的标志位*/
//	bomb_claw.flag_working_mode = ENABLE;
  /*各种标志位置1*/
  memset(bomb_claw.ecd_Record, ENABLE, sizeof(bomb_claw.ecd_Record));
	
	memset(bomb_claw.from_claw_zero, YES, sizeof(bomb_claw.from_claw_zero));
	memset(bomb_claw.from_claw_bit, YES, sizeof(bomb_claw.from_claw_bit));
	memset(bomb_claw.from_claw_half, YES, sizeof(bomb_claw.from_claw_half));
	memset(bomb_claw.from_claw_entire, YES, sizeof(bomb_claw.from_claw_entire));	
	
	memset(bomb_claw.flag_claw_zero, ENABLE, sizeof(bomb_claw.flag_claw_zero));
	memset(bomb_claw.flag_claw_bit, ENABLE, sizeof(bomb_claw.flag_claw_bit));
	memset(bomb_claw.flag_claw_half, ENABLE, sizeof(bomb_claw.flag_claw_half));
	memset(bomb_claw.flag_claw_entire, ENABLE, sizeof(bomb_claw.flag_claw_entire));	
	
	//堵转保护
	memset(bomb_claw.stall_protect_record, ENABLE, sizeof(bomb_claw.stall_protect_record));	

	bomb_claw.target_entire[L] = -13000;
	bomb_claw.target_bit[L] = -12345;
	bomb_claw.target_half[L] = -7000;	
	bomb_claw.target_zero[L] = -555;
 
	bomb_claw.target_entire[R] =  13000;
  bomb_claw.target_bit[R] = 12345;
	bomb_claw.target_half[R] =  7000;
	bomb_claw.target_zero[R] = 555;
	
//	bomb_claw.target_entire[X] = 50000;
//	bomb_claw.target_half[X] = 25000;
//	bomb_claw.target_bit[X] = 1000;
	
	bomb_claw.Y_target[L] = 1600;
	bomb_claw.Y_target[M] = 40500;	
	bomb_claw.Y_target[R] = bomb_claw.Y_target[M] * 2 - bomb_claw.Y_target[L];
	
	bomb_claw.target_entire[Y] =  bomb_claw.Y_target[R];
	bomb_claw.target_half[Y] =  bomb_claw.Y_target[M];	
	bomb_claw.target_zero[Y] = bomb_claw.Y_target[L];
}

void bomb_claw_cas_ctrl(int32_t target_angle[])//串级计算
{	                                                  
	for(u8 k = 0; k < 4; k++)
	{
		bomb_claw.speed_ref[k] = pidckz_calc(&pid_bomb_claw_position[k], bomb_claw.total_angle[k], target_angle[k]);         //外环(位置环)PID计算
		bomb_claw.current[k] = pidckz_calc(&pid_bomb_claw_speed[k], bomb_claw.speed_fdb[k], bomb_claw.speed_ref[k]);         //PID计算
		
		if((abs(bomb_claw.current[k]) > 4500) || bomb_claw.across_stall_protect[k])
		{
			if(bomb_claw.stall_protect_record[k])
			{
				bomb_claw.stall_protect_time[k] = millis();
				bomb_claw.stall_protect_record[k] = DISABLE;
			} 
			if((millis() - bomb_claw.stall_protect_time[k] > 2000) &&
				 (millis() - bomb_claw.stall_protect_time[k] < 2080))
			{
				bomb_claw.across_stall_protect[k] = ENABLE;
				
				if(bomb_claw.current[k] > 0)
				{
				  bomb_claw.current[k] = 3000;
				}
				else bomb_claw.current[k] = -3000;
			}
			else if(millis() - bomb_claw.stall_protect_time[k] > 2080)
			{
		  	bomb_claw.stall_protect_record[k] = ENABLE;
				bomb_claw.across_stall_protect[k] = DISABLE;
				bomb_claw.stall_times[k] ++;
			}				
		}
	  else 
		{
			bomb_claw.stall_protect_record[k] = ENABLE;
			bomb_claw.across_stall_protect[k] = DISABLE;
		}
	}
}

void bomb_claw_spd_ctrl(int16_t target_speed[])//速度环计算
{	                                                  
	for(u8 k = 0; k < 4; k++)
	{
		bomb_claw.current[k] = pidckz_calc(&pid_bomb_claw_speed[k], bomb_claw.speed_fdb[k], target_speed[k]);         //PID计算
	}
}

void bomb_claw_data_handler(u8 k)//记圈数的函数置于CAN中断里
{	
//	if(bomb_claw.ALL_reset)
//	{
		if(bomb_claw.ecd_Record[k] == ENABLE)
		{
			bomb_claw.ecd_Record[k] = DISABLE;
			
			/*只记录一次初始偏差*/     
			bomb_claw.offset_ecd[k] = bomb_claw.angle_fdb[k]; 				
			
			bomb_claw.las_ecd[k] = bomb_claw.angle_fdb[k];
			bomb_claw.cur_ecd[k] = bomb_claw.angle_fdb[k];
		}	
		bomb_claw.las_ecd[k] = bomb_claw.cur_ecd[k];
		bomb_claw.cur_ecd[k] = bomb_claw.angle_fdb[k];
	
		if(bomb_claw.cur_ecd[k] - bomb_claw.las_ecd[k] > 4096)
		{
			bomb_claw.round_cnt[k]--;
		}
		else if(bomb_claw.cur_ecd[k] - bomb_claw.las_ecd[k] < -4096)
		{
			bomb_claw.round_cnt[k]++;
		}
		bomb_claw.total_ecd[k] = bomb_claw.round_cnt[k] * 8192 + bomb_claw.cur_ecd[k] - bomb_claw.offset_ecd[k];
		bomb_claw.total_angle[k] = bomb_claw.total_ecd[k] / ENCODER_ANGLE_RATIO;//度数换算 最后出来的东西是转子的转动总角度
//	}
}

void bomb_claw_ctrl(void)
{
/*----------------------------------运动过程位置记录----------------------------------*/		
	{
		/*R_mode位置标定*/ 
		if((abs(bomb_claw.target_entire[L] - bomb_claw.total_angle[L]) < 100) || 
			 (abs(bomb_claw.target_entire[R] - bomb_claw.total_angle[R]) < 100))							
		{
			bomb_claw.from_claw_entire[R_axis] = YES;
			bomb_claw.from_claw_bit[R_axis] = NO;
			bomb_claw.from_claw_half[R_axis] = NO;
			bomb_claw.from_claw_zero[R_axis] = NO;
		}
		else if((abs(bomb_claw.target_bit[L] - bomb_claw.total_angle[L]) < 100) || 
			      (abs(bomb_claw.target_bit[R] - bomb_claw.total_angle[R]) < 100))							
		{
			bomb_claw.from_claw_entire[R_axis] = NO;
			bomb_claw.from_claw_bit[R_axis] = YES;
			bomb_claw.from_claw_half[R_axis] = NO;
			bomb_claw.from_claw_zero[R_axis] = NO;
		}
		else if((abs(bomb_claw.target_half[L] - bomb_claw.total_angle[L]) < 100) || 
			      (abs(bomb_claw.target_half[R] - bomb_claw.total_angle[R]) < 100))							
		{
			bomb_claw.from_claw_entire[R_axis] = NO;
			bomb_claw.from_claw_bit[R_axis] = NO;
			bomb_claw.from_claw_half[R_axis] = YES;
			bomb_claw.from_claw_zero[R_axis] = NO;
		}
		else if((abs(bomb_claw.target_zero[L] - bomb_claw.total_angle[L]) < 100) || 
			      (abs(bomb_claw.target_zero[R] - bomb_claw.total_angle[R]) < 100))							
		{
			bomb_claw.from_claw_entire[R_axis] = NO;
			bomb_claw.from_claw_bit[R_axis] = NO;
			bomb_claw.from_claw_half[R_axis] = NO;
			bomb_claw.from_claw_zero[R_axis] = YES;
		}
		/*Y_mode位置标定*/
		if(abs(bomb_claw.target_zero[Y] - bomb_claw.total_angle[Y]) < 100)	
//		if(bomb_claw.total_angle[Y] - bomb_claw.target_half[Y] < 0)	
		{
			bomb_claw.from_claw_entire[Y_axis] = NO;
			bomb_claw.from_claw_zero[Y_axis] = YES;
		}
		else if(abs(bomb_claw.target_entire[Y] - bomb_claw.total_angle[Y]) < 100)	
//		else if(bomb_claw.total_angle[Y] - bomb_claw.target_half[Y] > 0)			
		{
			bomb_claw.from_claw_entire[Y_axis] = YES;
			bomb_claw.from_claw_zero[Y_axis] = NO;
		}
	}	
/*----------------------------------电机控制----------------------------------*/		
	{
		switch(bomb_claw.R_mode)
		{		
	/*----------------------------------R轴----------------------------------*/		
			
			case R_CLAW_ENTIRE://爪子完全收回来
			{									
				if(bomb_claw.flag_claw_entire[R_axis])
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L];
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R];	
					/*标志位处理*/
					bomb_claw.flag_claw_entire[R_axis] = DISABLE;
					bomb_claw.flag_claw_half[R_axis] = ENABLE;
					bomb_claw.flag_claw_bit[R_axis] = ENABLE;
					bomb_claw.flag_claw_zero[R_axis] = ENABLE;
				}
								
				if(bomb_claw.target_angle[L] > bomb_claw.target_entire[L])
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L] - 333;					
				}
				else bomb_claw.target_angle[L] = bomb_claw.target_entire[L];
				
				if(bomb_claw.target_angle[R] < bomb_claw.target_entire[R])
				{
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R] + 333;			
				}
				else bomb_claw.target_angle[R] = bomb_claw.target_entire[R];
										
			}break;
			
			case R_CLAW_HALF://爪子垂直
			{
				if(bomb_claw.flag_claw_half[R_axis])
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L];
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R];	
					/*标志位处理*/
					bomb_claw.flag_claw_half[R_axis] = DISABLE;
					bomb_claw.flag_claw_bit[R_axis] = ENABLE;
					bomb_claw.flag_claw_entire[R_axis] = ENABLE;
					bomb_claw.flag_claw_zero[R_axis] = ENABLE;	
				}

				
				if((bomb_claw.target_angle[L] < bomb_claw.target_half[L]) && (bomb_claw.from_claw_bit[R_axis] || bomb_claw.from_claw_entire[R_axis]))//从entire来
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L] + 800;					
				}			
				else if((bomb_claw.target_angle[L] > bomb_claw.target_half[L]) && bomb_claw.from_claw_zero[R_axis])//从zero或者从bit来
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L] - 800;			
				}
				else bomb_claw.target_angle[L] = bomb_claw.target_half[L];
				
				if((bomb_claw.target_angle[R] > bomb_claw.target_half[R]) && (bomb_claw.from_claw_bit[R_axis] || bomb_claw.from_claw_entire[R_axis]))//从entire来
				{
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R] - 800;					
				}			
				else if((bomb_claw.target_angle[R] < bomb_claw.target_half[R]) && bomb_claw.from_claw_zero[R_axis])//从zero或者从bit来
				{
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R] + 800;			
				}
				else bomb_claw.target_angle[R] = bomb_claw.target_half[R];
					 
			}break;
						
			case R_CLAW_BIT://爪子收回来还差一丢丢
			{				
				if(bomb_claw.flag_claw_bit[R_axis])
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L];
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R];	
					/*标志位处理*/
					bomb_claw.flag_claw_bit[R_axis] = DISABLE;				
					bomb_claw.flag_claw_zero[R_axis] = ENABLE;				
					bomb_claw.flag_claw_entire[R_axis] = ENABLE;
					bomb_claw.flag_claw_half[R_axis] = ENABLE;
				}
					
				if((bomb_claw.target_angle[L] < bomb_claw.target_bit[L]) && bomb_claw.from_claw_entire[R_axis])//从entire或half来
				{					
					if(get_bomb.box_num == 1 | get_bomb.box_num == 2)//在第二箱&第三箱的时候
					{
						bomb_claw.target_angle[L] = bomb_claw.total_angle[L] + 450;		
					}
					else bomb_claw.target_angle[L] = bomb_claw.total_angle[L] + 650;					
				}			
				else if((bomb_claw.target_angle[L] > bomb_claw.target_bit[L]) && (bomb_claw.from_claw_zero[R_axis] || bomb_claw.from_claw_half[R_axis]))//从zero来
				{
					if(get_bomb.box_num == 1 | get_bomb.box_num == 2)//在第二箱&第三箱的时候
					{
						bomb_claw.target_angle[L] = bomb_claw.total_angle[L] - 450;	
					}
					else bomb_claw.target_angle[L] = bomb_claw.total_angle[L] - 650;			
				}
				else bomb_claw.target_angle[L] = bomb_claw.target_bit[L];
				
				if((bomb_claw.target_angle[R] > bomb_claw.target_bit[R]) && bomb_claw.from_claw_entire[R_axis])//从entire或half来
				{
					if(get_bomb.box_num == 1 | get_bomb.box_num == 2)//在第二箱&第三箱的时候
					{
						bomb_claw.target_angle[R] = bomb_claw.total_angle[R] - 450;		
					}
					else bomb_claw.target_angle[R] = bomb_claw.total_angle[R] - 650;					
				}			
				else if((bomb_claw.target_angle[R] < bomb_claw.target_bit[R]) && (bomb_claw.from_claw_zero[R_axis] || bomb_claw.from_claw_half[R_axis]))//从zero来
				{
					if(get_bomb.box_num == 1 | get_bomb.box_num == 2)//在第二箱&第三箱的时候
					{
						bomb_claw.target_angle[R] = bomb_claw.total_angle[R] + 450;	
					}
					else bomb_claw.target_angle[R] = bomb_claw.total_angle[R] + 650;			
				}
				else bomb_claw.target_angle[R] = bomb_claw.target_bit[R];
				
			}break;
			
			case R_CLAW_ZERO://爪子归零(完全翻出去)
			{				
				if(bomb_claw.flag_claw_zero[R_axis])
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L];
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R];	
					/*标志位处理*/
					bomb_claw.flag_claw_zero[R_axis] = DISABLE;				
					bomb_claw.flag_claw_entire[R_axis] = ENABLE;
					bomb_claw.flag_claw_half[R_axis] = ENABLE;
					bomb_claw.flag_claw_bit[R_axis] = ENABLE;	
				}
					
				if(bomb_claw.target_angle[L] < bomb_claw.target_zero[L])
				{
					bomb_claw.target_angle[L] = bomb_claw.total_angle[L] + 800;					
				}
				else bomb_claw.target_angle[L] = bomb_claw.target_zero[L];

				
				if(bomb_claw.target_angle[R] > bomb_claw.target_zero[R])
				{
					bomb_claw.target_angle[R] = bomb_claw.total_angle[R] - 800;			
				}
				else bomb_claw.target_angle[R] = bomb_claw.target_zero[R];
				
			}break;
						
			default: break;
		}  		
		
	/*----------------------------------X轴(弃用)----------------------------------*/		
	{
		switch(bomb_claw.X_mode)
		{		
			case X_CLAW_ENTIRE://爪子完全伸出
			{									
				if(bomb_claw.flag_claw_entire[X_axis])
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X];
					/*标志位处理*/
					bomb_claw.flag_claw_entire[X_axis] = DISABLE;
					bomb_claw.flag_claw_zero[X_axis] = ENABLE;
					bomb_claw.flag_claw_half[X_axis] = ENABLE;
					bomb_claw.flag_claw_bit[X_axis] = ENABLE;
					
					bomb_claw.from_claw_entire[X_axis] = YES;
					bomb_claw.from_claw_half[X_axis] = NO;
					bomb_claw.from_claw_bit[X_axis] = NO;
					bomb_claw.from_claw_zero[X_axis] = NO;
				}
				if(bomb_claw.target_angle[X] < bomb_claw.target_entire[X])
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X] + 100;			
				}
				else bomb_claw.target_angle[X] = bomb_claw.target_entire[X];				
						
			}break;
			
			case X_CLAW_HALF: //爪子伸出一半
			{
				if(bomb_claw.flag_claw_half[X_axis])
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X];	
					/*标志位处理*/
					bomb_claw.flag_claw_half[X_axis] = DISABLE;				
					bomb_claw.flag_claw_bit[X_axis] = ENABLE;
					bomb_claw.flag_claw_zero[X_axis] = ENABLE;
					bomb_claw.flag_claw_entire[X_axis] = ENABLE;
					
					bomb_claw.from_claw_half[X_axis] = YES;
				}

				if((bomb_claw.target_angle[X] > bomb_claw.target_half[X]) && bomb_claw.from_claw_entire[X_axis])//从entire来
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X] - 100;					
				}			
				else if((bomb_claw.target_angle[X] < bomb_claw.target_half[X]) && (bomb_claw.from_claw_bit[X_axis] || bomb_claw.from_claw_zero[X_axis]))//从bit或是zero来
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X] + 100;			
				}
				else bomb_claw.target_angle[X] = bomb_claw.target_half[X];
								 
			}break;
				
			case X_CLAW_BIT: //爪子伸出一丢丢
			{
				if(bomb_claw.flag_claw_bit[X_axis])
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X];	
					/*标志位处理*/
					bomb_claw.flag_claw_bit[X_axis] = DISABLE;				
					bomb_claw.flag_claw_half[X_axis] = ENABLE;
					bomb_claw.flag_claw_zero[X_axis] = ENABLE;
					bomb_claw.flag_claw_entire[X_axis] = ENABLE;
					
					bomb_claw.from_claw_bit[X_axis] = YES;
				}

				if((bomb_claw.target_angle[X] > bomb_claw.target_bit[X]) && (bomb_claw.from_claw_bit[X_axis] || bomb_claw.from_claw_entire[X_axis]))//从entire或是half来
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X] - 100;					
				}			
				else if((bomb_claw.target_angle[X] < bomb_claw.target_bit[X]) && bomb_claw.from_claw_zero[X_axis])//从zero来
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X] + 100;			
				}
				else bomb_claw.target_angle[X] = bomb_claw.target_bit[X];
								 
			}break;
			
			case X_CLAW_ZERO: //爪子归零
			{				
				if(bomb_claw.flag_claw_zero[X_axis])
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X];
					/*标志位处理*/
					bomb_claw.flag_claw_zero[X_axis] = DISABLE;
					bomb_claw.flag_claw_entire[X_axis] = ENABLE;
					bomb_claw.flag_claw_half[X_axis] = ENABLE;
					bomb_claw.flag_claw_bit[X_axis] = ENABLE;
					
					bomb_claw.from_claw_zero[X_axis] = YES;	
					bomb_claw.from_claw_half[X_axis] = NO;
					bomb_claw.from_claw_bit[X_axis] = NO;				
					bomb_claw.from_claw_entire[X_axis] = NO;

				}
				
				if(bomb_claw.target_angle[X] > 0)
				{
					bomb_claw.target_angle[X] = bomb_claw.total_angle[X] - 100;			
				}
				else bomb_claw.target_angle[X] = 0;
				
			}break;
			
			default: break;
		}  
	}	
	/*----------------------------------Y轴----------------------------------*/		
		
		switch(bomb_claw.Y_mode)
		{		
			case Y_CLAW_ENTIRE://爪子向右
			{									
				if(bomb_claw.flag_claw_entire[Y_axis])
				{
					bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y];	
					/*标志位处理*/
					bomb_claw.flag_claw_entire[Y_axis] = DISABLE;
					bomb_claw.flag_claw_zero[Y_axis] = ENABLE;
					bomb_claw.flag_claw_half[Y_axis] = ENABLE;
									
					bomb_claw.from_claw_entire[Y_axis] = YES;
					bomb_claw.from_claw_zero[Y_axis] = NO;
				}					
				
				if(bomb_claw.target_angle[Y] < bomb_claw.target_entire[Y])
				{
					if(get_bomb.ctrl_mode == PUSH_AIM_SCAN_MODE && climb_tube.location == ON_GROUND)
					{
						bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y] + 300;//岛下扫描速度
					}
					else if(get_bomb.ctrl_mode == PUSH_AIM_SCAN_MODE && climb_tube.location == ON_ISLAND)
					{
						if(optic_switch.value_start[M])
						{					
							bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y] + 1000;//岛上扫描速度（怼箱子上）
						}
						else bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y] + 300;//岛上扫描速度（怼缝里）
					}
					else bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y] + 4000;//取弹速度	
				}
				else bomb_claw.target_angle[Y] = bomb_claw.target_entire[Y];				
						
			}break;
			
			case Y_CLAW_HALF://爪子中间
			{
				if(bomb_claw.flag_claw_half[Y_axis])
				{
					bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y];
					/*标志位处理*/
					bomb_claw.flag_claw_half[Y_axis] = DISABLE;
					bomb_claw.flag_claw_zero[Y_axis] = ENABLE;
					bomb_claw.flag_claw_entire[Y_axis] = ENABLE;
				}

				if((bomb_claw.target_angle[Y] > bomb_claw.target_half[Y]) && bomb_claw.from_claw_entire[Y_axis])
				{
					bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y] - 4000;					
				}			
				else if((bomb_claw.target_angle[Y] < bomb_claw.target_half[Y]) && bomb_claw.from_claw_zero[Y_axis])
				{
					bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y] + 4000;			
				}
				else bomb_claw.target_angle[Y] = bomb_claw.target_half[Y];
					 
			}break;
				
			case Y_CLAW_ZERO://爪子向左
			{				
				if(bomb_claw.flag_claw_zero[Y_axis])
				{
					bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y];
					/*标志位处理*/
					bomb_claw.flag_claw_zero[Y_axis] = DISABLE;
					bomb_claw.flag_claw_entire[Y_axis] = ENABLE;
					bomb_claw.flag_claw_half[Y_axis] = ENABLE;
					
				  bomb_claw.from_claw_entire[Y_axis] = NO;
	  			bomb_claw.from_claw_zero[Y_axis] = YES;
				}
				
				if(bomb_claw.target_angle[Y] > bomb_claw.target_zero[Y])
				{
					bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y] - 4000;			
				}
				else bomb_claw.target_angle[Y] = bomb_claw.target_zero[Y];
							
			}break;
			
			case Y_CLAW_STOP://爪子原地停住
			{			
        bomb_claw.target_angle[Y] = bomb_claw.total_angle[Y];											
			}break;			
										
			default: break;
		}  	
//		/*被害妄想症*/
//		{
//			VAL_LIMIT(bomb_claw.target_angle[L], bomb_claw.target_entire[L], 0);
//			VAL_LIMIT(bomb_claw.target_angle[R], 0, bomb_claw.target_entire[R]);		
//			VAL_LIMIT(bomb_claw.target_angle[X], 0, bomb_claw.target_entire[X]);	
//			VAL_LIMIT(bomb_claw.target_angle[Y], 0, bomb_claw.target_entire[Y]);	  
//		}	
		
		/*给失控保护*/
		{
			if(system_mode == SAFETY_MODE)
			{
				memset(bomb_claw.current, 0, sizeof(bomb_claw.current));//这几个电机电流值清零
			}
			else 	bomb_claw_cas_ctrl(bomb_claw.target_angle);//串级计算
		}	
	}	
	
}

void bomb_claw_reset(void)
{
	if(!bomb_claw.ALL_reset)
	{
		for(u8 k = 0; k < 4; k++)
		{
			bomb_claw.target_speed[k] = bomb_claw.reset_speed[k] * ramp_calc(&reset_bomb_claw_spd_ramp);
		}
		bomb_claw_spd_ctrl(bomb_claw.target_speed);	
	}
		
	if(bomb_claw.reset_start_time_record)
	{
		bomb_claw.reset_start_time = millis();
		bomb_claw.ALL_reset = DISABLE;
		bomb_claw.LR_reset = DISABLE;
		bomb_claw.Y_reset = DISABLE;
		bomb_claw.reset_start_time_record = DISABLE;
	}
	
  if(millis() - bomb_claw.reset_start_time > 666)
	{
		if(!bomb_claw.LR_reset)
		{
			if((abs(bomb_claw.speed_fdb[L]) < 50) && 
				 (abs(bomb_claw.speed_fdb[R]) < 50) && 
				 (abs(bomb_claw.current[L]) > 5000) && 
				 (abs(bomb_claw.current[R]) > 5000))
			{
				if(bomb_claw.LR_stall_time_record)
				{
					bomb_claw.LR_stall_time = millis();
					bomb_claw.LR_stall_time_record = DISABLE;
				}
				if(millis() - bomb_claw.LR_stall_time > 100)
				{
					bomb_claw.LR_reset = ENABLE;
					bomb_claw.reset_speed[L] = 0;
					bomb_claw.reset_speed[R] = 0;				
				}		
			}
			/*一个电机坏了咋办*/
			else if(((abs(bomb_claw.speed_fdb[L]) < 33) && 
				       (abs(bomb_claw.current[L]) > 6666)) || 
		   	      ((abs(bomb_claw.speed_fdb[R]) < 33) && 
				       (abs(bomb_claw.current[R]) > 6666)))
			{
				if(bomb_claw.LR_stall_time_record)
				{
					bomb_claw.LR_stall_time = millis();
					bomb_claw.LR_stall_time_record = DISABLE;
				}
				if(millis() - bomb_claw.LR_stall_time > 888)
				{
					bomb_claw.LR_reset = ENABLE;
					bomb_claw.reset_speed[L] = 0;
					bomb_claw.reset_speed[R] = 0;				
				}	
			}
			else bomb_claw.LR_stall_time_record = ENABLE;
			

		}
		
		if(!bomb_claw.Y_reset)
		{
			if((abs(bomb_claw.speed_fdb[Y]) < 20) && 
				 (abs(bomb_claw.current[Y]) > 5000))
			{
				if(bomb_claw.Y_stall_time_record)
				{
					bomb_claw.Y_stall_time = millis();
					bomb_claw.Y_stall_time_record = DISABLE;
				}
				if(millis() - bomb_claw.Y_stall_time > 120)
				{
					bomb_claw.Y_reset = ENABLE;
					bomb_claw.reset_speed[Y] = 0;
				}		
			}
			else bomb_claw.Y_stall_time_record = ENABLE;
	  }
	}
	
	if(bomb_claw.LR_reset && bomb_claw.Y_reset)
	{
		bomb_claw.ALL_reset = ENABLE;
		bomb_claw.ZERO_time = millis();//记录复位好的时间延时一小段时间后切到Y_HALF
		
		bomb_claw.from_claw_entire[R_axis] = NO;
	  bomb_claw.from_claw_bit[R_axis] = NO;
	  bomb_claw.from_claw_half[R_axis] = NO;
	  bomb_claw.from_claw_zero[R_axis] = YES;
		
	  bomb_claw.from_claw_entire[Y_axis] = NO;
	  bomb_claw.from_claw_zero[Y_axis] = YES;
//		
//		BOMB_CLAW_MOVE_LEFT;
//		BOMB_CLAW_ROLL_ZERO
		
//		for(u8 k = 0; k < 4; k++)//四个电机一套PID
//		{
//			pidckz_reset(&pid_bomb_claw_position[k]);
//			pidckz_reset(&pid_bomb_claw_speed[k]);
//		
//			pidckz_param_set(&pid_bomb_claw_position[k], 10000, 1000, 6.5f, 0.0f, 0.0f);            //拨盘电机位置环
//			pidckz_param_set(&pid_bomb_claw_speed[k], 10000, 1000, 15.0f, 0.0f, 0.0f);	            //拨盘电机速度环	
//		}
		
		
	}
	else bomb_claw.ALL_reset = DISABLE;
		
}

void bomb_claw_task(void)
{
	if(bomb_claw.ALL_reset)
	{
		bomb_claw_ctrl();
	}	
	else if(lifter.flag_bomb_claw_reset)
	{
	  bomb_claw_reset();
	}
	
	if(bomb_claw.ALL_reset && 
		(millis() - bomb_claw.ZERO_time > 24) && 
	  (MOTORS_ALL_READY == DISABLE))
	{		
		BOMB_CLAW_ROLL_ENTIRE;                   //ENTIRE位为收起位
		BOMB_CLAW_MOVE_MID;                      //取弹爪子平移至中

		if((abs(bomb_claw.target_half[Y] - bomb_claw.total_angle[Y]) < 100) &&
			((abs(bomb_claw.target_entire[L] - bomb_claw.total_angle[L]) < 100) ||
		   (abs(bomb_claw.target_entire[R] - bomb_claw.total_angle[R]) < 100)))		
		{
			MOTORS_ALL_READY = ENABLE;		
		}		
	
	}

}

