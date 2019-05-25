#ifndef __OMRON_CTRL_H__
#define __OMRON_CTRL_H__

#include "bsp_sys.h"

#define R_SIDE			        0
#define L_SIDE		          1

typedef enum
{
  DETECT_ENABLE      = 0,
  FORCE_ZERO         = 1,
  FORCE_ONE          = 2,
} om_mode_e;


typedef __packed struct
{	
	om_mode_e  ctrl_mode;
		
	bool       value[2];
	bool       read[2];
	uint32_t   react_time[2];
	int16_t    confirm_time;
	
} optic_switch_t;


void optoelectronic_switch_init(void);
void optoelectronic_switch_task(void);

extern optic_switch_t optic_switch;

#endif
