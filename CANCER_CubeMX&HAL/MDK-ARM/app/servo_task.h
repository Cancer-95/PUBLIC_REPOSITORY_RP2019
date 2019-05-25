#ifndef __SERVO_TASK_H__
#define __SERVO_TASK_H__

#include "bsp_sys.h"

#define GIM_SERVO_ZERO		(TIM4->CCR1 = 23)
#define GIM_SERVO_SIDE		(TIM4->CCR1 = 50)
#define GIM_SERVO_BACK		(TIM4->CCR1 = 83)

#define MAGAZINE_OPEN               \
{                                   \
  TIM4->CCR2 = 70;                  \
	flag_magazine = 1;                \
}                                   \

#define MAGAZINE_CLOSE              \
{                                   \
	TIM4->CCR2 = 23;                  \
	flag_magazine = 0;                \
}   

void servo_task(void);


#endif

