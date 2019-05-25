#ifndef __PWM_H__
#define __PWM_H__

#include "system.h"

#define FRIC_R_PWM		TIM4->CCR3
#define FRIC_L_PWM		TIM4->CCR4

#define SERVO_PWM     TIM1->CCR2

void PWM_Init(void);
void Fric_Wheel_PWM_Configuration(void);  //TIM4
void BBox_Servo__PWM_Configuration(void);	//TIM1


#endif

