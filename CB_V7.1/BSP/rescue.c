#include "rescue.h"

rescue_t rescue;

void rescue_param_init(void)
{
	TRAILER_HOOK_PULL;                      //钩子气缸回收	
	
	trailer.ctrl_mode = TRAILER_PULL;       //拖车收回			
	
	rescue.flag_hook_confirm = 0;           //钩子确认置零
	rescue.hook_push_time_record = ENABLE;  //钩子时间记录标志位置一
  rescue.hook_enable = DISABLE;           //钩子不可控
}

void rescue_ctrl(void)
{
	keyboard_rescue_handler();//keyboard	
		
	if(rescue.rescue_enable == ENABLE)	
	{
		switch(rescue.ctrl_mode)
		{	
			case RETRACT_HOOK_MODE://收回钩子
			{
				rescue.hook_enable = DISABLE;           //钩子不可控
				/*记录钩子放下的时间*/
				if(rescue.hook_push_time_record)
				{
					rescue.hook_push_time = millis();
					rescue.hook_push_time_record = DISABLE;
				}
								
				if((millis() - rescue.hook_push_time < 1000) && (trailer.ctrl_mode == TRAILER_PUSH))  //2000ms以内只收砸气缸下来
				{
					TRAILER_HOOK_PUSH;                         //拖车钩子气缸弹下 释放伤员
					chassis.force_draft_out_enable = ENABLE;
				}
				else
				{
					trailer.ctrl_mode = TRAILER_PULL;                                //拖车收回		
          TRAILER_HOOK_PULL;                                               //钩子气缸回收					
					chassis.force_draft_out_enable = DISABLE;
					if(abs(trailer.total_angle - trailer.target_zero) < 100)         //拖车收的差不多了
					{
						flag_rescue_sight = 0;
						GIM_SERVO_ZERO;
						chassis.ass_mode_enable = DISABLE;                             //关闭车尾模式				
//						TRAILER_HOOK_PULL;                                            //钩子气缸回收
						flag_trailer_hook = 0;                                         //让下一次钩子伸出来是它是收上去的
						rescue.flag_hook_confirm = 0;                                  //钩子确认置零
						rescue.hook_push_time_record = 1;
						rescue.rescue_enable = DISABLE;                                //让这个case只跑一次			
					}							
				}
			}break;	
			
			case RELEASE_HOOK_MODE://释放钩子
			{				
			  trailer.ctrl_mode = TRAILER_PUSH;  //拖车推出
//				GIM_SERVO_BACK;
				chassis.ass_mode_enable = ENABLE;  //开启车尾模式
				
				if(abs(trailer.total_angle - trailer.target_entire) < 100) rescue.hook_enable = ENABLE;//钩子可控 	
			}break;	
			
			case HOOK_CONFIRM_MODE://确认钩住
			{
				rescue.hook_enable = DISABLE;      //钩子不可控
				TRAILER_HOOK_PULL;                 //钩子气缸回收(钩住伤员)
				trailer.ctrl_mode = TRAILER_RELAX; //拖车泻力
			  flag_rescue_sight = 0;
				GIM_SERVO_ZERO;
				chassis.ass_mode_enable = DISABLE; //关闭车尾模式
			}break;				
			
			default: break;
		}
	}
}

void rescue_task(void)
{
//	if(trailer.reset)
//	{
		rescue_ctrl();
//	}
}


