#ifndef __SERVO_H__
#define __SERVO_H__

#include "system.h"

#define GIM_SERVO_ZERO		(TIM4->CCR1 = 111)
#define GIM_SERVO_SIDE		(TIM4->CCR1 = 68)
#define GIM_SERVO_BACK		(TIM4->CCR1 = 49)

#define MAGAZINE_OPEN               \
{                                   \
  TIM4->CCR2 = 67;                  \
	flag_magazine_exit = 1;                \
}                                   \

#define MAGAZINE_CLOSE              \
{                                   \
	TIM4->CCR2 = 112;                 \
	flag_magazine_exit = 0;                \
}   
//extern uint8_t MAM_PWM;

/*用于舵机的各种延时*/
extern bool servo_get_bomb_delay_record ;
extern bool servo_get_bomb_scan_delay_record ;

extern bool servo_feed_bomb_start_delay_record ;
extern bool servo_feed_bomb_lift_up_delay_record ;
extern bool servo_feed_bomb_lift_down_delay_record ;
extern bool servo_feed_bomb_end_delay_record ;

extern uint32_t servo_get_bomb_delay_time;
extern uint32_t servo_get_bomb_scan_delay_time;

extern uint32_t servo_feed_bomb_start_delay_time;
extern uint32_t servo_feed_bomb_lift_up_delay_time;
extern uint32_t servo_feed_bomb_lift_down_delay_time;
extern uint32_t servo_feed_bomb_end_delay_time;


void servo_init(void);
//void servo_task(void);


#endif

