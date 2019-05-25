#include "modeswitch.h"

GIMBAL_MODE gimbal_mode;
CHASSIS_MODE chassis_mode;
FRIC_MODE fric_mode;
SYSTEM_MODE system_mode;

u8 Key_MG_flag = 1;
bool Timeout_Tri_CTRL = 1;
u8 Key_DD_flag = 0;
bool Timeout_Tri_Q = 1;

void Ctrl_Mode_Select()//拨杆S1的模式切换
{
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
			/*键盘切换机械陀螺仪模式*/
			if(KEY_CTRL_PRESSED  && Timeout_Tri_CTRL == 1)
			{
				Timeout_Tri_CTRL = 0;
				Key_MG_flag ++;
				Key_MG_flag %= 2;
			}
			else if(KEY_CTRL_NOT_PRESSED) Timeout_Tri_CTRL = 1;
		
			if(Key_MG_flag)
			{
				system_mode = KEY_MACHINE_MODE;
			}
			else
			{
				system_mode = KEY_GYRO_MODE;
			}
			
			/*键盘进入扭腰模式*/
			if(KEY_Q_PRESSED  && Timeout_Tri_Q == 1)
			{
				Timeout_Tri_Q = 0;
				Key_DD_flag ++;
				Key_DD_flag %= 2;
			}
			else if(KEY_Q_NOT_PRESSED) Timeout_Tri_Q = 1;
			
			if(Key_DD_flag)
			{
				system_mode = KEY_DODGE_MODE;
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
		  gimbal_mode = GIMBAL_MEC;
			chassis_mode = MANUAL_FOLLOW_CHASSIS;//机械模式云台跟随底盘
			fric_mode = FRIC_WHEEL_OFF;	
		}break;
		
		case RC_GYRO_MODE://遥控陀螺仪模式
		{
		  gimbal_mode = GIMBAL_GYO;	
			fric_mode = FRIC_WHEEL_RUN;
			
			if(RC_Ctl.rc.s2 == RC_SW_DOWN) //遥控器陀螺仪模式下S2下拨
			{
				chassis_mode = DODGE_MODE;
			}
			else 
			{
		   	chassis_mode = MANUAL_FOLLOW_GIMBAL;//陀螺仪模式底盘跟随云台
			}

		}break;
		
		case KEY_MACHINE_MODE://键盘机械模式
		{
		  gimbal_mode = GIMBAL_MEC;
			chassis_mode = MANUAL_FOLLOW_CHASSIS;//机械模式云台跟随底盘
			fric_mode = FRIC_WHEEL_OFF;	
		}break;
		
		case KEY_GYRO_MODE://键盘陀螺仪模式
		{
		  gimbal_mode = GIMBAL_GYO;	
			chassis_mode = MANUAL_FOLLOW_GIMBAL;//陀螺仪模式底盘跟随云台
			fric_mode = FRIC_WHEEL_RUN;
		}break;
		
		case KEY_DODGE_MODE://键盘扭腰
		{  
	  	gimbal_mode = GIMBAL_GYO;	
			chassis_mode = DODGE_MODE;//陀螺仪模式底盘跟随云台
			fric_mode = FRIC_WHEEL_OFF;
		}break;
		
		case SAFETY_MODE://安全模式
		{
			gimbal_mode = GIMBAL_STO;
			chassis_mode = CHASSIS_STOP;
			fric_mode = FRIC_WHEEL_OFF;	
			

			
		}break;
		
		default:		
		break;
	
	}
	
	
	
	
	
	
	
}

