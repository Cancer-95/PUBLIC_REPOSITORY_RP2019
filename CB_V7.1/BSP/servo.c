#include "servo.h"

//uint8_t MAM_PWM;

/*用于舵机的各种延时*/
bool servo_get_bomb_delay_record = 1;
bool servo_get_bomb_scan_delay_record = 1;

bool servo_feed_bomb_start_delay_record = 1;
bool servo_feed_bomb_lift_up_delay_record = 1;
bool servo_feed_bomb_lift_down_delay_record = 1;
bool servo_feed_bomb_end_delay_record = 1;

uint32_t servo_get_bomb_delay_time;
uint32_t servo_get_bomb_scan_delay_time;

uint32_t servo_feed_bomb_start_delay_time;
uint32_t servo_feed_bomb_lift_up_delay_time;
uint32_t servo_feed_bomb_lift_down_delay_time;
uint32_t servo_feed_bomb_end_delay_time;


void servo_init()	//TIM4  
{
	GPIO_InitTypeDef          gpio;
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef          oc;
	
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM4_STOP,ENABLE);					//使能Debug时设断点使定时器停
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		  //

	
  gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&gpio);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13, GPIO_AF_TIM4);   
 
	tim.TIM_Prescaler = 3360-1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;									//向上计数
	tim.TIM_Period = 1000-1;   																//50hz  25ms 计数周期
	tim.TIM_ClockDivision = TIM_CKD_DIV1;											//设置时钟分割，与某个滤波器有关这里设啥都没事
	TIM_TimeBaseInit(TIM4,&tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM2;													//选择PWM2则在向上计数时CNT没达到CCRx时输出无效电平 看到下面LOW之后即低电平有效，所以这里输出高
	oc.TIM_OutputState = TIM_OutputState_Enable;							//开启OC输出到相应引脚
	oc.TIM_Pulse = 0;																					//设置待装入捕获比较器的脉冲值 其实就是设置CCR
	oc.TIM_OCPolarity = TIM_OCPolarity_Low;										//设置输出极性  为低则表示有效电平为低电平
	
	oc.TIM_OutputNState = TIM_OutputNState_Disable;						//失能互补输出 关闭OCN输出到对应引脚   高级定时器使用 比如TIM_CH1
	oc.TIM_OCNPolarity = TIM_OCNPolarity_High;								//设置互补输出极性
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;								//选择空闲状态下的非工作状态
	oc.TIM_OCNIdleState = TIM_OCNIdleState_Set;								//选择互补空闲状态下的非工作状态
	
	TIM_OC1Init(TIM4,&oc);																		//通道1
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Disable);					//Enable为下一次更新事件才生效，Disable的话就是立即生效
	TIM_OC2Init(TIM4,&oc);																		//通道2TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Disable);
				 
	TIM_ARRPreloadConfig(TIM4,ENABLE);												//ARR可以重装，这里不是频域的控制所以这个有没有关系  与上面的一个道理
	
	TIM_CtrlPWMOutputs(TIM4,ENABLE);													//使能PWM输出
	
	TIM_Cmd(TIM4,ENABLE);

	TIM4->CCR1 = 0;																				    //对应云台舵机
	TIM4->CCR2 = 0;	                                          //对应弹仓舵机
  
	GIM_SERVO_ZERO;	
  MAGAZINE_CLOSE;
//	GIM_SERVO_BACK;	
//  MAGAZINE_CLOSE;
}

//void servo_task()
//{
////	if((lifter.total_angle[R] < lifter.target_tube_bit[R] + 100) ||
////		 (lifter.total_angle[R] > lifter.target_bomb_bit[R] - 100) ||
////	   (lifter.total_angle[L] > lifter.target_tube_bit[L] - 100) ||
////	   (lifter.total_angle[L] < lifter.target_bomb_bit[L] + 100))
////	{
////		if(get_bomb.get_bomb_enable == ENABLE || climb_tube.climb_enable == ENABLE)//取弹和爬杆模式下看向小电视
////		{	
////			GIM_SERVO_SIDE;  
////		}
////		else if(chassis.ass_mode_enable == ENABLE)//其他车尾模式
////		{
////			GIM_SERVO_BACK;
////		}
////		else if(chassis.ass_mode_enable == DISABLE)//关闭车尾模式时
////		{
////			GIM_SERVO_ZERO;
////		}	
////	}
//}  

