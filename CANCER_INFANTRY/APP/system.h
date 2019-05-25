#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdio.h>
#include "stdbool.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx.h"
#include "stdint.h"

#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "math.h"

#include "my_math.h"
#include "nvic.h"
#include "can1.h"
#include "can2.h"
#include "roboremo.h"
#include "ano_tc.h"
#include "pwm.h"
#include "pidckz.h"
#include "chassis_ctrl.h"
#include "gimbal_ctrl.h"
#include "shoot_ctrl.h"
#include "modeswitch.h"

#include "mpu6050_iic.h"
#include "mpu6050_driver.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#include "led.h"
#include "rc.h"

#include "judgement.h"//这里已经包含了CRC申明的函数


extern volatile uint32_t sysTickUptime;

#define abs(x) ((x)>0? (x):(-(x)))

/*----- 条件编译 -----*/
//#define USING_UART4_JUDGE




float constrain(float amt, float low, float high);
int32_t constrain_int32(int32_t amt, int32_t low, int32_t high);
int16_t constrain_int16(int16_t amt, int16_t low, int16_t high);

uint32_t micros(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

uint32_t millis(void);

void System_Init(void);
void Loop(void);

#endif


