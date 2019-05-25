/****************************************************************************
 *  Copyright (C) 2019 Cancer.Chen
 ***************************************************************************/
/** @file remote_ctrl.h
 *  @version 1.0
 *  @date June 2019
 *
 *  @brief remote control message handle
 *
 *  @copyright 2019 Cancer.Chen All rights reserved.
 *
 */
 
#ifndef __REMOTE_CTRL_H__
#define __REMOTE_CTRL_H__

#include "bsp_sys.h"


/** 
  * @brief  remote control information
  */
typedef __packed struct
{	
	bool rc_enable;
  /* rocker channel information */
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;
  /* left and right lever information */
  uint8_t sw1;
  uint8_t sw2;
  /* mouse movement and button information */
  __packed struct
  {
    int16_t x;
    int16_t y;
    int16_t z;
  
    uint8_t l:1;
    uint8_t r:1;
  } mouse;
  /* keyboard key information */
  __packed union
  {
    uint16_t key_code;
    __packed struct 
    {
      uint16_t W:1;
      uint16_t S:1;
      uint16_t A:1;
      uint16_t D:1;
      uint16_t SHIFT:1;
      uint16_t CTRL:1;
      uint16_t Q:1;
      uint16_t E:1;
      uint16_t R:1;
      uint16_t F:1;
      uint16_t G:1;
      uint16_t Z:1;
      uint16_t X:1;
      uint16_t C:1;
      uint16_t V:1;
      uint16_t B:1;
    } bit;
  } kb;
} rc_info_t;

typedef struct
{
  float vx;
  float vy;
  float vw;
  
//  float pit_v;
//  float yaw_v;

	
} rc_ctrl_t;



enum
{
  RC_UP = 1,
  RC_MI = 3,
  RC_DN = 2,
};

///* control operation definition */
////      shoot relevant      remote control operation
//#define RC_SINGLE_SHOOT    ((glb_sw.last_sw1 == RC_MI) && (rc.sw1 == RC_DN))
//#define RC_CONTINUE_SHOOT  (rc.sw1 == RC_DN)
//#define RC_CTRL_FRIC_WHEEL ((glb_sw.last_sw1 == RC_MI) && (rc.sw1 == RC_UP))

//typedef struct
//{
//  uint8_t last_sw1;
//  uint8_t last_sw2;
//  uint32_t stable_time_sw1;
//  uint32_t stable_time_sw2;
//} sw_record_t;





extern uint8_t     dbus_buf[];
extern rc_info_t   rc;
extern rc_ctrl_t   rm;
//extern sw_record_t glb_sw;


void rc_callback_handler(rc_info_t *rc, uint8_t *buff);

void remote_ctrl_chassis_handler(void);
//void remote_ctrl_gimbal_hook(void);
//void remote_ctrl_shoot_hook(void);



#endif

