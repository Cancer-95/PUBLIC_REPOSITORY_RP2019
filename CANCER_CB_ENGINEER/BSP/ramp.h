#ifndef __RAMP_H__
#define __RAMP_H__

#include "system.h"

/*计数方式for ramp_keyboard*/
#define TO_ZERO  0
#define CNT_UP   1
#define CNT_DOWN 2

#define FAN_RUN  0
#define FAN_OFF  1


typedef struct ramp_t
{
  int32_t count;
  int32_t scale;
  float   out;
  void  (*init)(struct ramp_t *ramp, int32_t scale);
  float (*calc)(struct ramp_t *ramp);
	float (*kb_calc)(struct ramp_t *ramp, uint8_t dir, int32_t scale);
	float (*dutched_calc)(struct ramp_t *ramp, uint8_t dir, int32_t scale);
	
}ramp_t;
         
#define RAMP_GEN_DAFAULT               \
{                                      \
   .count = 0,                         \
   .scale = 0,                         \
   .out = 0,                           \
   .init = &ramp_init,                 \
   .calc = &ramp_calc,                 \
	 .kb_calc = &ramp_kb_calc,           \
   .dutched_calc = &ramp_dutched_calc, \
}                                      \

void  ramp_init(ramp_t *ramp, int32_t scale);
float ramp_calc(ramp_t *ramp);

float ramp_kb_calc(ramp_t *ramp, uint8_t dir, int32_t scale);
float ramp_dutched_calc(ramp_t *ramp, uint8_t dir, int32_t scale);


extern ramp_t fb_ramp;
extern ramp_t lr_ramp; 

extern ramp_t rd_run_ramp;
extern ramp_t rd_off_ramp;
extern ramp_t ld_run_ramp;
extern ramp_t ld_off_ramp;

extern ramp_t reset_lifter_spd_ramp;
extern ramp_t reset_bomb_claw_spd_ramp;
extern ramp_t reset_trailer_spd_ramp;

extern ramp_t landing_angle_ramp;

extern ramp_t turn_around_ramp;
extern ramp_t turn_left_ramp;
extern ramp_t turn_right_ramp;

extern ramp_t spin_brake_ramp;

#endif
