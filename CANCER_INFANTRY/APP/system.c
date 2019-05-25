#include "system.h"

static volatile uint32_t usTicks = 0;

uint32_t currentTime = 0;

uint32_t previousTime = 0;
uint16_t cycleTime = 0; 

//限幅
float constrain(float amt, float low, float high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}
int32_t constrain_int32(int32_t amt, int32_t low, int32_t high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}

int16_t constrain_int16(int16_t amt, int16_t low, int16_t high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}

int constrain_int(int amt,int low,int high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}

//计数器初始化
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

//系统初始化
void System_Init(void)
{	
	cycleCounterInit();
	SysTick_Config(SystemCoreClock / 1000);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//设置系统中断优先级分组2 （4组Pre 4组Sub）

	Judgement_Init();    			//裁判系统 uart4初始化  p.s. system.h中可注释宏定义，切换成uart5
	CRC_init();               //CRC校验初始化	
	
	RC_Init();
	Led_Init();
	
	PWM_Init();
	CAN1_Init();
	CAN2_Init();
	ANO_TC_Init();
	Roboremo_Init();

	delay_ms(200);
/**上面这个延时很重要 若没有这句陀螺仪就会出问题了 因为这里并没有调用MPU_Init() 而是在下面直接mpu_dmp_init()进行dmp初始化配置 
	*因为俩个Init中都有IIC的初始化 MPU_Init()中有很多工作重复 但是mpu_dmp_init()少了很关键的100ms延时
	*因为一般传感器芯片甚至是STM32内部都有ADC ADC从上电到稳定需要时间
	*/
  Shoot_Init();
	Chassis_Init();	
	Gimbal_Init();

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
//	mpu_offset_cal();     //用于计算陀螺仪固有OFFECT 在MPU_Get_Gyroscope()函数中减掉固有偏差后注释掉即可
	delay_ms(1500);//这个延时纯属瞎搞
}

//主循环
void Loop(void)
{	
	static uint32_t currentTime = 0;    
	static uint32_t loopTime_1ms = 0;   //模式、发射、底盘
  static uint32_t loopTime_2ms = 0;   //陀螺仪云台
	static uint32_t loopTime_5ms = 0;   //可以放各种乱七八糟的测试
  static uint32_t loopTime_20ms = 0;  //丢控保护
	static uint32_t loopTime_50ms = 0;  //反馈Roboremo
	static uint32_t loopTime_100ms = 0; //发送数据给裁判系统

	static u8 printf_delay = 0; //roboremo的printf轮流
	
	currentTime = micros();	//获取当前系统时间
	
		
	if((int32_t)(currentTime - loopTime_1ms) >= 0)  
	{	
		loopTime_1ms = currentTime + 1000;	//1ms		
		
		Ctrl_Mode_Select(); //模式选择		
		Shoot_Remote_Ctrl();//发射控制
		Chassis_Control();  //底盘控制
		

	}
	
	if((int32_t)(currentTime - loopTime_2ms) >= 0)  
	{			
		loopTime_2ms = currentTime + 2000;	//2ms	
					
		MPU_Get_Gyroscope(&imu.gx,&imu.gy,&imu.gz);       	//读取角速度			
		mpu_dmp_get_data(&imu.roll,&imu.pit,&imu.yaw); 	    //读取欧拉角
    Gimbal_Control();//云台控制
		}
		
	if((int32_t)(currentTime - loopTime_5ms) >= 0)  
	{			
		loopTime_5ms = currentTime + 5000;	//5ms	
		
//		ANO_TC_Report16(PowerProtect.Judge_power*100);//Test-给匿名发送实时功率		
//		ANO_TC_Report(0,0,0,imu.gx,imu.gy,imu.gz,imu.roll,imu.pit,imu.yaw);//Test-给匿名发陀螺仪数据
	}
		
	if((int32_t)(currentTime - loopTime_20ms) >= 0)  
	{			
		loopTime_20ms = currentTime + 20000;	//20ms	
		
		RC_Offline_Handle();//丢控保护
	}
	
	if((int32_t)(currentTime - loopTime_50ms) >= 0)  
	{			
    loopTime_50ms = currentTime + 50000;    //50ms 这个时间戳是给roboremo_printf准备
		
		/*
	 	*用于确认参数修改成功
		*执行两条printf间隔不能不能太短
		*/
		printf_delay ++;
		switch(printf_delay)
		{
			case 1: Roboremo_printf("IN_p %.3f\n",roboremo_pid.Inner_p);   break;        //pid设定参数传回roboremo
			case 2: Roboremo_printf("IN_i %.3f\n",roboremo_pid.Inner_i);   break;
			case 3: Roboremo_printf("IN_d %.3f\n",roboremo_pid.Inner_d);   break;
			case 4: Roboremo_printf("OUT_p %.3f\n",roboremo_pid.Outter_p); break;
			case 5: Roboremo_printf("OUT_i %.3f\n",roboremo_pid.Outter_i); break;
			case 6: Roboremo_printf("OUT_d %.3f\n",roboremo_pid.Outter_d); break;
			
			case 7: Roboremo_printf("TT_L %.1f\n",roboremo_power.Total_Limit); break;     //功率设定参数传回roboremo
			case 8: Roboremo_printf("PW_T %.1f\n",roboremo_power.Power_Threshold); break;
			
			default:printf_delay = 0; break;
		}
	}
	
	if((int32_t)(currentTime - loopTime_100ms) >= 0)  
	{			
		loopTime_100ms = currentTime + 100000;	//100ms	
		
		Send_Data_To_Judgement();//发送自定义数据裁判系统
	}
	

}








