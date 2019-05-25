#ifndef __JUDGEMENT_H
#define __JUDGEMENT_H

#include "system.h"

//#define send_max_len     200
//#define receive_max_len  200

#define UART5_TX_BUF_LEN        200
#define UART5_DMA_RX_BUF_LEN    200

#define CMD_ID_SIZE 	 		 2
#define FRAME_TAIL_SIZE 	 2

#define BLUE  0
#define RED   1

enum Judge_CmdID
{ 
	Game_State 								= 0x0001,		//比赛数据信息 0x0001  发送频率:1Hz
	Game_Result 							= 0x0002,		//比赛结果数据:0x0002   比赛结束后发送
	Game_Robot_Survivors			= 0x0003,		//机器人存活数据:0x0003   发送频率:1Hz
	Event_Data 								= 0x0101,		//场地时间数据：0x0101  发送频率：事件改变后发送
	Supply_Projectile_Action	= 0x0102,		//补给站动作标识：0x0102  发送频率：动作改变后发送
	Supply_Orojectile_Booking = 0x0103,   //补给站预约子弹：cmd_id(0x0103) 发送频率：上限10Hz RM对抗赛尚未开放
	Game_Robot_State					= 0x0201,		//比赛机器人状态：0x0201  发送频率:10Hz
	Power_Heat_Data						= 0x0202,		//实时功率热量数据:0x0202  发送频率:50Hz
	Game_Robot_Pos						= 0x0203,		//机器人位置：0x0203  发送频率:10Hz
	Buff_Musk									= 0x0204,		//机器人增益：0x0204   发送频率：状态改变后发送
	Aerial_Robot_Energy				=	0x0205,		//空中机器人能量状态：0x0205 发送频率:10Hz
	Robot_Hurt								= 0x0206,		//伤害状态:0x0206  发送频率：伤害发生后发送
	Shoot_Data								= 0x0207,		//实时射击信息:0x0207 发送频率：射击后发送
	Teammate_Data             = 0x0301,   //接受队友信息
};

//机器人交互信息发送的ID  自行定义
enum InteractiveStory
{
	GOLF_NUM_FOR_HERO         =  0x0203,  //发送弹量给英雄
};

//帧头
typedef __packed struct
{
	uint8_t 	SQF; 			 //数据帧起始字节，固定值为0xA5
	uint16_t 	DataLength;//数据帧内Data长度
	uint8_t 	Seq;			 //包序号
	uint8_t 	CRC8;			 //帧头CRC校验
}extFrameHeader;

//比赛数据信息 0x0001  发送频率:1Hz
typedef __packed struct
{
 uint8_t  game_type : 4;
 uint8_t  game_progress : 4;
 uint16_t stage_remain_time;
}ext_game_state_t;

//比赛结果数据:0x0002   比赛结束后发送
typedef __packed struct
{
 uint8_t winner;
}ext_game_result_t;

//机器人存活数据:0x0003   发送频率:1Hz
typedef __packed struct
{
 uint16_t robot_legion;
} ext_game_robot_survivors_t;

//场地时间数据：0x0101  发送频率：事件改变后发送
typedef __packed struct
{
	uint32_t event_type;
} ext_event_data_t;

//补给站动作标识：0x0102  发送频率：动作改变后发送
typedef __packed struct
{
 uint8_t supply_projectile_id;
 uint8_t supply_robot_id;
 uint8_t supply_projectile_step;
 uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

//补给站预约子弹：cmd_id(0x0103) 发送频率：上限10Hz RM对抗赛尚未开放
typedef __packed struct
{
 uint8_t supply_projectile_id;
 uint8_t supply_robot_id;
 uint8_t supply_num;
} ext_supply_projectile_booking_t;


//比赛机器人状态：0x0201  发送频率:10Hz
typedef __packed struct
{
 uint8_t  robot_id;
 uint8_t  robot_level;
 uint16_t remain_HP;
 uint16_t max_HP;
 uint16_t shooter_heat0_cooling_rate;
 uint16_t shooter_heat0_cooling_limit;
 uint16_t shooter_heat1_cooling_rate;
 uint16_t shooter_heat1_cooling_limit;
 uint8_t  mains_power_gimbal_output  : 1;
 uint8_t  mains_power_chassis_output : 1;
 uint8_t  mains_power_shooter_output : 1;
} ext_game_robot_state_t;

//实时功率热量数据:0x0202  发送频率:50Hz
typedef __packed struct
{
 uint16_t chassis_volt;
 uint16_t chassis_current;
 float    chassis_power;
 uint16_t chassis_power_buffer;
 uint16_t shooter_heat0;
 uint16_t shooter_heat1; 
}ext_power_heat_data_t;

//机器人位置：0x0203  发送频率:10Hz
typedef __packed struct
{
 float x;
 float y;
 float z;
 float yaw;
}ext_game_robot_pos_t;

//机器人增益：0x0204   发送频率：状态改变后发送
typedef __packed struct
{
 uint8_t power_rune_buff;
}ext_buff_musk_t;

//空中机器人能量状态：0x0205 发送频率:10Hz
typedef __packed struct
{
 uint8_t energy_point;
 uint8_t attack_time;
}aerial_robot_energy_t;

//伤害状态:0x0206  发送频率：伤害发生后发送
typedef __packed struct
{
 uint8_t armor_id  : 4;
 uint8_t hurt_type : 4;
}ext_robot_hurt_t;

//实时射击信息:0x0207 发送频率：射击后发送
typedef __packed struct
{
 uint8_t bullet_type;
 uint8_t bullet_freq;
 float   bullet_speed;
}ext_shoot_data_t;

/*机器人交互数据*/
//交互数据接收信息:0x0301  发送频率:上限10Hz
typedef __packed struct
{
 uint16_t data_cmd_id;  //内容ID
 uint16_t send_ID;
 uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

//内容ID           由于存在多个内容ID,但整个cmd_id上行频率最大是10Hz  注意合理安排带宽
/*内容ID           长度                        功能        				 */
/*                 (头结构长度+内容数据长度) 	 	 									 */
/*0xD180            6+13 											 客户端自定义数据		 */
/*0X0200~0X02FF		  6+n												 己方机器人间通信		 */


//上传客户端自定义数据   cmd_id:0x0301   内容id:0xD180   发送频率:上限10Hz
typedef __packed struct
{
	float   data1;/*用于显示机器人状态 
	                最后一位:1机械模式 2陀螺仪模式  
	                前五位:00000X常规模式 11111X登岛模式 22222X取弹模式 33333X补弹模式 44444X救援模式*/
	float   data2;/*用于显示多箱模式切换 
	                单箱11111X 三箱33333X 五箱55555X 其他00000X*/
	float   data3;/*用于显示弹仓弹量 
	                岛上完成一次取弹动作+20 岛下完成一次取弹动作+6*/
	uint8_t masks;/*全亮111111 间隙亮101010 全灭000000*/

}client_custom_data_t;

//学生间机器人通信  cmd_id:0x0301  内容id：0x0201~0x02FF   发送频率：上限10Hz
typedef __packed struct
{
	uint8_t  data_frame[10];//小于113字节
}robot_interactive_data_t;

//帧头  命令码  数据段头结构  数据段   帧尾
//上传客户端
typedef __packed struct
{
	extFrameHeader                          txFrameHeader;//帧头
	uint16_t		                            CmdID;//命令码
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	client_custom_data_t  	                clientData;//数据段
	uint16_t		                            FrameTail;//帧尾
}exSendClientData_t;

//机器人交互信息
typedef __packed struct
{
	extFrameHeader                          txFrameHeader;//帧头
	uint16_t		                            CmdID;//命令码
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	robot_interactive_data_t  	            interactData;//数据段
	uint16_t		                            FrameTail;//帧尾
}exCommunatianData_t;

//结受队友信息
typedef __packed struct
{
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	robot_interactive_data_t  	            interactData;//数据段
}ext_teammate_data_t;

/*--------------------------------- 自定义 -------------------------------*/
/*execute_task*/
#define NORMAL_TASK           (float)000000
#define CLIMB_TUBE_OPTIC_TASK (float)111110 
#define CLIMB_TUBE_GYRO_TASK  (float)101010 
#define GET_BOMB_TASK         (float)222220
#define FEED_BOMB_TASK        (float)333330
#define RESCUE_TASK           (float)444440

#define SEPARATE_CHASSIS      (float)1     
#define FOLLOW_CHASSIS        (float)2	   
#define UNCONTROLLED_CHASSIS  (float)9     
/*get_bomb_mode*/
#define GET_NO_CASE           (float)000000
#define GET_SINGLE_CASE       (float)111110
#define GET_TRIPLE_CASE       (float)333330
#define GET_PENTA_CASE        (float)555550
	
#define LOCATED_ON_GROUND     (float)0     
#define LOCATED_ON_ISLAND     (float)1     
#define ROBOT_IN_DANGER       (float)999999
/*status_bar*/
#define ALL_GREEN           (uint8_t)(7<<3|7)
#define CRISS_CROSS         (uint8_t)(5<<3|2)
#define HALF_HALF           (uint8_t)(0<<3|7)
#define ALL_RED             (uint8_t)(0<<3|0)
/*for status_bar dynamic display*/
#define SENTRY_MAYDAY          0 //哨兵求救
#define RESETING               1 //正在复位
#define MOTOR_OFFLINE          2 //电机离线

//#define A_G           0 //all green
//#define C_C           1 //criss cross
//#define H_H           2 //half half
//#define A_R           3 //all red

//上传客户端自定义数据   cmd_id:0x0301   内容id:0xD180   发送频率:上限10Hz
typedef __packed struct
{
	float   execute_task;             /*用于显示机器人状态  
	                                    前三位:111机械模式 222陀螺仪模式 333不可控模式
	                                    后三位:000常规模式 111登岛模式 222取弹模式 333补弹模式 444救援模式*/
	float   get_bomb_mode;            /*用于显示多箱模式切换 
	                                    单箱111111 三箱333333 五箱555555 其他000000*/
	float   golf_num;                 /*用于显示弹仓弹量 
	                                    岛上完成一次取弹动作+20 岛下完成一次取弹动作+6*/
	bool    golf_num_add_once;        /*加法每次只加一次*/
	
	bool    status_bar_record[3];     /*暂存*/
	
	int32_t preemption_time;          /*抢占时间*/
	
	bool    suspend_flag;             /*悬起标志位*/
	
	bool    timing_flag;
	
	bool    preemption_time_record;   /*重启时间记录*/

	uint8_t status_bar;               /*开了视觉全亮111111 扫描间隙亮101010 其他全灭000000*/

}data_for_client_t;

extern data_for_client_t data_for_client;

extern unsigned char _UART5_DMA_RX_BUF[UART5_DMA_RX_BUF_LEN];

/*--------------------------------- 函数声明 -------------------------------*/
void judgement_init(void);
void receive_data_from_judgement(unsigned char *JudgementData);
void send_to_teammate(void);
void send_to_client(void);
bool is_red_or_blue(void);
void confirm_ID(void);
void judgement_send_task(void);

/*来自CRC的函数申明*/
void CRC_init(void);
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);

#endif

