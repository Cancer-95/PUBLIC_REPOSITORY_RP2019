#ifndef __FEED_BOMB_H__
#define __FEED_BOMB_H__

#include "system.h"


typedef enum
{	
	QUIT_FEED_MODE     = 0,//退出喂弹模式
	AIM_FEED_MODE      = 1,//对准喂弹模式
	
}feed_bomb_mode_e;

typedef __packed  struct
{
	feed_bomb_mode_e  ctrl_mode;
		
	bool feed_bomb_enable;
	bool sight_enable;
	bool vision_enable;	
	bool lifter_enable;	 
	bool exit_enable;

} feed_bomb_t;

extern feed_bomb_t feed_bomb;

void feed_bomb_param_init(void);
void feed_bomb_task(void);

#endif

