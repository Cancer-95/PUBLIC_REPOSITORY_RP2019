#include "dutched.h"

#define DUTCHED_CTRL_PERIOD     5  //ms 放在哪个时间戳
#define DUTCHED_ACC_TIME        50  //ms 

//uint8_t MAM_PWM;

dutched_t ducted;

void ducted_init(void)
{
	memset(&ducted, 0, sizeof(dutched_t));
	
////	GPIO_InitTypeDef         gpio;
//	TIM_TimeBaseInitTypeDef   tim;
//	TIM_OCInitTypeDef          oc;
//	
//	DBGMCU_APB1PeriphConfig(DBGMCU_TIM4_STOP,ENABLE);					//使能Debug时设断点使定时器停
////	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		  //

////  gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
////	gpio.GPIO_Mode = GPIO_Mode_AF;
////	gpio.GPIO_Speed = GPIO_Speed_100MHz;
////	GPIO_Init(GPIOB,&gpio);
//	
////	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8, GPIO_AF_TIM4);
////	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9, GPIO_AF_TIM4);  
//	
// /*周期 = (Period * Prescaler)/FLK = (2500*84)/84000000=2.5ms*/
//	tim.TIM_Prescaler = 1680-1;
//	tim.TIM_CounterMode = TIM_CounterMode_Up;									//向上计数
//	tim.TIM_Period = 1000-1;   																//50hz  计数周期
//	tim.TIM_ClockDivision = TIM_CKD_DIV1;											//设置时钟分割，与某个滤波器有关这里设啥都没事
//	TIM_TimeBaseInit(TIM4,&tim);
//	
//	oc.TIM_OCMode = TIM_OCMode_PWM2;													//选择PWM2则在向上计数时CNT没达到CCRx时输出无效电平 看到下面LOW之后即低电平有效，所以这里输出高
//	oc.TIM_OutputState = TIM_OutputState_Enable;							//开启OC输出到相应引脚
//	oc.TIM_Pulse = 0;																					//设置待装入捕获比较器的脉冲值 其实就是设置CCR
//	oc.TIM_OCPolarity = TIM_OCPolarity_Low;										//设置输出极性  为低则表示有效电平为低电平
//	
//	oc.TIM_OutputNState = TIM_OutputNState_Disable;						//失能互补输出 关闭OCN输出到对应引脚   高级定时器使用 比如TIM_CH1
//	oc.TIM_OCNPolarity = TIM_OCNPolarity_High;								//设置互补输出极性
//	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;								//选择空闲状态下的非工作状态
//	oc.TIM_OCNIdleState = TIM_OCNIdleState_Set;								//选择互补空闲状态下的非工作状态
//	
//	TIM_OC3Init(TIM4,&oc);																		//通道3
//	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Disable);					//Enable为下一次更新事件才生效，Disable的话就是立即生效
//	TIM_OC4Init(TIM4,&oc);																		//通道4
//	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Disable);
//				 
//	TIM_ARRPreloadConfig(TIM4,ENABLE);												//ARR可以重装，这里不是频域的控制所以这个有没有关系  与上面的一个道理
//	
//	TIM_CtrlPWMOutputs(TIM4,ENABLE);													//使能PWM输出
//	
//	TIM_Cmd(TIM4,ENABLE);
//	//CCRx/ARR 就是占空比		ARR就是通过TIM_Period设置(2500)

  /*涵道解锁*/
	DUCTED_R_UNLOCKED;//PB8																			//对应1
	DUCTED_L_UNLOCKED;//PB9                                     //对应2

}

void ducted_pwm(uint16_t pwm_L, uint16_t pwm_R)//摩擦轮的控制
{
	DUCTED_L_PWM = pwm_L + 40;
	DUCTED_R_PWM = pwm_R + 40;
}

void ducted_task(void)
{
	if (system_mode == SAFETY_MODE)
	{
		ducted.pwm_set[L] = 0;
		ducted.pwm_set[R] = 0;		
	}	
	else
	{
		ducted.pwm_set[L] = 83;
		ducted.pwm_set[R] = 83;		
	}

	
	switch(ducted.ctrl_mode)
	{
		case DUCTED_L_RUN:
		{								
		 ducted.pwm_out[L] = ducted.pwm_set[L] * ramp_dutched_calc(&ld_run_ramp, CNT_UP, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);
		 ducted.pwm_out[R] = ducted.pwm_set[R] * ramp_dutched_calc(&rd_run_ramp, TO_ZERO, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);;
		}break;

		case DUCTED_R_RUN:
		{						
			ducted.pwm_out[R] = ducted.pwm_set[R] * ramp_dutched_calc(&rd_run_ramp, CNT_UP, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);
			ducted.pwm_out[L] = ducted.pwm_set[L] * ramp_dutched_calc(&ld_run_ramp, TO_ZERO, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);;		
		}break;
		
		case DUCTED_ALL_OFF:
		{	
			ducted.pwm_out[L] = ducted.pwm_set[L] * ramp_dutched_calc(&ld_run_ramp, TO_ZERO, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);;
			ducted.pwm_out[R] = ducted.pwm_set[R] * ramp_dutched_calc(&rd_run_ramp, TO_ZERO, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);;
		}break;
		
		case DUCTED_ALL_RUN:
		{	
			ducted.pwm_out[L] = ducted.pwm_set[L] * ramp_dutched_calc(&ld_run_ramp, CNT_UP, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);;
			ducted.pwm_out[R] = ducted.pwm_set[R] * ramp_dutched_calc(&rd_run_ramp, CNT_UP, DUTCHED_ACC_TIME/DUTCHED_CTRL_PERIOD);;
		}break;		
		
		default:																																															
    break;
	}	
	ducted_pwm((int16_t)ducted.pwm_out[L],(int16_t)ducted.pwm_out[R]);
}


