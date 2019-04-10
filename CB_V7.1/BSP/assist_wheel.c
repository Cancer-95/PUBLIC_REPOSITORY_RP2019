#include "assist_wheel.h"

assist_wheel_t assist_wheel;

void assist_wheel_param_init(void)
{
	memset(&assist_wheel, 0, sizeof(assist_wheel_t));
	
	pidckz_reset(&pid_fric_position);
	pidckz_reset(&pid_fric_speed);
	
	pidckz_param_set(&pid_fric_position, 8000, 1000, 8.5f, 0.0f, 0.0f);            //拨盘电机位置环
	pidckz_param_set(&pid_fric_speed, 8000, 1000, 7.5f, 0.04f, 5.0f);	            //拨盘电机速度环
}

void assist_wheel_spd_ctrl(int16_t target_speed)
{
	assist_wheel.current = pidckz_calc(&pid_fric_speed, assist_wheel.wheel_speed_fdb, target_speed);         //PID计算
}	

void assist_wheel_cas_ctrl(int32_t target_angle)
{	
	assist_wheel.wheel_speed_ref = pidckz_calc(&pid_fric_position, assist_wheel.total_angle, target_angle);           //外环(位置环)PID计算
	assist_wheel.current = pidckz_calc(&pid_fric_speed, assist_wheel.wheel_speed_fdb, assist_wheel.wheel_speed_ref);         //PID计算
}

void assist_wheel_data_handler(void)//该函数最好置于最短的时间戳 以防记不到圈数 置于2ms都会出问题
{	
	static bool assist_wheel_ecd_Record = 1;
	if(assist_wheel_ecd_Record)
	{
		assist_wheel_ecd_Record = 0;
		assist_wheel.offset_ecd = assist_wheel.wheel_angle_fdb;  //只记录一次初始偏差
	}
	
		assist_wheel.las_ecd = assist_wheel.cur_ecd;
		assist_wheel.cur_ecd = assist_wheel.wheel_angle_fdb;
	
		if(assist_wheel.cur_ecd - assist_wheel.las_ecd > 4096)
		{
			assist_wheel.round_cnt--;
		}
		else if(assist_wheel.cur_ecd - assist_wheel.las_ecd < -4096)
		{
			assist_wheel.round_cnt++;
		}
	
		assist_wheel.total_ecd = assist_wheel.round_cnt * 8192 + assist_wheel.cur_ecd - assist_wheel.offset_ecd;
		assist_wheel.total_angle = assist_wheel.total_ecd / ENCODER_ANGLE_RATIO;//度数换算 最后出来的东西是转子的转动总角度
}


void assist_wheel_task()
{
	static bool flag_assist = 1;	
	switch(assist_wheel.ctrl_mode)
	{
		case WHEEL_FORWARD:
		{
		 flag_assist = 1;
		 assist_wheel_spd_ctrl(-2000);
		}break;
		
		case WHEEL_FORWARD_SLOW:
		{
		 flag_assist = 1;
		 assist_wheel_spd_ctrl(100);
		}break;
		
		case WHEEL_HOLD:
		{
			if(flag_assist)
			{
        assist_wheel.target_angle = assist_wheel.total_angle; 					
				flag_assist = 0;
			}
			assist_wheel_cas_ctrl(assist_wheel.target_angle);	//除连射外执行串级
		}break;
			
		case WHEEL_REVERSE:
		{
		 flag_assist = 1;
		 assist_wheel_spd_ctrl(2000);
		}break;
		
		case WHEEL_REVERSE_SLOW:
		{
		 flag_assist = 1;
		 assist_wheel_spd_ctrl(-100);
		}break;
				
		case WHEEL_RELAX:
		{
     assist_wheel.current = 0;
		}break;
		
		default:break;//Default do nothing
	}   	
}	

