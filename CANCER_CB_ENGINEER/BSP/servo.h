#ifndef __SERVO_H__
#define __SERVO_H__

#include "system.h"

#define GIM_SERVO_ZERO    \
{                         \
	TIM4->CCR1 = 111;       \
	flag_watch_CCTV = 0;    \
}                         \

#define GIM_SERVO_SIDE	  \
{                         \
	TIM4->CCR1 = 64;        \
	flag_watch_CCTV = 1;    \
}                         \

#define GIM_SERVO_BACK    \
{                         \
	TIM4->CCR1 = 48;        \
	flag_watch_CCTV = 0;    \
}                         \

#define MAGAZINE_OPEN               \
{                                   \
/*  TIM4->CCR2 = 67;              */\
	GPIO_SetBits(GPIOD,GPIO_Pin_7);   \
	GPIO_SetBits(GPIOE,GPIO_Pin_12);  \
	flag_magazine_exit = 1;           \
}                                   \

#define MAGAZINE_CLOSE              \
{                                   \
/*	TIM4->CCR2 = 112;             */\
	GPIO_ResetBits(GPIOD,GPIO_Pin_7); \
	GPIO_ResetBits(GPIOE,GPIO_Pin_12);\
	flag_magazine_exit = 0;           \
}   
//extern uint8_t MAM_PWM;

/*用于舵机的各种延时*/
//extern bool servo_get_bomb_delay_record ;
//extern bool servo_get_bomb_scan_delay_record ;

//extern bool servo_feed_bomb_start_delay_record ;
//extern bool servo_feed_bomb_lift_up_delay_record ;
//extern bool servo_feed_bomb_lift_down_delay_record ;
//extern bool servo_feed_bomb_end_delay_record ;

//extern uint32_t servo_get_bomb_delay_time;
//extern uint32_t servo_get_bomb_scan_delay_time;

//extern uint32_t servo_feed_bomb_start_delay_time;
//extern uint32_t servo_feed_bomb_lift_up_delay_time;
//extern uint32_t servo_feed_bomb_lift_down_delay_time;
//extern uint32_t servo_feed_bomb_end_delay_time;

extern bool flag_watch_CCTV;//用来拟补小电视镜像

void servo_init(void);
//void servo_task(void);


#endif

