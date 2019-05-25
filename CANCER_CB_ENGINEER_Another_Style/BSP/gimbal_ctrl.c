#include "gimbal_ctrl.h"






void Gimbal_G_Control()//陀螺仪模式内外环合并
{	
	Get_GYO_Target();                     //获取云台目标值	
	Gimbal_G_Control_Outter();            //陀螺仪模式外环
	Gimbal_G_Control_Inner();             //陀螺仪模式内环
}
void  Gimbal_Control()
{
//  chassis.yaw_relative_angle = ( Motor_angle[YAW] - Yaw_M_MID ) / ENCODER_ANGLE_RATIO;//记录实时的相对角度

	
	switch (gimbal_mode)
  {

		case GIMBAL_GYO:												//云台陀螺仪模式																
		{
			Gimbal_G_Control();
		}break;
		default:break;
	}
}




