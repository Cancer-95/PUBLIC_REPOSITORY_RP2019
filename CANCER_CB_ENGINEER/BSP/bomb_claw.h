#ifndef __BOMB_CLAW_H__
#define __BOMB_CLAW_H__

#include "system.h"

#define NO			          0
#define YES			          1

#define R_axis			      0
#define X_axis			      1
#define Y_axis			      2

typedef enum
{
	R_CLAW_ZERO 		      = 0,//
	R_CLAW_BIT            = 1,//
	R_CLAW_HALF           = 2,//
	R_CLAW_ENTIRE         = 3,//
	
} claw_r_mode_e;

typedef enum
{
	X_CLAW_ZERO 		      = 0,//
	X_CLAW_BIT            = 1,//
	X_CLAW_HALF           = 2,//
	X_CLAW_ENTIRE         = 3,//
	
} claw_x_mode_e;

typedef enum
{
	Y_CLAW_ZERO 		      = 0,//
  Y_CLAW_HALF           = 1,//把它排在0位
	Y_CLAW_ENTIRE         = 2,//
	Y_CLAW_STOP           = 3,//原地停住

} claw_y_mode_e;

typedef struct
{
  bool              ALL_reset;
	bool              LR_reset;
	bool              Y_reset;
	int16_t           reset_speed[4];          //复位速度
	int16_t           target_speed[4];         //temp
	
	bool              across_stall_protect[4];
	bool              stall_protect_record[4]; 
  uint32_t          stall_protect_time[4];   //
	
	uint8_t           relax_trigger[4];        //触发泄力的变量
	
	bool              LR_stall_time_record;    //堵转时间标记
	uint32_t          LR_stall_time;           //堵转时间

	bool              Y_stall_time_record;     //堵转时间标记
	uint32_t          Y_stall_time;            //堵转时间
	
	uint16_t          stall_times[4];          //堵转次数

	bool              reset_start_time_record; //时间标记
	uint32_t          reset_start_time;        //复位堵转前延时一段时间越过启动
	
	claw_r_mode_e     R_mode;
	claw_x_mode_e     X_mode;
	claw_y_mode_e     Y_mode;
	
	int16_t           speed_fdb[4];
  int16_t           speed_ref[4];
	int16_t           angle_fdb[4];
  int16_t           angle_ref[4];
  int16_t           current[4];
	
	uint16_t          offset_ecd[4]; //电机初始角度偏差
	uint16_t          cur_ecd[4];    //电机当前时刻读数
  uint16_t          las_ecd[4];    //电机上一时刻读数
	bool              ecd_Record[4]; //单次记录误差标志位
	
	int32_t           round_cnt[4];       //圈数
	int32_t           total_ecd[4];       //总读数
	int32_t           total_angle[4];     //总角度换算值
	 
  int32_t           target_angle[4];    //temp
	
	int32_t           zero_stall_point[2];//翻转堵转点
	int32_t           zero_brake_point[2];//翻转刹车点
	bool              zero_correct;       //标记堵转完成
	bool              zero_correct_hold;  //放弃第一次
	
	int32_t           target_entire[4];   //final
	int32_t           target_half[4];     //final
  int32_t           target_bit[3];      //final(没有Y_axis)
  int32_t           target_zero[4];     //final

  int32_t           Y_target[3];        //Y轴电机的三个位置
	
	/*三个轴的各种标志位*/
  bool              from_claw_zero[3];
	bool              from_claw_bit[2];   //(没有Y_axis)
  bool              from_claw_half[2];  //(没有Y_axis)
	bool              from_claw_entire[3];
	
	bool              flag_claw_zero[3];
	bool              flag_claw_bit[2];   //(没有Y_axis)
  bool              flag_claw_half[3];
	bool              flag_claw_entire[3];

	uint32_t          ZERO_time;   //时间
//  bool              flag_working_mode;
	
} bomb_claw_t;



void bomb_claw_param_init(void);
void bomb_claw_data_handler(u8 k);
void bomb_claw_task(void);

extern bomb_claw_t bomb_claw;
#endif

