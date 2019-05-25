#ifndef __GET_BOMB_H__
#define __GET_BOMB_H__

#include "system.h"

#define HAVENT     0
#define FINISHED   1

#define BOMB_CLAW_MOVE_LEFT 	(bomb_claw.Y_mode = Y_CLAW_ZERO) 
#define BOMB_CLAW_MOVE_MID 	  (bomb_claw.Y_mode = Y_CLAW_HALF) 
#define BOMB_CLAW_MOVE_RIGHT 	(bomb_claw.Y_mode = Y_CLAW_ENTIRE)
#define BOMB_CLAW_DONT_MOVE 	(bomb_claw.Y_mode = Y_CLAW_STOP)

#define BOMB_CLAW_ROLL_ENTIRE (bomb_claw.R_mode = R_CLAW_ENTIRE) 
#define BOMB_CLAW_ROLL_BIT 	  (bomb_claw.R_mode = R_CLAW_BIT) 

#define BOMB_CLAW_ROLL_HALF         \
{                                   \
	bomb_claw.R_mode = R_CLAW_HALF;   \
	flag_bomb_claw_roll = 0;          \
}                                   \

#define BOMB_CLAW_ROLL_ZERO         \
{                                   \
	bomb_claw.R_mode = R_CLAW_ZERO;   \
	flag_bomb_claw_roll = 1;          \
} 

//#define BOMB_CLAW_ROLL_ZERO 	(bomb_claw.R_mode = R_CLAW_ZERO) 
//#define BOMB_CLAW_ROLL_BIT 	  (bomb_claw.R_mode = R_CLAW_BIT) 

//#define BOMB_CLAW_ROLL_HALF         \
//{                                   \
//	bomb_claw.R_mode = R_CLAW_HALF;   \
//	flag_bomb_claw_roll = 0;          \
//}                                   \

//#define BOMB_CLAW_ROLL_ENTIRE       \
//{                                   \
//	bomb_claw.R_mode = R_CLAW_ENTIRE; \
//	flag_bomb_claw_roll = 1;          \
//}                                   \

//#define BOMB_CLAW_TRIPLE_DISABLE    \
//{                                   \
//	get_bomb.once_triple = 0;	        \
//	flag_bomb_claw_triple = 0;        \
//}                                   \
//  
//#define BOMB_CLAW_TRIPLE_ENABLE     \
//{                                   \
//	get_bomb.once_triple = 1;	        \
//	flag_bomb_claw_triple = 1;        \
//}                                   \

typedef enum
{	
	STAND_BY_MODE          = 0,//归中待命模式
	PUSH_AIM_SINGLE_MODE   = 1,//单箱模式下的推出对准模式
	PUSH_AIM_MULTIPLE_MODE = 2,//多箱模式下的推出对准模式
	PUSH_AIM_SCAN_MODE     = 3,//扫描模式下的推出对准模式
	VISION_ASSIST_MODE     = 4,//视觉辅助模式
	GET_POUR_MODE          = 5,//取弹倒弹模式
	SUSPEND_MODE           = 6,//阻塞态模式 用于纠错程序抢占控制
}get_bomb_mode_e;

typedef __packed  struct
{
	get_bomb_mode_e  ctrl_mode;
		
	bool             get_bomb_enable;	 
	bool             bomb_claw_catch_enable;

//	bool vision_assist;
	bool             turn_around_enable;
	bool             turn_around_record;
	uint32_t         turn_around_time;
	
	bool             once_triple;
	bool             once_penta;
	bool             scan;
	bool             scan_finished;
	
	uint8_t          box_num;
	uint8_t          get_pour_step;      //取弹倒弹模式的步骤
	
	bool             flag_push_aim_single;
	bool             flag_push_aim_multiple;
  bool             flag_push_aim_scan;
	
	bool             flag_scan_failed;
	
	bool             flag_get_pour;
	
	uint32_t         claw_loose_Time;   //时间
	uint32_t         eject_push_Time;   //时间
	uint32_t         scan_safe_Time;    //时间
	
	bool             scan_delay_record;
	uint32_t         scan_delay_time;
		
} get_bomb_t;

extern get_bomb_t get_bomb;

/*这些标志位准备给keyboard.c用 ctrl+Z 纠错的时候 有些没用到*/
extern bool flag_get_bomb_init;

extern bool flag_push_aim_single_init;
extern bool flag_push_aim_multiple_init;
extern bool flag_push_aim_scan_init;
extern bool flag_scan_safe_delay;

extern bool flag_eject_push_Time_record;
extern bool flag_bomb_claw_record;  

void get_bomb_param_init(void);
void get_bomb_task(void);

#endif

