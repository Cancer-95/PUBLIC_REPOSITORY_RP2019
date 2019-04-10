#ifndef __LED_H
#define __LED_H

#include "system.h"

#define LED_BLUE_ON    	   GPIO_ResetBits(GPIOC,GPIO_Pin_13)
#define LED_BLUE_OFF  	   GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define LED_BLUE_TOGGLE    GPIOC->ODR ^= GPIO_Pin_13


#define LED_ORANGE_ON		   GPIO_ResetBits(GPIOC,GPIO_Pin_14)
#define LED_ORANGE_OFF     GPIO_SetBits(GPIOC,GPIO_Pin_14)
#define LED_ORANGE_TOGGLE  GPIOC->ODR ^= GPIO_Pin_14

#define LED_GREEN_ON		   GPIO_ResetBits(GPIOC,GPIO_Pin_10)
#define LED_GREEN_OFF      GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define LED_GREEN_TOGGLE   GPIOC->ODR ^= GPIO_Pin_10

#define LED_RED_ON		     GPIO_ResetBits(GPIOC,GPIO_Pin_11)
#define LED_RED_OFF        GPIO_SetBits(GPIOC,GPIO_Pin_11)
#define LED_RED_TOGGLE     GPIOC->ODR ^= GPIO_Pin_11

#define LED_CAMERA_ON		   GPIO_SetBits(GPIOC,GPIO_Pin_8)
#define LED_CAMERA_OFF     GPIO_ResetBits(GPIOC,GPIO_Pin_8)
#define LED_CAMERA_TOGGLE  GPIOC->ODR ^= GPIO_Pin_8

void led_init(void);
void led_task(void);
#endif

