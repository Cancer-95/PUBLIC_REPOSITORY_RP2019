#ifndef __KEYBOARD_CTRL_H__
#define __KEYBOARD_CTRL_H__

#include "bsp_sys.h"

//      direction key
#define FORWARD               (rc.kb.bit.W)
#define BACK                  (rc.kb.bit.S)
#define LEFT                  (rc.kb.bit.A)
#define RIGHT                 (rc.kb.bit.D)
//      speed key
#define FAST_SPD              (rc.kb.bit.SHIFT)
#define SLOW_SPD              (rc.kb.bit.CTRL)
//      force speed key
#define FAST_SPD_ENABLE       1
#define SLOW_SPD_ENABLE       1
#define FAST_SPD_DISABLE      0
#define SLOW_SPD_DISABLE      0
//      climb_mode key
#define RUN_PLAIN             (rc.kb.bit.Z && rc.kb.bit.CTRL) //CTRL+Z 平原模式
#define LIFT_UP               (rc.kb.bit.Q && rc.kb.bit.CTRL) //CTRL+Q 爬杆模式
//      tube claw ctrl
#define TUBE_CLAW_CATCH_CTRL  (rc.kb.mouse.r)                 //鼠标右键控制爬杆爪子
//      turn_around 
#define TURN_AROUND           (rc.kb.bit.C)
#define TURN_AROUND_DISABLE   0
//      get_bomb key
#define GET_BOMB_CTRL         (rc.kb.bit.E && rc.kb.bit.CTRL) //CTRL+E 进入退出取弹模式
//      bomb_claw ctrl
#define BOMB_CLAW_ROLL_CTRL   (rc.kb.mouse.l)                 //鼠标左键控制爬杆爪子
#define BOMB_CLAW_CATCH_CTRL  (rc.kb.mouse.r)                 //鼠标右键控制取弹爪子
#define BOMB_CLAW_PUSH_CTRL   (rc.kb.bit.R && rc.kb.bit.CTRL) //CTRL+F 前排后排选择
#define BOMB_CLAW_TRIPLE_CTRL (rc.kb.bit.F && rc.kb.bit.CTRL) //CTRL+F 切换单三
//      rescue ctrl
#define RESCUE_CTRL           (rc.kb.bit.X && rc.kb.bit.CTRL) //CTRL+X 进入退出救援模式
#define TRAILER_HOOK_CTRL     (rc.kb.mouse.l)                 //鼠标左键手动控制钩子
#define HOOK_CONFIRM_CTRL     (rc.kb.mouse.r)                 //鼠标右键确认钩住
//      feed_bomb_ctrl
#define FEED_BOMB_CTRL        (rc.kb.bit.G && rc.kb.bit.CTRL)  //CTRL+G 进入退出补给模式
#define MAGAZINE_CTRL         (rc.kb.mouse.l)                  //鼠标左键手动控制弹药箱出口开合

typedef enum 
{
  NORMAL_SPEED = 0, //正常速度置于初始状态
  FAST_SPEED   = 1,
  SLOW_SPEED   = 2,
} kb_chassis_speed;

typedef __packed  struct
{
  float vx;
  float vy;
  float vw; 
  
  bool kb_enable;
	
	bool tube_claw_enable;	//爬杆爪子使能位
	bool bomb_claw_enable;	//取弹爪子使能位

  kb_chassis_speed move_speed;
  
	uint16_t x_spd_limit; //X方向速度限制 (键盘)
  uint16_t y_spd_limit; //Y方向速度限制 (键盘)
	uint16_t w_spd_limit; //W方向速度限制 (键盘)

} kb_ctrl_t;

extern kb_ctrl_t kb;

extern bool flag_tube_claw_catch;   //relay.h

extern bool flag_bomb_claw_catch;   //relay.h
extern bool flag_bomb_claw_roll;    //get_bomb.c
extern bool flag_bomb_claw_push;    //relay.h
extern bool flag_bomb_claw_triple;
extern bool flag_get_bomb;          //get_bomb.c
 
extern bool flag_trailer_hook;      //relay.h 
extern bool flag_magazine;          //servo.h


void keyboard_chassis_handler(void);//底盘键盘控制

void keyboard_climb_tube_handler(void);//爬杆键盘控制

void keyboard_get_bomb_handler(void);//取弹键盘控制

void keyboard_rescue_handler(void);//救援键盘控制

void keyboard_feed_bomb_handler(void);//补弹键盘控制

#endif

