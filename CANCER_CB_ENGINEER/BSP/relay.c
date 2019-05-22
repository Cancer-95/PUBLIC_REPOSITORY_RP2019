#include "relay.h"

bool flag_mirror_CCTV;
bool flag_bomb_claw_CCTV_switch;

void relay_init(void)
{
	GPIO_InitTypeDef gpio;   

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | 
	                       RCC_AHB1Periph_GPIOC | 
	                       RCC_AHB1Periph_GPIOD | 
	                       RCC_AHB1Periph_GPIOE,ENABLE);
			
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	gpio.GPIO_Speed = GPIO_Speed_50MHz;	
	gpio.GPIO_Mode = GPIO_Mode_OUT;										
	
//	gpio.GPIO_Pin = GPIO_Pin_1;//PB1:气动刹车 	
	gpio.GPIO_Pin = GPIO_Pin_3 
								| GPIO_Pin_4 
								| GPIO_Pin_5
								| GPIO_Pin_6 //PE3:取弹爪子 PE4:箱子弹射 PE5:补弹摄像头 PE6:小电视切换	
								| GPIO_Pin_15
								| GPIO_Pin_14
								| GPIO_Pin_13
								| GPIO_Pin_12;//PE15:抱杆爪子 PE14:取弹爪子推拉 PE13:救援 PE12:弹仓开合	
	GPIO_Init(GPIOE,&gpio);	
	
//	gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;//PE9:取弹爪子 PE11:箱子弹射 		
//	GPIO_Init(GPIOE,&gpio);	
	
	gpio.GPIO_Pin = GPIO_Pin_2
	              | GPIO_Pin_7
								| GPIO_Pin_6
               	| GPIO_Pin_5 
	              | GPIO_Pin_4;//PA2:取弹爪子 PA5:箱子弹射 PA4:小电视切换 PA67:拖车推出
	GPIO_Init(GPIOA,&gpio);	
	
	gpio.GPIO_Pin = GPIO_Pin_0;//PC0:补弹摄像头 
	GPIO_Init(GPIOC,&gpio);	
	
	gpio.GPIO_Pin = GPIO_Pin_4
               	| GPIO_Pin_5 
	              | GPIO_Pin_6
	              | GPIO_Pin_7;//PD4:抱杆爪子 PD5:取弹爪子推拉 PD6:拖车打下 PD7:弹仓开合
	GPIO_Init(GPIOD,&gpio);	
	
//	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;//PC6:取弹爪子推拉 PC7:救援 PC8：姿态矫正 PC9:小电视切换
//	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;//PC6:取弹爪子推拉 PC7:救援
//	GPIO_Init(GPIOC,&gpio);	
	
  TUBE_CLAW_LOOSE;//抱杆爪子松开
//	TUBE_BRAKE_PULL;//气动刹车收回
	
	BOMB_CLAW_CATCH;//取弹爪子合上
	BOMB_CLAW_PULL;//取弹爪子收回
	BOMB_EJECT_PULL;//气动弹射收回	
	
	TRAILER_HOOK_PULL;//钩子气缸回收
	TRAILER_HOOK_CAPTURE;//钩子气缸回收
//	SUPPORTING_WHEEL_PULL;//辅助支撑腿收回

  CCTV_CLIMB_TUBE;//辅助视野默认爬杆
}

void CCTV_switch_task(void)
{
	if(get_bomb.get_bomb_enable == ENABLE)
	{
		if(flag_bomb_claw_CCTV_switch)
		{
			CCTV_FEED_RESCUE;
		}
		else CCTV_GET_BOMB;
	}
	else if(feed_bomb.feed_bomb_enable == ENABLE || 
		      rescue.rescue_enable == ENABLE)
	{
		if(flag_magazine_exit)
		{
			CCTV_GET_BOMB;
		}
	  else CCTV_FEED_RESCUE;
	}
	else if(!check.check_CCTV_enable)//检查程序不占用
	{
		CCTV_CLIMB_TUBE;
	}
		
}
