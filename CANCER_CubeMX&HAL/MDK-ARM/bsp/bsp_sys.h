#ifndef __BSP_SYS_H
#define __BSP_SYS_H

#include <stdio.h>
#include "stdbool.h"

#include "main.h"
#include "gpio.h"
#include "can.h"
#include "dma.h"
#include "usart.h"

#include "bsp_can.h"
#include "bsp_uart.h"

//#include "stm32f4xx_can.h"
//#include "stm32f4xx_it.h"
//#include "stm32f4xx.h"

#include "stdint.h"

#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "math.h"

#include "my_ramp.h"
#include "my_math.h"
#include "my_pid.h"

//#include "nvic.h"
//#include "can1.h"
//#include "can2.h"
//#include "roboremo.h"
//#include "ano_tc.h"
//#include "relay.h"
#include "servo_task.h"

#include "chassis_task.h"
//#include "lifter.h"
//#include "bomb_claw.h"
//#include "assist_wheel.h"
//#include "trailer.h"
//#include "dutched.h"
#include "omron_ctrl.h"
//#include "climb_tube.h"
//#include "get_bomb.h"
//#include "feed_bomb.h"
//#include "rescue.h"

//#include "modeswitch.h"
//#include "sys_check.h"

#include "info_interactive.h"
#include "keyboard_ctrl.h"
#include "remote_ctrl.h"

#include "mpu6050_iic.h"
#include "mpu6050_driver.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "imu_task.h"


#include "led_ctrl.h"
//#include "rc.h"

//#include "judgement.h"//It has CRC's declare


/* Cancer's conventional definition */
//#define R_Cancer			        0
//#define L_Cancer			        1
//#define X_Cancer			        2
//#define Y_Cancer			        3

#define M1						0
#define M2  					1
#define M3 						2
#define M4						3

#define ANGLE			    0
#define SPEED			    1
/*************************************/

/* can relevant */
#define CHASSIS_CAN        hcan1
#define LIFTER_CAN         hcan1
#define GET_BOMB_CAN       hcan2
#define RESCUE_CAN         hcan2

//#define ZGYRO_CAN         hcan2
//#define CHASSIS_ZGYRO_CAN hcan1
//#define GIMBAL_CAN        hcan1
#define TRIGGER_CAN       hcan2//can2 test

/* the ratio of motor encoder value translate to degree */
#define ENCODER_ANGLE_RATIO    (8192.0f/360.0f)


/* uart relevant */
/**
  * @attention
  * close usart DMA receive interrupt, so need add 
  * uart_receive_handler() before HAL_UART_IROHandler() in uart interrupt function
*/
#define DBUS_HUART         huart2 //for dji remote controler reciever
#define JUDGE_HUART        huart5 //connected to judge system



uint32_t micros(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

uint32_t millis(void);

//void system_init(void);
void loop(void);

#endif


