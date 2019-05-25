#include "bsp_sys.h"

//以微秒为单位返回系统时间
uint32_t micros(void)
{
	register uint32_t ms, cycle_cnt;
	do {
			ms = HAL_GetTick();
			cycle_cnt = SysTick->VAL;
	} while (ms != HAL_GetTick());
	return (ms * 1000) + (168 * 1000 - cycle_cnt) / 168;
}

//微秒级延时
void delay_us(uint32_t us)
{
	uint32_t now = micros();
	while (micros() - now < us);
}

//毫秒级延时
void delay_ms(uint32_t ms)
{
	while (ms--)
	delay_us(1000);
}

//以毫秒为单位返回系统时间其实就是HAL_GetTick();
uint32_t millis(void)
{
	return HAL_GetTick();
}

//主循环
void loop(void)
{	
	static uint32_t currentTime = 0;    
	static uint32_t loopTime_1ms = 0;   //模式、发射、底盘
  static uint32_t loopTime_3ms = 0;   //陀螺仪云台
	static uint32_t loopTime_5ms = 0;   //可以放各种乱七八糟的测试
  static uint32_t loopTime_20ms = 0;  //丢控保护
	static uint32_t loopTime_50ms = 0;  //反馈Roboremo
	static uint32_t loopTime_100ms = 0; //发送数据给裁判系统
	static uint32_t loopTime_200ms = 0; //LED指示

//	static u8 printf_delay = 0; //roboremo的printf轮流
	
	currentTime = micros();	//获取当前系统时间
		
	if((int32_t)(currentTime - loopTime_1ms) >= 0)  
	{	
		loopTime_1ms = currentTime + 1000;	//1ms		
		
		chassis_task();  
//    lifter_task();
//		assist_wheel_task();
//		trailer_task();
		servo_task();
//		CCTV_switch();
//	  bomb_claw_task();	

//		/*CAN1发送电流*/	
//	  CAN1_send(0x200,chassis.current[M1],chassis.current[M2],chassis.current[M3],chassis.current[M4]);
//		CAN1_send(0X1FF, lifter.current[L], lifter.current[R], 0, 0);
		send_chassis_cur(chassis.current[M1], chassis.current[M2], chassis.current[M3], chassis.current[M4]);
//		/*CAN2发送电流*/
//		CAN2_send(0X200, assist_wheel.current, trailer.current, 0, bomb_claw.current[L]);      
//	  CAN2_send(0X1FF, bomb_claw.current[R], bomb_claw.current[X], bomb_claw.current[Y], 0);  
  	
   
	}
	
	if((int32_t)(currentTime - loopTime_3ms) >= 0)  
	{			
		loopTime_3ms = currentTime + 3000;	//3ms								
    mpu_get_data();	
//		
//		mode_switch_task(); 
		optoelectronic_switch_task();
//    climb_tube_task();	
//		get_bomb_task();
//		rescue_task();
//		feed_bomb_task();
	}
		
	if((int32_t)(currentTime - loopTime_5ms) >= 0)  
	{			
		loopTime_5ms = currentTime + 5000;	//5ms	
  

//		ducted_task();
////		claw_task();
////		ANO_TC_Report16(PowerProtect.Judge_power*100);//Test-给匿名发送实时功率
////		ANO_TC_Report(0,0,0,imu.gx,imu.gy,imu.gz,imu.roll,imu.pit,imu.yaw);//Test-给匿名发陀螺仪数据
	}
		
	if((int32_t)(currentTime - loopTime_20ms) >= 0)  
	{			
		loopTime_20ms = currentTime + 20000;	//20ms	
		    
//		RC_offline_handler();//丢控保护
	}
	
	if((int32_t)(currentTime - loopTime_50ms) >= 0)  
	{			
    loopTime_50ms = currentTime + 50000;    //50ms 这个时间戳是给roboremo_printf准备
    imu_temp_keep();
		
//		sys_check();
		/*
	 	*用于确认参数修改成功
		*执行两条printf间隔不能太短
		*/
//		printf_delay ++;
//		switch(printf_delay)
//		{
//			case 1: Roboremo_printf("IN_p %.3f\n",roboremo_pid.Inner_p);   break;        //pid设定参数传回roboremo
//			case 2: Roboremo_printf("IN_i %.3f\n",roboremo_pid.Inner_i);   break;
//			case 3: Roboremo_printf("IN_d %.3f\n",roboremo_pid.Inner_d);   break;
//			case 4: Roboremo_printf("OUT_p %.3f\n",roboremo_pid.Outter_p); break;
//			case 5: Roboremo_printf("OUT_i %.3f\n",roboremo_pid.Outter_i); break;
//			case 6: Roboremo_printf("OUT_d %.3f\n",roboremo_pid.Outter_d); break;
//			
//			case 7: Roboremo_printf("TT_L %.1f\n",roboremo_power.Total_Limit); break;     //功率设定参数传回roboremo
//			case 8: Roboremo_printf("PW_T %.1f\n",roboremo_power.Power_Threshold); break;
//			
//			default:printf_delay = 0; break;
//		}
	}
	
	if((int32_t)(currentTime - loopTime_100ms) >= 0)  
	{			
		loopTime_100ms = currentTime + 100000;	//100ms	
		
//		Send_Data_To_Judgement();//发送自定义数据裁判系统
	}
	
	if((int32_t)(currentTime - loopTime_200ms) >= 0)  
	{			
		loopTime_200ms = currentTime + 200000;	//200ms	
		led_task();

	}
	

}



