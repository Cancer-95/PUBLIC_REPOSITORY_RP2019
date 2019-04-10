#ifndef __VISION_H_
#define __VISION_H_

#include "system.h"

#define    FALSE    0
#define    TRUE     1

/*--------------------------发送---------------------------*/

typedef __packed struct
{
	/* 头 */
	uint8_t   SOF;			//帧头起始位,暂定0xA5
	uint8_t   CmdID;		//指令 00不要给我发数据了 01岛下 02岛上
	uint8_t   CRC8;			//帧头CRC校验,保证发送的指令是正确的
	
}extuart4_sendheader_t;

typedef __packed struct
{
	/* 数据 */
	uint8_t data_0;//预留的
	
}extuart4_senddata_t;

//将打包好的数据一个字节一个字节地发送出去
typedef __packed struct
{	
	/* 尾 */
	uint16_t  CRC16;
	
}extuart4_sendtail_t;

/*--------------------------接收---------------------------*/

typedef __packed struct
{
	/* 头 */
	uint8_t   SOF;			//帧头起始位,暂定0xA5
	uint8_t   CmdID;		//指令 
	uint8_t   CRC8;			//帧头CRC校验,保证发送的指令是正确的
	
	/* 数据 */
	float     y_fdb;    //像素点坐标fdb
	uint8_t   captured; //捕捉到了 这个时候才允许执行自动程序
	/* 尾 */
	uint16_t  CRC16;       
	
}extuart4_recvdata_t;

/*----------------------------------------------------------*/
typedef __packed struct
{	
//	bool     aim;       //是否完成了视觉对准
	bool     enable;
	bool     delay_record; 
  uint32_t light_delay_time; 
	uint32_t quit_delay_time; 
	uint8_t  TX_Cmd;
	uint8_t  RX_Cmd;
	uint8_t  captured;	
	float    y_fdb;
	float    y_ref;
}vision_t;

extern vision_t  vision;            //视觉结构体
 

void vision_init(void);
//void vision_param_init(void);
void vision_send_task(void);
void vision_task(void);

#endif
