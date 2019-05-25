#ifndef __LED_H
#define __LED_H

#include "system.h"

#define LED_BLUE_ON    	   GPIO_ResetBits(LED_ONBOARD_GPIO_Port, LED_BLUE_Pin)
#define LED_BLUE_OFF  	   GPIO_SetBits(LED_ONBOARD_GPIO_Port, LED_BLUE_Pin)
#define LED_BLUE_TOGGLE    LED_ONBOARD_GPIO_Port -> ODR ^= LED_BLUE_Pin


#define LED_ORANGE_ON		   GPIO_ResetBits(LED_ONBOARD_GPIO_Port, LED_ORANGE_Pin)
#define LED_ORANGE_OFF     GPIO_SetBits(LED_ONBOARD_GPIO_Port, LED_ORANGE_Pin)
#define LED_ORANGE_TOGGLE  LED_ONBOARD_GPIO_Port -> ODR ^= LED_ORANGE_Pin

#define LED_GREEN_ON		   GPIO_ResetBits(LED_ONBOARD_GPIO_Port, LED_GREEN_Pin)
#define LED_GREEN_OFF      GPIO_SetBits(LED_ONBOARD_GPIO_Port, LED_GREEN_Pin)
#define LED_GREEN_TOGGLE   LED_ONBOARD_GPIO_Port -> ODR ^= LED_GREEN_Pin

#define LED_RED_ON		     GPIO_ResetBits(LED_ONBOARD_GPIO_Port, LED_RED_Pin)
#define LED_RED_OFF        GPIO_SetBits(LED_ONBOARD_GPIO_Port, LED_RED_Pin)
#define LED_RED_TOGGLE     LED_ONBOARD_GPIO_Port -> ODR ^= LED_RED_Pin

#define LED_CAMERA_ON		   GPIO_SetBits(LED_CAMERA_GPIO_Port, LED_CAMERA_Pin)
#define LED_CAMERA_OFF     GPIO_ResetBits(LED_CAMERA_GPIO_Port, LED_CAMERA_Pin)
#define LED_CAMERA_TOGGLE  LED_CAMERA_GPIO_Port -> ODR ^= LED_CAMERA_Pin

void led_init(void);
void led_task(void);
#endif

