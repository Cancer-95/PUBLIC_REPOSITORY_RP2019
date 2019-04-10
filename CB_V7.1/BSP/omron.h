#ifndef __OMRON_H__
#define __OMRON_H__

#include "system.h"

#define OPTIC_SWITCH_S_GPIO         GPIOA
#define OPTIC_SWITCH_S_GPIO_PIN     GPIO_Pin_6
//#define OPTIC_SWITCH_R_GPIO         GPIOA
//#define OPTIC_SWITCH_R_GPIO_PIN     GPIO_Pin_7
#define OPTIC_SWITCH_M_GPIO         GPIOC
#define OPTIC_SWITCH_M_GPIO_PIN     GPIO_Pin_6

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


void optoelectronic_switch_init(void);

void optic_scan_task(void);

void optoelectronic_switch_task(void);

extern optic_switch_t optic_switch;


#endif
