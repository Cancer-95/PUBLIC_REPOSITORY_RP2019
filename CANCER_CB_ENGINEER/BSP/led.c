#include "led.h"

void led_init()
{
	GPIO_InitTypeDef gpio;	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;	
	gpio.GPIO_Mode = GPIO_Mode_OUT;	

	
	gpio.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;  		
	GPIO_Init(GPIOC,&gpio);	

	gpio.GPIO_Pin = GPIO_Pin_13;  		
	GPIO_Init(GPIOD,&gpio);	
//	GPIO_SetBits(GPIOC,GPIO_Pin_8);
//	GPIO_SetBits(GPIOC,GPIO_Pin_10);
//	GPIO_SetBits(GPIOC,GPIO_Pin_11);
//	GPIO_SetBits(GPIOC,GPIO_Pin_13);
//	GPIO_SetBits(GPIOC,GPIO_Pin_14);
	
	LED_CAMERA_OFF;
	LED_BLUE_OFF;
  LED_ORANGE_OFF;
	LED_GREEN_OFF;
  LED_RED_OFF;
}

void led_task(void)
{
	if(system_mode == SAFETY_MODE) 
	{
		if(check.check_enable)
		{
			LED_ORANGE_ON;
			LED_GREEN_ON;
			LED_RED_ON;
			LED_BLUE_ON;
			check.check_enable = DISABLE;
		}
		else
		{
			LED_ORANGE_TOGGLE;
			LED_GREEN_TOGGLE;	
			LED_RED_TOGGLE;
			LED_BLUE_TOGGLE;			
		}
	}	
	else if(flag_bomb_claw_penta == ENABLE) 
	{
	  LED_ORANGE_TOGGLE;
		LED_GREEN_OFF;	
	  LED_RED_OFF;
		LED_BLUE_TOGGLE;	
	}	
	else if(flag_bomb_claw_triple == ENABLE) 
	{
	  LED_ORANGE_OFF;
		LED_GREEN_TOGGLE;	
	  LED_RED_TOGGLE;
		LED_BLUE_OFF;	
	}
	else if(!check.check_enable)
	{
		LED_ORANGE_OFF;
		LED_GREEN_OFF;
		LED_RED_OFF;
		LED_BLUE_OFF;
	}
	

}
