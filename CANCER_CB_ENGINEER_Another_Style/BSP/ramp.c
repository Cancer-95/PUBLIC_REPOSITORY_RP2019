#include "ramp.h"

/*给键盘WSAD的斜坡*/
ramp_t fb_ramp = RAMP_GEN_DAFAULT;
ramp_t lr_ramp = RAMP_GEN_DAFAULT; 

/*给涵道启动的斜坡*/
ramp_t ld_run_ramp = RAMP_GEN_DAFAULT;
ramp_t ld_off_ramp = RAMP_GEN_DAFAULT;

ramp_t rd_run_ramp = RAMP_GEN_DAFAULT;
ramp_t rd_off_ramp = RAMP_GEN_DAFAULT;

/*给复位启动的斜坡*/
ramp_t reset_lifter_spd_ramp = RAMP_GEN_DAFAULT;
ramp_t reset_bomb_claw_spd_ramp = RAMP_GEN_DAFAULT;
ramp_t reset_trailer_spd_ramp = RAMP_GEN_DAFAULT;

/*给复位启动的斜坡*/
ramp_t landing_angle_ramp = RAMP_GEN_DAFAULT;

/*给掉头的斜坡*/
ramp_t turn_around_ramp = RAMP_GEN_DAFAULT;

/*给左转的斜坡*/
ramp_t turn_left_ramp = RAMP_GEN_DAFAULT;

/*给右转的斜坡*/
ramp_t turn_right_ramp = RAMP_GEN_DAFAULT;

/*给自转刹车的斜坡*/
ramp_t spin_brake_ramp = RAMP_GEN_DAFAULT;

void ramp_init(ramp_t *ramp, int32_t scale)//斜坡计数清零
{
  ramp->count = 0;
  ramp->scale = scale;
}

float ramp_calc(ramp_t *ramp)//实时斜坡比例计算值
{
  if (ramp->scale <= 0)
    return 0;
  
  if (ramp->count++ >= ramp->scale)
    ramp->count = ramp->scale;
  
  ramp->out = ramp->count / ((float)ramp->scale);
  return ramp->out;
}

/*Cacner自己加上的用于底盘的斜坡 加上了反向刹车*/
float ramp_kb_calc(ramp_t *ramp, uint8_t dir, int32_t scale)
{
	ramp->scale = scale;

	switch(dir)
	{
	  case CNT_UP://正方向
	  {
			if(ramp->count < 0) ramp->count += 3;
			else if(ramp->count++ >= ramp->scale) ramp->count = ramp->scale;	
	  }break;
		
	  case CNT_DOWN://反方向
	  {
			if(ramp->count > 0) ramp->count -= 3;
      else if(ramp->count-- <= -ramp->scale) ramp->count = -ramp->scale;
	  }break;
		
		case TO_ZERO://两个方向刹车归零
	  {		
			if(ramp->count < 0 && (ramp->count += 3) >= 0) ramp->count = 0;			
      else if(ramp->count > 0 && (ramp->count -= 3) <= 0) ramp->count = 0;
	  }break;

		default:break;
	}
	
  ramp->out = ramp->count / ((float)ramp->scale);
  return ramp->out;
}
/*Cacner自己加上的用于涵道的斜坡 没有什么特点*/
float ramp_dutched_calc(ramp_t *ramp, uint8_t dir, int32_t scale)
{
	ramp->scale = scale;

	switch(dir)
	{
	  case CNT_UP:
	  {
			if(ramp->count++ >= ramp->scale) ramp->count = ramp->scale;	
	  }break;
	
		case TO_ZERO:
	  {		
      if((ramp->count -= 3) <= 0) ramp->count = 0;
	  }break;

		default:break;
	}
	
  ramp->out = ramp->count / ((float)ramp->scale);
  return ramp->out;
}

