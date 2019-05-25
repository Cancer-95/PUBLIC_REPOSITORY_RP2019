#ifndef __OMRON_H__
#define __OMRON_H__

#include "system.h"

typedef enum
{
  DETECT_ENABLE      = 0,
  FORCE_ZERO         = 1,
  FORCE_ONE          = 2,
} om_mode_e;


typedef __packed struct
{	
	om_mode_e  ctrl_mode;
	
	bool       scan_enable;
	bool       value_record;    //用来记录扫描值的flag
	bool       value_start[3];  //开始扫描值
//	bool       value_last[3];   //上一次光电值
	bool       value[3];
	bool       read[3];
	uint32_t   react_time_A[3];
	uint32_t   react_time_B[3];
		
	int16_t    confirm_time;
	int16_t    scan_confirm_time;
	
} optic_switch_t;


void optic_switch_init(void);

void optic_scan_task(void);

void optoelectronic_switch_task(void);

extern optic_switch_t optic_switch;


#endif
