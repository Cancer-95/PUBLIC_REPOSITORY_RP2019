#include "gimbal_ctrl.h"

gimbal_t gim;

float  Pitch_Target[2] = {Pitch_M_MID, Pitch_G_MID}, Yaw_Target[2] = {Yaw_M_MID, Yaw_G_MID};

void Gimbal_Init()//Gimbal的PID参数初始化
{
	PIDCKZ_param_clear(&pid_yaw_M_angle);
	PIDCKZ_param_clear(&pid_yaw_M_speed);
	PIDCKZ_param_clear(&pid_pit_M_angle);
	PIDCKZ_param_clear(&pid_pit_M_speed);
	
	memset(&gim, 0, sizeof(gimbal_t));
	
	PIDCKZ_param_set(&pid_yaw_M_speed, 5000, 3000, 5.0f, 0.0f, 0.0f);        //机械模式YAW内环参数设定
	PIDCKZ_param_set(&pid_pit_M_speed, 5000, 3000, 3.0f, 0.0f, 0.0f);        //机械模式PIT内环参数设定
	PIDCKZ_param_set(&pid_yaw_M_angle, 5000, 1000, 5.0f, 0.0f, 0.0f);	       //机械模式YAW外环参数设定
	PIDCKZ_param_set(&pid_pit_M_angle, 5000, 1000, 6.0f, 0.0f, 0.0f);        //机械模式PIT外环参数设定
	
	PIDCKZ_param_set(&pid_yaw_G_speed, 5000, 3000, 8.0f, 0.0f, 0.0f);        //机械模式YAW内环参数设定
	PIDCKZ_param_set(&pid_pit_G_speed, 5000, 3000, 5.0f, 0.0f, 0.0f);        //机械模式PIT内环参数设定
	PIDCKZ_param_set(&pid_yaw_G_angle, 5000, 1000, 150.0f, 0.0f, 0.0f);	       //机械模式YAW外环参数设定
	PIDCKZ_param_set(&pid_pit_G_angle, 5000, 1000, 150.0f, 0.0f, 0.0f);        //机械模式PIT外环参数设定
	
	}
void Gimbal_Output_Stop()//无输出停止状态
{
  pid_yaw_M_speed.Out = pid_pit_M_speed.Out = 0;
	pid_yaw_G_speed.Out = pid_pit_G_speed.Out = 0;		
	CAN1_Send(0x1ff,0,0,0,0);               
}
u8 Turn_Around_flag;	
void Gimbal_Turn_Around()//键盘按C掉头
{
/*注释掉的掉头程序是一次性加的180度 往往会碰到限位 故加上累加延迟*/
//	if(KEY_C_PRESSED && Turn_Around_flag)
//	{
//		Yaw_Target[GYO] += 180;
//		Turn_Around_flag = 0;
//	}
//	else if(KEY_C_NOT_PRESSED) Turn_Around_flag = 1;
	if(KEY_C_PRESSED)
	{
    Turn_Around_flag = 1;
	}
	else if(KEY_C_NOT_PRESSED && Turn_Around_flag !=0) 
	{
		Yaw_Target[GYO] += 0.72f;
		Turn_Around_flag++;
		if(Turn_Around_flag >= 250)
		Turn_Around_flag = 0;
	}
}

void Get_MEC_Target()//获取P轴Y轴的目标值
{
	Yaw_Target[MEC] = Yaw_M_MID;//机械模式下给定值锁着就行了
	
	if(system_mode == RC_MACHINE_MODE)
	{
		Pitch_Target[MEC] += -(RC_Ctl.rc.ch1-RC_CH_VALUE_OFFSET)*0.01;
	}
	else if(system_mode == KEY_MACHINE_MODE)
	{
		Pitch_Target[MEC] += RC_Ctl.mouse.y * 0.25;	
	}
	
	Pitch_Target[MEC]  = constrain(Pitch_Target[MEC],Pitch_M_MIN,Pitch_M_MAX);

}
void Get_GYO_Target()//获取P轴Y轴的目标值
{	 
	if(system_mode == RC_GYRO_MODE)
	{
		Yaw_Target[GYO]   += -(RC_Ctl.rc.ch0-RC_CH_VALUE_OFFSET)*0.0008;
		Pitch_Target[GYO] += -(RC_Ctl.rc.ch1-RC_CH_VALUE_OFFSET)*0.0005;
	}
	else if(system_mode == KEY_GYRO_MODE || system_mode == KEY_DODGE_MODE)
	{
		Gimbal_Turn_Around();//键盘模式下掉头判断
	  Yaw_Target[GYO]   += -RC_Ctl.mouse.x * 0.01;
		Pitch_Target[GYO] +=  RC_Ctl.mouse.y * 0.01;
	}

	Pitch_Target[GYO]  = constrain(Pitch_Target[GYO],Pitch_G_MIN,Pitch_G_MAX);	
		
/*YAW轴目标值确保越过临界值*/	
	if(Yaw_Target[GYO] >= 180)Yaw_Target[GYO] = Yaw_Target[GYO]  - 360;
	else if(Yaw_Target[GYO] <= -180)Yaw_Target[GYO] = 360 + Yaw_Target[GYO];
}

void Gimbal_M_Control_Outter()//机械模式外环
{
	//yaw串级外环
  gim.yaw_angle_ref=Yaw_Target[MEC];                                            //外环(角度环)目标值
	gim.yaw_angle_fdb=Motor_angle[YAW];                                           //外环(角度环)反馈值
	PIDCKZ_Calc(&pid_yaw_M_angle, gim.yaw_angle_fdb, gim.yaw_angle_ref);    //PID计算得到外环输出(以角速度形式输出)	
	//pit串级外环
	gim.pit_angle_ref=Pitch_Target[MEC];                                          //外环(角度环)目标值
	gim.pit_angle_fdb=Motor_angle[PIT];                                           //外环(角度环)反馈值
	PIDCKZ_Calc(&pid_pit_M_angle, gim.pit_angle_fdb, gim.pit_angle_ref);    //PID计算得到外环输出(以角速度形式输出)	
}

void Gimbal_M_Control_Inner()//机械模式内环
{
	//yaw串级内环
	gim.yaw_speed_ref = pid_yaw_M_angle.Out;                                //外环输出作为目标值
	gim.yaw_speed_fdb = imu.gz;                                             //读取陀螺仪的角速度值
	PIDCKZ_Calc(&pid_yaw_M_speed, gim.yaw_speed_ref, gim.yaw_speed_fdb);    //PID计算得到最终输出(注意谁减谁)
	//pit串级内环
	gim.pit_speed_ref = pid_pit_M_angle.Out;                                //外环输出作为目标值
	gim.pit_speed_fdb = imu.gx;                                             //读取陀螺仪的角速度值
	PIDCKZ_Calc(&pid_pit_M_speed, gim.pit_speed_ref, gim.pit_speed_fdb);    //PID计算得到最终输出(注意谁减谁)

//	Moving_Average_Filter(pid_yaw_M_speed.Out, 4, &pid_yaw_M_speed.Out);     //滑动平均滤波 失败了
//	Moving_Average_Filter(pid_pit_M_speed.Out, 4, &pid_pit_M_speed.Out);     //滑动平均滤波 失败了
	CAN1_Send(0x1ff,pid_yaw_M_speed.Out,pid_pit_M_speed.Out,0,0);           //can1发送电流值
}


void Gimbal_G_Control_Outter()//陀螺仪模式外环
{
	//yaw串级
  gim.yaw_angle_ref=Yaw_Target[GYO];                                      //外环(角度环)目标值
	gim.yaw_angle_fdb=imu.yaw;                                              //获取陀螺仪角度作为当前角度
	Critical_Value_Treatment(gim.yaw_angle_fdb, &gim.yaw_angle_ref,0,360);    //临界值处理
	PIDCKZ_Calc(&pid_yaw_G_angle, gim.yaw_angle_fdb, gim.yaw_angle_ref);    //PID计算得到外环输出(以角速度形式输出)	
	//pit串级
	gim.pit_angle_ref=Pitch_Target[GYO];                                    //外环(角度环)目标值
	gim.pit_angle_fdb=imu.pit;                                              //获取陀螺仪角度作为当前角度
	Critical_Value_Treatment(gim.pit_angle_fdb, &gim.pit_angle_ref,0,360);    //临界值处理
	PIDCKZ_Calc(&pid_pit_G_angle, gim.pit_angle_fdb, gim.pit_angle_ref);    //PID计算得到外环输出(以角速度形式输出)	
}

void Gimbal_G_Control_Inner()//陀螺仪模式内环
{
	//yaw串级
	gim.yaw_speed_ref = pid_yaw_G_angle.Out;                                //外环输出作为目标值
	gim.yaw_speed_fdb = imu.gz;                                             //读取陀螺仪的角速度值
	PIDCKZ_Calc(&pid_yaw_G_speed, gim.yaw_speed_ref, gim.yaw_speed_fdb);    //PID计算得到最终输出(注意谁减谁)
	//pit串级
	gim.pit_speed_ref = pid_pit_G_angle.Out;                                //外环输出作为目标值
	gim.pit_speed_fdb = imu.gx;                                             //读取陀螺仪的角速度值
	PIDCKZ_Calc(&pid_pit_G_speed, gim.pit_speed_ref, gim.pit_speed_fdb);    //PID计算得到最终输出(注意谁减谁)

//	Moving_Average_Filter(pid_yaw_G_speed.Out, 4, &pid_yaw_G_speed.Out);     //滑动平均滤波 失败了
//	Moving_Average_Filter(pid_pit_G_speed.Out, 4, &pid_pit_G_speed.Out);     //滑动平均滤波 失败了
	CAN1_Send(0x1ff,pid_yaw_G_speed.Out,pid_pit_G_speed.Out,0,0);           //can1发送电流值
}
void Gimbal_M_Control()//机械模式内外环合并
{
  Yaw_Target[GYO] = imu.yaw;      //记录陀螺仪模式陀螺仪角度
	Pitch_Target[GYO] = imu.pit;    //记录陀螺仪模式陀螺仪角度
	Get_MEC_Target();                     //获取云台目标值
	Gimbal_M_Control_Outter();            //机械模式外环
	Gimbal_M_Control_Inner();             //机械模式内环
}
void Gimbal_G_Control()//陀螺仪模式内外环合并
{	
	Yaw_Target[MEC] = Motor_angle[YAW];   //记录机械模式机械角
	Pitch_Target[MEC] = Motor_angle[PIT]; //记录机械模式机械角
	Get_GYO_Target();                     //获取云台目标值	
	Gimbal_G_Control_Outter();            //陀螺仪模式外环
	Gimbal_G_Control_Inner();             //陀螺仪模式内环
}
void  Gimbal_Control()
{
  gim.yaw_relative_angle = ( Motor_angle[YAW] - Yaw_M_MID ) / ENCODER_ANGLE_RATIO;//记录实时的相对角度
	gim.pit_relative_angle = ( Motor_angle[PIT] - Pitch_M_MID ) / ENCODER_ANGLE_RATIO;//记录实时的相对角度
	
	if((Motor_angle[YAW] <= Yaw_M_MIN && (RC_Ctl.mouse.x > 0 || RC_Ctl.rc.ch0 > RC_CH_VALUE_OFFSET))	//限制云台与底盘分离角度
		 ||(Motor_angle[YAW] >= Yaw_M_MAX && (RC_Ctl.mouse.x < 0 || RC_Ctl.rc.ch0 < RC_CH_VALUE_OFFSET)))
	{
		RC_Ctl.mouse.x = 0;
		RC_Ctl.rc.ch0 = RC_CH_VALUE_OFFSET;
	}
	
	switch (gimbal_mode)
  {
		case GIMBAL_STO:												//云台不输出																								
		{
			Gimbal_Output_Stop();
		}break;
		case GIMBAL_MEC:												//云台机械模式																											
		{
			Gimbal_M_Control();
		}break;
		case GIMBAL_GYO:												//云台陀螺仪模式																
		{
			Gimbal_G_Control();
		}break;
		default:																																															
		{
			Gimbal_Output_Stop();
		}break;
	}
}




