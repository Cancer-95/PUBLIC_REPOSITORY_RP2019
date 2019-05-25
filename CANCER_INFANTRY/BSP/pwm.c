#include "pwm.h"
void PWM_Init()
{
	Fric_Wheel_PWM_Configuration();
	BBox_Servo__PWM_Configuration();
}

void Fric_Wheel_PWM_Configuration()	//TIM4  摩擦轮
{
	GPIO_InitTypeDef          gpio;
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef          oc;
	
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM4_STOP,ENABLE);					//使能Debug时设断点使定时器停
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		  //

	
  gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&gpio);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9, GPIO_AF_TIM4);   
 
	tim.TIM_Prescaler = 84-1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;									//向上计数
	tim.TIM_Period = 2500-1;   																//50hz  25ms 计数周期
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
	
	TIM_OC3Init(TIM4,&oc);																		//通道3
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Disable);					//Enable为下一次更新事件才生效，Disable的话就是立即生效
	TIM_OC4Init(TIM4,&oc);																		//通道4
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Disable);
				 
	TIM_ARRPreloadConfig(TIM4,ENABLE);												//ARR可以重装，这里不是频域的控制所以这个有没有关系  与上面的一个道理
	
	TIM_CtrlPWMOutputs(TIM4,ENABLE);													//使能PWM输出
	
	TIM_Cmd(TIM4,ENABLE);
	//CCRx/ARR 就是占空比		ARR就是通过TIM_Period设置
	TIM4->CCR3 = 1000;																				  //对应1
	TIM4->CCR4 = 1000;	                                        //对应2
}

void BBox_Servo__PWM_Configuration(void)	//TIM1 弹仓舵机
{
	GPIO_InitTypeDef          gpio;
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef         oc;
	
  DBGMCU_APB1PeriphConfig(DBGMCU_TIM1_STOP,ENABLE);					//使能Debug时设断点使定时器停
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);		

	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&gpio);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_TIM1);     
	
	tim.TIM_Prescaler     = 3360-1;
	tim.TIM_CounterMode   = TIM_CounterMode_Up;	 //向上计数
	tim.TIM_Period        = 999;                 //20ms	计数周期
	tim.TIM_ClockDivision = TIM_CKD_DIV1;		     //设置时钟分割 不为1的话会乘2
	TIM_TimeBaseInit(TIM1,&tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM2;		            //选择定时器模式
	oc.TIM_OutputState = TIM_OutputState_Enable;		//选择输出比较状态
	oc.TIM_OutputNState = TIM_OutputState_Disable;	//选择互补输出比较状态
	oc.TIM_Pulse = 0;		                            //设置待装入捕获比较器的脉冲值
	oc.TIM_OCPolarity = TIM_OCPolarity_Low;		      //设置输出极性
	oc.TIM_OCNPolarity = TIM_OCPolarity_High;		    //设置互补输出极性
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;		  //选择空闲状态下的非工作状态
	oc.TIM_OCNIdleState = TIM_OCIdleState_Set;		  //选择互补空闲状态下的非工作状态

	TIM_OC2Init(TIM1,&oc);		//通道2
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
				 
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	TIM_Cmd(TIM1,ENABLE);

  TIM1->CCR2 = 0;
}

