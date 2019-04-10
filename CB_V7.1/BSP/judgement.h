#ifndef __JUDGEMENT_H
#define __JUDGEMENT_H

#include "system.h"

#define CMD_ID_SIZE 	 		 2
#define FRAME_TAIL_SIZE 	 2

#define BLUE  0
#define RED   1

enum Judge_CmdID
{ 
	Game_State 								= 0x0001,		//±ÈÈüÊı¾İĞÅÏ¢ 0x0001  ·¢ËÍÆµÂÊ:1Hz
	Game_Result 							= 0x0002,		//±ÈÈü½á¹ûÊı¾İ:0x0002   ±ÈÈü½áÊøºó·¢ËÍ
	Game_Robot_Survivors			= 0x0003,		//»úÆ÷ÈË´æ»îÊı¾İ:0x0003   ·¢ËÍÆµÂÊ:1Hz
	Event_Data 								= 0x0101,		//³¡µØÊ±¼äÊı¾İ£º0x0101  ·¢ËÍÆµÂÊ£ºÊÂ¼ş¸Ä±äºó·¢ËÍ
	Supply_Projectile_Action	= 0x0102,		//²¹¸øÕ¾¶¯×÷±êÊ¶£º0x0102  ·¢ËÍÆµÂÊ£º¶¯×÷¸Ä±äºó·¢ËÍ
	Supply_Orojectile_Booking = 0x0103,   //²¹¸øÕ¾Ô¤Ô¼×Óµ¯£ºcmd_id(0x0103) ·¢ËÍÆµÂÊ£ºÉÏÏŞ10Hz RM¶Ô¿¹ÈüÉĞÎ´¿ª·Å
	Game_Robot_State					= 0x0201,		//±ÈÈü»úÆ÷ÈË×´Ì¬£º0x0201  ·¢ËÍÆµÂÊ:10Hz
	Power_Heat_Data						= 0x0202,		//ÊµÊ±¹¦ÂÊÈÈÁ¿Êı¾İ:0x0202  ·¢ËÍÆµÂÊ:50Hz
	Game_Robot_Pos						= 0x0203,		//»úÆ÷ÈËÎ»ÖÃ£º0x0203  ·¢ËÍÆµÂÊ:10Hz
	Buff_Musk									= 0x0204,		//»úÆ÷ÈËÔöÒæ£º0x0204   ·¢ËÍÆµÂÊ£º×´Ì¬¸Ä±äºó·¢ËÍ
	Aerial_Robot_Energy				=	0x0205,		//¿ÕÖĞ»úÆ÷ÈËÄÜÁ¿×´Ì¬£º0x0205 ·¢ËÍÆµÂÊ:10Hz
	Robot_Hurt								= 0x0206,		//ÉËº¦×´Ì¬:0x0206  ·¢ËÍÆµÂÊ£ºÉËº¦·¢Éúºó·¢ËÍ
	Shoot_Data								= 0x0207,		//ÊµÊ±Éä»÷ĞÅÏ¢:0x0207 ·¢ËÍÆµÂÊ£ºÉä»÷ºó·¢ËÍ
};

//»úÆ÷ÈË½»»¥ĞÅÏ¢·¢ËÍµÄID  ×ÔĞĞ¶¨Òå
enum InteractiveStory
{
	Supply_Loading     =  0x0200,//²¹µ¯
	Supply_Unavailable =  0x0201,//Ã»µ¯
	Supply_Available   =  0x0202,//ÓĞµ¯
};

//Ö¡Í·
typedef __packed struct
{
	uint8_t 	SQF; 			 //Êı¾İÖ¡ÆğÊ¼×Ö½Ú£¬¹Ì¶¨ÖµÎª0xA5
	uint16_t 	DataLength;//Êı¾İÖ¡ÄÚData³¤¶È
	uint8_t 	Seq;			 //°üĞòºÅ
	uint8_t 	CRC8;			 //Ö¡Í·CRCĞ£Ñé
}extFrameHeader;

//±ÈÈüÊı¾İĞÅÏ¢ 0x0001  ·¢ËÍÆµÂÊ:1Hz
typedef __packed struct
{
 uint8_t  game_type : 4;
 uint8_t  game_progress : 4;
 uint16_t stage_remain_time;
}ext_game_state_t;

//±ÈÈü½á¹ûÊı¾İ:0x0002   ±ÈÈü½áÊøºó·¢ËÍ
typedef __packed struct
{
 uint8_t winner;
}ext_game_result_t;

//»úÆ÷ÈË´æ»îÊı¾İ:0x0003   ·¢ËÍÆµÂÊ:1Hz
typedef __packed struct
{
 uint16_t robot_legion;
} ext_game_robot_survivors_t;

//³¡µØÊ±¼äÊı¾İ£º0x0101  ·¢ËÍÆµÂÊ£ºÊÂ¼ş¸Ä±äºó·¢ËÍ
typedef __packed struct
{
	uint32_t event_type;
} ext_event_data_t;

//²¹¸øÕ¾¶¯×÷±êÊ¶£º0x0102  ·¢ËÍÆµÂÊ£º¶¯×÷¸Ä±äºó·¢ËÍ
typedef __packed struct
{
 uint8_t supply_projectile_id;
 uint8_t supply_robot_id;
 uint8_t supply_projectile_step;
 uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

//²¹¸øÕ¾Ô¤Ô¼×Óµ¯£ºcmd_id(0x0103) ·¢ËÍÆµÂÊ£ºÉÏÏŞ10Hz RM¶Ô¿¹ÈüÉĞÎ´¿ª·Å
typedef __packed struct
{
 uint8_t supply_projectile_id;
 uint8_t supply_robot_id;
 uint8_t supply_num;
} ext_supply_projectile_booking_t;


//±ÈÈü»úÆ÷ÈË×´Ì¬£º0x0201  ·¢ËÍÆµÂÊ:10Hz
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
 uint8_t  mains_power_gimbal_output : 1;
 uint8_t  mains_power_chassis_output : 1;
 uint8_t  mains_power_shooter_output : 1;
} ext_game_robot_state_t;

//ÊµÊ±¹¦ÂÊÈÈÁ¿Êı¾İ:0x0202  ·¢ËÍÆµÂÊ:50Hz
typedef __packed struct
{
 uint16_t chassis_volt;
 uint16_t chassis_current;
 float    chassis_power;
 uint16_t chassis_power_buffer;
 uint16_t shooter_heat0;
 uint16_t shooter_heat1; 
}ext_power_heat_data_t;

//»úÆ÷ÈËÎ»ÖÃ£º0x0203  ·¢ËÍÆµÂÊ:10Hz
typedef __packed struct
{
 float x;
 float y;
 float z;
 float yaw;
}ext_game_robot_pos_t;

//»úÆ÷ÈËÔöÒæ£º0x0204   ·¢ËÍÆµÂÊ£º×´Ì¬¸Ä±äºó·¢ËÍ
typedef __packed struct
{
 uint8_t power_rune_buff;
}ext_buff_musk_t;

//¿ÕÖĞ»úÆ÷ÈËÄÜÁ¿×´Ì¬£º0x0205 ·¢ËÍÆµÂÊ:10Hz
typedef __packed struct
{
 uint8_t energy_point;
 uint8_t attack_time;
}aerial_robot_energy_t;

//ÉËº¦×´Ì¬:0x0206  ·¢ËÍÆµÂÊ£ºÉËº¦·¢Éúºó·¢ËÍ
typedef __packed struct
{
 uint8_t armor_id : 4;
 uint8_t hurt_type : 4;
}ext_robot_hurt_t;

//ÊµÊ±Éä»÷ĞÅÏ¢:0x0207 ·¢ËÍÆµÂÊ£ºÉä»÷ºó·¢ËÍ
typedef __packed struct
{
 uint8_t bullet_type;
 uint8_t bullet_freq;
 float   bullet_speed;
}ext_shoot_data_t;

/*»úÆ÷ÈË½»»¥Êı¾İ*/
//½»»¥Êı¾İ½ÓÊÕĞÅÏ¢:0x0301  ·¢ËÍÆµÂÊ:ÉÏÏŞ10Hz
typedef __packed struct
{
 uint16_t data_cmd_id;  //ÄÚÈİID
 uint16_t send_ID;
 uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

//ÄÚÈİID           ÓÉÓÚ´æÔÚ¶à¸öÄÚÈİID,µ«Õû¸öcmd_idÉÏĞĞÆµÂÊ×î´óÊÇ10Hz  ×¢ÒâºÏÀí°²ÅÅ´ø¿í
/*ÄÚÈİID           ³¤¶È                        ¹¦ÄÜ        				 */
/*                 (Í·½á¹¹³¤¶È+ÄÚÈİÊı¾İ³¤¶È) 	 	 									 */
/*0xD180            6+13 											 ¿Í»§¶Ë×Ô¶¨ÒåÊı¾İ		 */
/*0X0200~0X02FF		  6+n												 ¼º·½»úÆ÷ÈË¼äÍ¨ĞÅ		 */


//ÉÏ´«¿Í»§¶Ë×Ô¶¨ÒåÊı¾İ   cmd_id:0x0301   ÄÚÈİid:0xD180   ·¢ËÍÆµÂÊ:ÉÏÏŞ10Hz
typedef __packed struct
{
	float   data1;/*ÓÃÓÚÏÔÊ¾»úÆ÷ÈË×´Ì¬ 
	                Ç°ÈıÎ»:000»úĞµÄ£Ê½ 111ÍÓÂİÒÇÄ£Ê½  
	                ºóÈıÎ»:000³£¹æÄ£Ê½ 111µÇµºÄ£Ê½ 222È¡µ¯Ä£Ê½ 333²¹µ¯Ä£Ê½ 444¾ÈÔ®Ä£Ê½*/
	float   data2;/*ÓÃÓÚÏÔÊ¾¶àÏäÄ£Ê½ÇĞ»» 
	                µ¥Ïä111111 ÈıÏä333333 ÎåÏä555555 ÆäËû000000*/
	float   data3;/*ÓÃÓÚÏÔÊ¾µ¯²Öµ¯Á¿ 
	                µºÉÏÍê³ÉÒ»´ÎÈ¡µ¯¶¯×÷+20 µºÏÂÍê³ÉÒ»´ÎÈ¡µ¯¶¯×÷+6*/
	uint8_t masks;/*¿ªÁËÊÓ¾õÈ«ÁÁ111111 É¨Ãè¼äÏ¶ÁÁ101010 ÆäËûÈ«Ãğ000000*/

}client_custom_data_t;

//Ñ§Éú¼ä»úÆ÷ÈËÍ¨ĞÅ  cmd_id:0x0301  ÄÚÈİid£º0x0201~0x02FF   ·¢ËÍÆµÂÊ£ºÉÏÏŞ10Hz
typedef __packed struct
{
	uint8_t  data_frame[112];//Ğ¡ÓÚ113×Ö½Ú
}robot_interactive_data_t;

//Ö¡Í·  ÃüÁîÂë  Êı¾İ¶ÎÍ·½á¹¹  Êı¾İ¶Î   Ö¡Î²
//ÉÏ´«¿Í»§¶Ë
typedef __packed struct
{
	extFrameHeader                          txFrameHeader;//Ö¡Í·
	uint16_t		                            CmdID;//ÃüÁîÂë
	ext_student_interactive_header_data_t   dataFrameHeader;//Êı¾İ¶ÎÍ·½á¹¹
	client_custom_data_t  	                clientData;//Êı¾İ¶Î
	uint16_t		                            FrameTail;//Ö¡Î²
}exSendClientData_t;

//»úÆ÷ÈË½»»¥ĞÅÏ¢
typedef __packed struct
{
	extFrameHeader                          txFrameHeader;//Ö¡Í·
	uint16_t		                            CmdID;//ÃüÁîÂë
	ext_student_interactive_header_data_t   dataFrameHeader;//Êı¾İ¶ÎÍ·½á¹¹
	robot_interactive_data_t  	            interactData;//Êı¾İ¶Î
	uint16_t		                            FrameTail;//Ö¡Î²
}exCommunatianData_t;

/*--------------------------------- ×Ô¶¨Òå -------------------------------*/
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
#define VISION_ASSIST         (uint8_t)(7<<3|7)
#define SCAN_FAILED           (uint8_t)(5<<3|2)
#define SCAN_ASSIST           (uint8_t)(0<<3|7)
#define NO_ASSIST             (uint8_t)(0<<3|0)

//ÉÏ´«¿Í»§¶Ë×Ô¶¨ÒåÊı¾İ   cmd_id:0x0301   ÄÚÈİid:0xD180   ·¢ËÍÆµÂÊ:ÉÏÏŞ10Hz
typedef __packed struct
{
	float   execute_task;      /*ÓÃÓÚÏÔÊ¾»úÆ÷ÈË×´Ì¬  
	                             Ç°ÈıÎ»:111»úĞµÄ£Ê½ 222ÍÓÂİÒÇÄ£Ê½ 333²»¿É¿ØÄ£Ê½
	                             ºóÈıÎ»:000³£¹æÄ£Ê½ 111µÇµºÄ£Ê½ 222È¡µ¯Ä£Ê½ 333²¹µ¯Ä£Ê½ 444¾ÈÔ®Ä£Ê½*/
	float   get_bomb_mode;     /*ÓÃÓÚÏÔÊ¾¶àÏäÄ£Ê½ÇĞ»» 
	                             µ¥Ïä111111 ÈıÏä333333 ÎåÏä555555 ÆäËû000000*/
	float   golf_num;          /*ÓÃÓÚÏÔÊ¾µ¯²Öµ¯Á¿ 
	                             µºÉÏÍê³ÉÒ»´ÎÈ¡µ¯¶¯×÷+20 µºÏÂÍê³ÉÒ»´ÎÈ¡µ¯¶¯×÷+6*/
	bool    golf_num_add_once; /*¼Ó·¨Ã¿´ÎÖ»¼ÓÒ»´Î¿*/
	uint8_t status_bar;        /*¿ªÁËÊÓ¾õÈ«ÁÁ111111 É¨Ãè¼äÏ¶ÁÁ101010 ÆäËûÈ«Ãğ000000*/

}data_for_client_t;

extern data_for_client_t data_for_client;

/*--------------------------------- º¯ÊıÉùÃ÷ -------------------------------*/
void judgement_init(void);
void receive_data_from_judgement(unsigned char *JudgementData);
void send_to_teammate(void);
void send_to_client(void);
bool is_red_or_blue(void);
void confirm_ID(void);
void judgement_send_task(void);

/*À´×ÔCRCµÄº¯ÊıÉêÃ÷*/
void CRC_init(void);
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);

#endif

