#include "led.h"

void Led_Init()
{
	GPIO_InitTypeDef gpio;	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC,ENABLE);

	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;	
	gpio.GPIO_Mode = GPIO_Mode_OUT;										
	gpio.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;  		
	GPIO_Init(GPIOC,&gpio);	

	gpio.GPIO_Pin = GPIO_Pin_8;		
	GPIO_Init(GPIOA,&gpio);	

	Visual_Off;
	Blue_Off;
	Orange_Off;
}

