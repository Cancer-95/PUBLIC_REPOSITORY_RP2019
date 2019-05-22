#ifndef __REMOTE_CTRL_H__
#define __REMOTE_CTRL_H__

#include "system.h"

typedef struct
{
  float vx;
  float vy;
  float vw;
  
	bool rc_enable;
	
} rc_ctrl_t;

extern rc_ctrl_t rc;


void remote_ctrl_chassis_handler(void);

#endif

