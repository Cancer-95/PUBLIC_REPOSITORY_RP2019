#include "system.h"

//计数器初始化
static volatile uint32_t usTicks = 0;
static void cycleCounterInit(void)
{
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	usTicks = clocks.SYSCLK_Frequency / 1000000; 
}

//以微秒为单位返回系统时间
uint32_t micros(void)
{
	register uint32_t ms, cycle_cnt;
	do {
			ms = sysTickUptime;
			cycle_cnt = SysTick->VAL;
	} while (ms != sysTickUptime);
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
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

//以毫秒为单位返回系统时间
uint32_t millis(void)
{
	return sysTickUptime;
}

/*--------------------------------------系统初始化--------------------------------------------*/
void system_init(void)
{	
	cycleCounterInit();
	SysTick_Config(SystemCoreClock / 1000);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//设置系统中断优先级分组2 （4组Pre 4组Sub）
	
	judgement_init();    			//裁判系统 uart5初始化  p.s. system.h中可注释宏定义，切换成uart5
	CRC_init();               //CRC校验初始化	
	
	vision_init();             //视觉接口uart4
	
/*执行元件初始化1*/  	
	RC_init();
	led_init();
  servo_init();
	relay_init();//继电器
	ducted_init();//涵道 
	
//	ANO_TC_Init();
//	Roboremo_Init();

/**下面这个延时很重要 若没有这句陀螺仪就会出问题了 因为这里并没有调用MPU_Init() 而是在下面直接mpu_dmp_init()进行dmp初始化配置 
	*因为俩个Init中都有IIC的初始化 MPU_Init()中有很多工作重复 但是mpu_dmp_init()少了很关键的100ms延时
	*因为一般传感器芯片甚至是STM32内部都有ADC ADC从上电到稳定需要时间
	*/
	delay_ms(1000);
/*系统检查初始化*/
	sys_check_init();
    
/*执行元件初始化2*/ 
	chassis_param_init();//底盘电机
  lifter_param_init();//抬升电机	
	assist_wheel_param_init();//辅助摩擦轮电机
	trailer_param_init();//拖车电机
	optoelectronic_switch_init();//光电开关
	bomb_claw_param_init();//取弹机构几个电机
/*任务参数初始化*/	
  global_task_init();//任务清零任务初始化
//  vision_param_init();//视觉初始化
	climb_tube_param_init();//爬杆初始化
  get_bomb_param_init();//取弹初始化	
	rescue_param_init();//救援初始化
	feed_bomb_param_init();//补弹初始化
	
	CAN1_init();	
	CAN2_init();
	
	MPU_Init();
	MPU_tempPID_init();
	imu_param_init();
	
	while(mpu_dmp_init())
	{
		static u8 fail_time = 0;
		fail_time++;
		delay_ms(100);
		if(fail_time>3)
		{
			mpu_set_dmp_state(1);	
			break;
		}
	}
	mpu_offset_cal();//用于计算陀螺仪固有OFFECT 在MPU_Get_Gyroscope()函数中减掉固有偏差后注释掉即可
	delay_ms(1000);//这个延时纯属瞎搞

	
}

/*--------------------------------------主循环--------------------------------------------*/
uint32_t Overflow_check[3]={0};
bool MOTORS_ALL_READY = 0;
void loop(void)
{	
	static uint32_t currentTime = 0;    
	static uint32_t loopTime_1ms = 0;   //模式、发射、底盘
  static uint32_t loopTime_3ms = 0;   //陀螺仪云台
	static uint32_t loopTime_5ms = 0;   //可以放各种乱七八糟的测试
  static uint32_t loopTime_20ms = 0;  //丢控保护
	static uint32_t loopTime_30ms = 0;  //发东西给妙算
	static uint32_t loopTime_50ms = 0;  //反馈Roboremo
	static uint32_t loopTime_100ms = 0; //发送数据给裁判系统
	static uint32_t loopTime_200ms = 0; //LED指示

//	static u8 printf_delay = 0; //roboremo的printf轮流
	
	currentTime = micros();	//获取当前系统时间
		
	if((int32_t)(currentTime - loopTime_1ms) >= 0)  
	{	
		loopTime_1ms = currentTime + 1000;	//1ms		
		Overflow_check[0] +=1;
		
		chassis_task();  
    lifter_task();
//		assist_wheel_task();
//		servo_task();

		/*CAN1发送电流*/	
	  CAN1_send(0x200, chassis.current[M1], chassis.current[M2], chassis.current[M3], chassis.current[M4]);
		CAN1_send(0X1FF, lifter.current[L], lifter.current[R], 0, 0);
//		/*CAN2发送电流*/
//		CAN2_send(0X200, bomb_claw.current[Y], trailer.current, 0, 0);  
//		CAN2_send(0X1FF, bomb_claw.current[L], bomb_claw.current[R], 0, 0);
		optic_scan_task();		
    
	}
	
	if((int32_t)(currentTime - loopTime_3ms) >= 0)  
	{			
		loopTime_3ms = currentTime + 3000;	//3ms	
    Overflow_check[1] +=3;
		
		trailer_task();
		bomb_claw_task();	
//		CAN2_send(0X200, 0, trailer.current, 0, bomb_claw.current[L]);      
//	  CAN2_send(0X1FF, bomb_claw.current[R], 0, bomb_claw.current[Y], 0);
		CAN2_send(0X200, bomb_claw.current[L], bomb_claw.current[R], bomb_claw.current[Y], trailer.current);      
		
		
		
		vision_task();
		
		mode_switch_task(); //模式选择			
		mpu_get_data();		
    
		optoelectronic_switch_task();	
	  CCTV_switch_task();
		
		if(MOTORS_ALL_READY)
		{
			climb_tube_task();
			get_bomb_task();		
			rescue_task();
			feed_bomb_task();				
		}
    

	}
		
	if((int32_t)(currentTime - loopTime_5ms) >= 0)  
	{			
		loopTime_5ms = currentTime + 5000;	//5ms	
	  Overflow_check[2] +=5;
	
		ducted_task();
//		ANO_TC_Report16(PowerProtect.Judge_power*100);//Test-给匿名发送实时功率		
//		ANO_TC_Report(0,0,0,imu.gx,imu.gy,imu.gz,imu.roll,imu.pit,imu.yaw);//Test-给匿名发陀螺仪数据
	}
		
	if((int32_t)(currentTime - loopTime_20ms) >= 0)  
	{			
		loopTime_20ms = currentTime + 20000;	//20ms	
				
		RC_offline_handler();//丢控保护
	}
	
	if((int32_t)(currentTime - loopTime_30ms) >= 0)  
	{			
		loopTime_30ms = currentTime + 30000;	//30ms	

		vision_send_task();//给视觉发东西
	}
	
	if((int32_t)(currentTime - loopTime_50ms) >= 0)  
	{			
    loopTime_50ms = currentTime + 50000;    //50ms 这个时间戳是给roboremo_printf准备
	 
    Tempeture_PID();
//		imu_temp_keep();//Cancer自己写的温飘比较难调		
		sys_check();//系统检查
		/*
	 	*用于确认参数修改成功
		*执行两条printf间隔不能不能太短
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

		judgement_send_task();//给裁判系统发东西
	}
	
	if((int32_t)(currentTime - loopTime_200ms) >= 0)  
	{			
		loopTime_200ms = currentTime + 200000;	//200ms	
		
		/*板载LED指示*/
    led_task();
	}

}








