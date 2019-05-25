#include "relay.h"

bool flag_mirror_CCTV;
bool flag_bomb_claw_CCTV_switch;

void relay_init(void)
{	
  TUBE_CLAW_LOOSE;  //抱杆爪子松开
	BOMB_CLAW_CATCH;  //取弹爪子合上
	BOMB_CLAW_PULL;   //取弹爪子收回
	BOMB_EJECT_PULL;  //气动弹射收回	
	TRAILER_HOOK_PULL;//钩子气缸回收
  CCTV_CLIMB_TUBE;  //辅助视野默认爬杆
}

void CCTV_switch_task(void)
{
	if(get_bomb.get_bomb_enable == ENABLE)
	{
		if(flag_bomb_claw_CCTV_switch)
		{
			CCTV_FEED_RESCUE;
		}
		else CCTV_GET_BOMB;
	}
	else if(feed_bomb.feed_bomb_enable == ENABLE || 
		      rescue.rescue_enable == ENABLE)
	{
		if(flag_magazine_exit)
		{
			CCTV_GET_BOMB;
		}
	  else CCTV_FEED_RESCUE;
	}
	else if(!check.check_CCTV_enable)//检查程序不占用
	{
		CCTV_CLIMB_TUBE;
	}
		
}
