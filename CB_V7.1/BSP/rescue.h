#ifndef __RESCUE_H__
#define __RESCUE_H__

#include "system.h"


typedef enum
{	
	RETRACT_HOOK_MODE      = 0,//收回钩子
	RELEASE_HOOK_MODE      = 1,//释放钩子
	HOOK_CONFIRM_MODE      = 2,//确认钩住

}rescue_mode_e;

typedef __packed  struct
{
	rescue_mode_e  ctrl_mode;
		
	bool rescue_enable;	 
	bool hook_enable;
	
	bool flag_hook_confirm;
	
	bool hook_push_time_record;
	uint32_t hook_push_time;   //时间

} rescue_t;

extern rescue_t rescue;

void rescue_param_init(void);
void rescue_task(void);

#endif

