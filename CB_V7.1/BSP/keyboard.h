#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "system.h"

//      direction key
#define FORWARD               (KEY_W_PRESSED ? 1 : 0)
#define BACK                  (KEY_S_PRESSED ? 1 : 0)
#define LEFT                  (KEY_A_PRESSED ? 1 : 0)
#define RIGHT                 (KEY_D_PRESSED ? 1 : 0)
//      speed key
#define FAST_SPD              (KEY_SHIFT_PRESSED ? 1 : 0)
#define SLOW_SPD              (KEY_CTRL_PRESSED ? 1 : 0)
//      force speed key
#define FAST_SPD_ENABLE       1
#define SLOW_SPD_ENABLE       1
#define FAST_SPD_DISABLE      0
#define SLOW_SPD_DISABLE      0
//      if_gyro_blow_up
#define GYRO_BLOW_UP_CTRL     ((KEY_SHIFT_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+SHIFT 如果陀螺仪炸了取消陀螺仪模式
//      climb_mode key
#define RUN_PLAIN             ((KEY_Z_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED)  //CTRL+Z   平原模式
#define CLIMB_UP_OPTIC        ((KEY_Q_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED)  //CTRL+Q   爬杆模式
#define CLIMB_DOWN_OPTIC      ((KEY_E_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED)  //CTRL+E   下杆模式
#define CLIMB_UP_GYRO         ((KEY_Q_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED) //SHIFT+Q  爬杆模式
#define CLIMB_DOWN_GYRO       ((KEY_E_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED) //SHIFT+E  下杆模式
//      tube claw ctrl
#define TUBE_CLAW_CATCH_CTRL  (MOUSE_R_PRESSED ? 1 : 0) //鼠标右键控制爬杆爪子
//      turn_around 
#define TURN_AROUND           ((KEY_C_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED)//CTRL+C 原地旋转180
#define TURN_AROUND_DISABLE   0
//      spin_around 
#define SPIN_AROUND           ((KEY_C_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED) //SHIFT+C 原地旋转
#define SPIN_AROUND_DISABLE   0
//      get_bomb key
#define GET_BOMB_CTRL         ((KEY_R_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+R 进入退出取弹模式
#define GET_BOMB_CLEAR_CTRL   ((KEY_Z_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+Z 纠错重来的按键
//      bomb_claw ctrl
#define BOMB_CLAW_ROLL_CTRL   (MOUSE_L_PRESSED ? 1 : 0)                     //鼠标左键控制爬杆爪子
#define BOMB_CLAW_CATCH_CTRL  (MOUSE_R_PRESSED ? 1 : 0)                     //还是鼠标右键控制爬杆爪子
#define BOMB_CLAW_PUSH_CTRL   ((KEY_V_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+V 前排后排选择
#define BOMB_CLAW_TRIPLE_CTRL ((KEY_F_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+F 切换单三
#define BOMB_CLAW_PENTA_CTRL  ((KEY_F_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED)//SHIFT+F 切换单五
#define BOMB_CLAW_SCAN_CTRL   ((KEY_R_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED)//SHIFT+R 自动扫描的控制
#define BOMB_CLAW_VISION_CTRL ((KEY_V_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED)//SHIFT+V 视觉辅助取弹
////      supporting_wheel ctrl
//#define SUPPORTING_WHEEL_CTRL ((KEY_F_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED)//SHIFT+F 辅助支撑腿的控制
//      rescue ctrl
#define RESCUE_CTRL           ((KEY_X_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+X 进入退出救援模式
#define RESCUE_SIGHT_CTRL     (MOUSE_L_PRESSED ? 1 : 0)                     // 手动切换视角
#define TRAILER_HOOK_CTRL     (MOUSE_R_PRESSED ? 1 : 0)                     // 手动控制钩子
#define HOOK_CONFIRM_CTRL     ((KEY_F_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) // 确认钩住
//      feed_bomb_ctrl
#define FEED_BOMB_CTRL        ((KEY_G_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) //CTRL+G 进入退出补给模式
#define FEED_BOMB_SIGHT_CTRL  (MOUSE_L_PRESSED ? 1 : 0)                     // 手动切换视角
#define MAGAZINE_LIFTER_CTRL  (MOUSE_R_PRESSED ? 1 : 0)                     // 手动控制弹药箱框架抬升
#define MAGAZINE_EXIT_CTRL    ((KEY_F_PRESSED ? 1 : 0) && KEY_CTRL_PRESSED) // 手动控制弹药箱出口开合
#define FEED_BOMB_VISION_CTRL ((KEY_V_PRESSED ? 1 : 0) && KEY_SHIFT_PRESSED)//SHIFT+V 视觉辅助补弹

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

/*用于任务清零*/
typedef __packed  struct
{
  bool climb_tube_task_enable;
	bool get_bomb_task_enable;	
	bool rescue_task_enable;	
	bool feed_bomb_task_enable;	
} global_task_t;

extern kb_ctrl_t kb;
extern global_task_t global_task;

extern bool flag_spin_around;       //chassis.h

extern bool flag_tube_claw_catch;   //给relay.h中的宏准备

extern bool flag_bomb_claw_catch;   //给relay.h中的宏准备
extern bool flag_bomb_claw_roll;    //给get_bomb.c准备
extern bool flag_bomb_claw_push;    //给relay.h中的宏准备
extern bool flag_bomb_claw_triple;
extern bool flag_bomb_claw_penta;
extern bool flag_bomb_claw_vision;
extern bool flag_bomb_claw_scan;           
//extern bool flag_get_bomb;          //给get_bomb.c切换case用

extern bool get_bomb_clear_record;    //退出执行纠错程序

extern bool flag_trailer_hook;        //给relay.h中的宏准备  
extern bool flag_rescue_sight;

extern bool flag_magazine_exit;          //给servo.h中的宏准备  
extern bool flag_magazine_lifter;       
extern bool flag_feed_bomb_vision;
extern bool flag_feed_bomb_sight;
//extern bool flag_supporting_wheel;  //给relay.h中的宏准备


extern bool flag_gyro_blow_up;

void global_task_init(void);

void keyboard_chassis_handler(void);//键盘底盘控制

void keyboard_climb_tube_handler(void);//爬杆键盘控制

void keyboard_get_bomb_handler(void);//取弹键盘控制

void keyboard_rescue_handler(void);//救援键盘控制

void keyboard_feed_bomb_handler(void);//补弹键盘控制

#endif

