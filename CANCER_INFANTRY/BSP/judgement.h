#ifndef __JUDGEMENT_H
#define __JUDGEMENT_H

#include "system.h"

#define CMD_ID_SIZE 	 		      2
#define FRAME_TAIL_SIZE 	      2

enum Judge_CmdID
{ 
	Robot_State_Data 		      	 = 0x0001,        //比赛机器人状态， 10Hz 频率周期发送
	Hurt_Data					           = 0x0002,        //伤害数据，收到伤害时发送
	Real_Time_Shoot_Data 		     = 0x0003,        //实时射击数据，发射弹丸时发送
	Real_Time_Power_Heat_Data	   = 0x0004,        //实时功率和热量数据，50Hz 频率周期发送
	Real_Time_Rfid_Detect_Data   = 0x0005,    	  //实时场地交互数据，检测到 RFID 卡时 10Hz 周期发送
	Game_Result_Data 		      	 = 0x0006,        //比赛结果数据，比赛结束时发送
	Get_Buff_Data 				       = 0x0007,        //获取到 buff，激活机关后发送一次
	Game_Robot_Pos_Data          = 0x0008,        //机器人位置朝向信息
	Team_Custom_Data 			       = 0x0100,   	    //参赛队自定义数据，用于显示在操作界面
};

//帧头
typedef __packed struct
{
	uint8_t 	SQF; 			 //数据帧起始字节，固定值为0xA5
	uint16_t 	DataLength;		 //数据帧内Data长度
	uint8_t 	Seq;			 //包序号
	uint8_t 	CRC8;			 //帧头CRC校验
}extFrameHeader;

//比赛机器人状态 (0x0001)
typedef __packed struct
{
	uint16_t  stageRemianTime;   //当前阶段剩余时间，单位 s
	uint8_t   gameProgress;      //当前比赛阶段:0：未开始比赛  1：准备阶段  2：自检阶段  3：5s 倒计时  4：对战中  5：比赛结算中
	uint8_t   robotLevel;        //机器人当前等级
	uint16_t  remainHP;          //机器人当前血量
	uint16_t  maxHP;             //机器人满血量
}extGameRobotState_t;	

//伤害数据 (0x0002)
typedef __packed struct
{
	uint8_t armorType : 4;
	uint8_t hurtType :  4;
}extRobotHurt_t;

//实时射击信息 (0x0003)
typedef __packed struct
{
	uint8_t bulletType;          //弹丸类型  1：17mm 弹丸   2：42mm 弹丸
	uint8_t bulletFreq;          //弹丸射频
	float bulletSpeed;           //弹丸射速
}extShootData_t;

//实时功率热量数据 (0x0004)
typedef __packed struct
{
	float chassisVolt;           //底盘输出电压
	float chassisCurrent;        //底盘输出电流
	float chassisPower;          //底盘输出功率
	float chassisPowerBuffer;    //底盘功率缓冲
	uint16_t shooterHeat0;       //17mm 枪口热量
	uint16_t shooterHeat1;       //42mm 枪口热量
}extPowerHeatData_t;

//场地交互数据（0x0005）
typedef __packed struct
{
	uint8_t cardType;            //卡类型0：攻击加成卡 1：防御加成卡 2：红方加血卡 3：蓝方加血卡 4：红方大能量机关卡 5：蓝方大能量机关卡
	uint8_t cardIdx;             //卡索引号，可用于区分不同区域
}extRfidDetect_t;

//比赛胜负数据（0x0006）
typedef __packed struct
{
	uint8_t winner;              //比赛结果 0：平局 1：红方胜 2：蓝方胜
}extGameResult_t;

//Buff 获取数据（0x0007）
typedef __packed struct
{
	uint8_t buffType;            //Buff 类型 0：攻击加成 1：防御加成 2：获得大能量机关
	uint8_t buffAddition;        //加成百分比（比如 10 代表加成 10%）
} extGetBuff_t;

//机器人位置朝向信息
typedef __packed struct
{
  float x;
	float y;
	float z;
	float yaw;
} extGameRobotPos_t;

//参赛队自定义数据（0x0100）
typedef __packed struct
{
	float data1;
	float data2;
	float data3;
	uint8_t mask;
}extShowData_t;

typedef __packed struct
{
	uint16_t		     CmdID;
	extShowData_t 	 ShowData;
	uint16_t		     FrameTail;
}tTxGameInfo;

extern extGameRobotState_t   RobotStateData;
extern extRobotHurt_t 		   HurtData;
extern extShootData_t        ShootData;
extern extPowerHeatData_t    PowerHeatData;
extern extRfidDetect_t 		   RfidData;
extern extGameResult_t 	  	 GameResultData;
extern extGetBuff_t 		     BuffData;
extern extShowData_t 		     TeamCustomData;


void Judgement_Init(void);
void Receive_Data_From_Judgement(unsigned char *JudgementData);
void Send_Data_To_Judgement(void);




/*来自CRC的函数申明*/
void CRC_init(void);
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);


/*------------- 自定义 -------------*/

//extern float current_heat42;
//extern int Bomb_Shoot_Num;
//extern bool Shoot_Data_Update;

//功率数据
typedef struct
{
	float  Judge_chassisVolt;     //底盘输出电压
  float  Judge_chassisCurrent;  //底盘输出电流
	bool   Judge_connect;         //掉线标志位
	float  Judge_power_remain;    //底盘剩余能量
	float  Judge_power;         //底盘实时功率
	int    Protect_num;           //
}PowerProtect_t;


//枪口数据
typedef struct
{
	float	   Current_heat17;   //枪管热量
	uint32_t Shoot_Time1;   //时间1
	uint32_t Shoot_Time2;   //时间2
	uint16_t Shooted_Num;   //发射子弹的数量
	uint32_t Shoot_Delay;   //发射延迟
}Shooter_t;

extern Shooter_t Shooter;
extern PowerProtect_t PowerProtect;

#endif




