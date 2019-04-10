#include "judgement.h"
#include "string.h"

#define send_max_len     200
#define receive_max_len  200

unsigned char JudgementRxBuffer[receive_max_len];
unsigned char ClientTxBuffer[send_max_len];
unsigned char TeammateTxBuffer[send_max_len];

uint8_t JudgementDataLength = 0;

uint16_t clientHero, 
         clientEngineer, 
				 clientInfantry3,  
				 clientInfantry4,  
				 clientInfantry5, 
				 clientPlane;
uint16_t Hero, 
         Engineer, 
				 Infantry3, 
				 Infantry4, 
				 Infantry5, 
				 Plane, 
				 Sentry;

//接收
extFrameHeader   									FrameHeader;
ext_game_state_t 									GameStateData;
ext_game_result_t 								GameResultData;
ext_game_robot_survivors_t 				RobotSurvivorsData;
ext_event_data_t    							EventData;
ext_supply_projectile_action_t 		SupplyActionData;
ext_supply_projectile_booking_t   SupplyBookData;
ext_game_robot_state_t  					RobotStateData;
ext_power_heat_data_t   					PowerHeatData ;
ext_game_robot_pos_t   						RobotPositionData;
ext_buff_musk_t  									BuffMuskData ;
aerial_robot_energy_t   					PlaneEnergyData;
ext_robot_hurt_t  								HurtData ;
ext_shoot_data_t 									ShootData;

//发送
exSendClientData_t      SendClient;
exCommunatianData_t     CommuData;

//串口初始化
void judgement_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_DMA1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);
	/* -------------- Configure GPIO ---------------------------------------*/
	{
		GPIO_InitTypeDef  GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
		GPIO_Init(GPIOD,&GPIO_InitStructure);

		USART_InitTypeDef USART_InitStructure;
		USART_DeInit(UART5);
		USART_InitStructure.USART_BaudRate   = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits   = USART_StopBits_1;
		USART_InitStructure.USART_Parity     = USART_Parity_No;
		USART_InitStructure.USART_Mode 	   = USART_Mode_Tx|USART_Mode_Rx;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(UART5,&USART_InitStructure);
		USART_Cmd(UART5,ENABLE);
		USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);
		USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);
		USART_DMACmd(UART5,USART_DMAReq_Tx,ENABLE);
	}
	/* -------------- Configure DMA -----------------------------------------*/
	{
		DMA_InitTypeDef DMA_InitStructure;
		DMA_DeInit(DMA1_Stream0);
		DMA_InitStructure.DMA_Channel = DMA_Channel_4;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART5->DR);
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)JudgementRxBuffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = receive_max_len;//接受区的大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream0,&DMA_InitStructure);
		DMA_Cmd(DMA1_Stream0,ENABLE);
	}
	/* -------------- Configure NVIC ---------------------------------------*/
	{
		nvic_config(UART5_IRQn,UART5_IRQn_pre,UART5_IRQn_sub);// Configure NVIC 

//		NVIC_InitTypeDef  nvic;
//		nvic.NVIC_IRQChannel = UART5_IRQn;
//		nvic.NVIC_IRQChannelPreemptionPriority = 0;
//		nvic.NVIC_IRQChannelSubPriority = 0;
//		nvic.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&nvic);
	}
}

//裁判系统数据处理
void receive_data_from_judgement(unsigned char *JudgementData)
{
	int CmdID = 0;

	memcpy(&FrameHeader, JudgementData, sizeof(extFrameHeader));

	if((*JudgementData == 0xA5)  
	&& (Verify_CRC8_Check_Sum(JudgementData, sizeof(extFrameHeader)) == 1) 
	&& (Verify_CRC16_Check_Sum(JudgementData, sizeof(extFrameHeader) + CMD_ID_SIZE + FrameHeader.DataLength + FRAME_TAIL_SIZE) == 1))
	{
		CmdID = (JudgementData[6] << 8 | JudgementData[5]);

		switch (CmdID)
		{
			case Game_State: 								//比赛数据信息 
			{
				memcpy(&GameStateData, JudgementData + 7, sizeof(GameStateData));
			}break;
			
			case Game_Result:          		 	//比赛结果数据
			{
				memcpy(&GameResultData, JudgementData + 7, sizeof(GameResultData));	
			}break;
			
			case Game_Robot_Survivors:  		//机器人存活数据:0x0003
			{
				memcpy(&RobotSurvivorsData, JudgementData + 7, sizeof(RobotSurvivorsData));
			}break;
				
			case Event_Data: 								//场地时间数据
			{
				memcpy(&EventData,JudgementData + 7, sizeof(EventData));
			}break;
			
			case Supply_Projectile_Action:	//补给站动作标识
			{
				memcpy(&SupplyActionData,JudgementData + 7, sizeof(SupplyActionData));
			}break;
			
			case Supply_Orojectile_Booking://补给站预约子弹
			{
				memcpy(&SupplyBookData,JudgementData+7,sizeof(SupplyBookData));
			}break;
			
			case Game_Robot_State:				//比赛机器人状态
			{
				memcpy(&RobotStateData,JudgementData+7,sizeof(RobotStateData));
			}break;
      
			case Power_Heat_Data:  				//实时功率和热量数据
			{
				memcpy(&PowerHeatData, JudgementData + 7, sizeof(PowerHeatData));
			}break;

			case Game_Robot_Pos:        	//机器人位置
			{
				memcpy(&RobotPositionData, JudgementData + 7, sizeof(RobotPositionData));  
			}break;

			case Buff_Musk:								//机器人增益
			{
				memcpy(&BuffMuskData, JudgementData + 7, sizeof(BuffMuskData));
			}break;
			
			case Aerial_Robot_Energy:			//空中机器人能量状态
			{
				memcpy(&PlaneEnergyData, JudgementData + 7, sizeof(PlaneEnergyData));		  
			}break;
			
			
			case Robot_Hurt:               //伤害数据
			{
				memcpy(&HurtData, JudgementData + 7, sizeof(HurtData));			
			}break;

			case Shoot_Data:      				 //实时射击数据
			{
				memcpy(&ShootData, JudgementData + 7, sizeof(ShootData));				
			}break;
		}
		
		if(*(JudgementData + sizeof(extFrameHeader) + CMD_ID_SIZE + FrameHeader.DataLength + FRAME_TAIL_SIZE) == 0xA5)
		{
			receive_data_from_judgement(JudgementData + sizeof(extFrameHeader) + CMD_ID_SIZE + FrameHeader.DataLength + FRAME_TAIL_SIZE);
		}
	}
}

//串口五接受中断
void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5,USART_IT_IDLE)!=RESET)
	{		
		JudgementDataLength = UART5->SR ;
		JudgementDataLength = UART5->DR ;
		
		DMA_Cmd(DMA1_Stream0,DISABLE);
		
		JudgementDataLength = receive_max_len - DMA_GetCurrDataCounter(DMA1_Stream0);
		
		receive_data_from_judgement(JudgementRxBuffer);

		DMA_Cmd(DMA1_Stream0,ENABLE);
	}
}

//发送给客户端
data_for_client_t data_for_client;
void send_to_client(void)
{
	static u8 datalength,i;
	
	SendClient.txFrameHeader.SQF = 0xA5;
	SendClient.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) 
		                                  + sizeof(client_custom_data_t);
	SendClient.txFrameHeader.Seq = 0;
	memcpy(ClientTxBuffer, 
	       &SendClient.txFrameHeader, 
	       sizeof(extFrameHeader));
	Append_CRC8_Check_Sum(ClientTxBuffer, sizeof(extFrameHeader));	
	
  SendClient.CmdID = 0x0301;
	
	SendClient.dataFrameHeader.data_cmd_id = 0xD180;
	SendClient.dataFrameHeader.send_ID     = Engineer;
	SendClient.dataFrameHeader.receiver_ID = clientEngineer;
	
	if (system_mode == SAFETY_MODE)
	{	
		SendClient.clientData.data1 = data_for_client.execute_task;  //执行什么任务
		SendClient.clientData.data2 = data_for_client.get_bomb_mode; //取弹什么模式
		SendClient.clientData.data3 = data_for_client.golf_num;      //高尔夫球数量
		SendClient.clientData.masks = data_for_client.status_bar;    //视觉、扫描还是手动
	}
	else
	{
		SendClient.clientData.data1 = (float)888888;  
		SendClient.clientData.data2 = (float)888888;
		SendClient.clientData.data3 = (float)888888;     
		SendClient.clientData.masks = (uint8_t)(0<<3|0);
	}    

	memcpy(ClientTxBuffer + 5, 
	       (uint8_t*)&SendClient.CmdID, 
	       (sizeof(SendClient.CmdID) + 
					sizeof(SendClient.dataFrameHeader) + 
					sizeof(SendClient.clientData)));			
	Append_CRC16_Check_Sum(ClientTxBuffer,sizeof(SendClient));	

	datalength = sizeof(SendClient); 
	for(i = 0;i < datalength;i++)
	{
		USART_SendData(UART5,(uint16_t)ClientTxBuffer[i]);
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET);
	}	 
}

//发给队友
void send_to_teammate(void)
{
	static u8 datalength,i;
	
	CommuData.txFrameHeader.SQF = 0xA5;
	CommuData.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) 
		                                 + sizeof(robot_interactive_data_t);
	CommuData.txFrameHeader.Seq = 0;
	memcpy(TeammateTxBuffer, 
	       &CommuData.txFrameHeader, 
	       sizeof(extFrameHeader));
  Append_CRC8_Check_Sum(TeammateTxBuffer, sizeof(extFrameHeader));	
	
	CommuData.CmdID = 0x0301;
	
	CommuData.dataFrameHeader.data_cmd_id = Supply_Unavailable;//在0x0200-0x02ff之间选择   
	CommuData.dataFrameHeader.send_ID = Engineer;//发送者的ID
	CommuData.dataFrameHeader.receiver_ID = Hero;
	
	CommuData.interactData.data_frame[0] = 66;//大小不要超过变量的变量类型   
	
	memcpy(TeammateTxBuffer + 5,
	       (uint8_t *)&CommuData.CmdID,
	       (sizeof(CommuData.CmdID)+
					sizeof(CommuData.dataFrameHeader) +
					sizeof(CommuData.interactData)));		
	Append_CRC16_Check_Sum(TeammateTxBuffer,sizeof(CommuData));
	
	datalength = sizeof(CommuData); 
	for(i = 0;i < datalength;i++)
	{
		USART_SendData(UART5,(uint16_t)TeammateTxBuffer[i]);
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET);
	}	 
}

void judgement_send_task(void)
{
	confirm_ID();
	/*-------------------------任务提示-------------------------*/	
	{
		/*后三位处理*/
		if(climb_tube.climb_enable == ENABLE)
		{
			if(climb_tube.ctrl_mode == CLIMEB_UP_OPTIC_MODE ||
				 climb_tube.ctrl_mode == CLIMEB_DOWN_OPTIC_MODE)
			{
				data_for_client.execute_task = CLIMB_TUBE_OPTIC_TASK; //爬杆任务111110 光电方法
			}
			else if(climb_tube.ctrl_mode == CLIMEB_UP_GYRO_MODE ||
				      climb_tube.ctrl_mode == CLIMEB_DOWN_GYRO_MODE)
			{
				data_for_client.execute_task = CLIMB_TUBE_GYRO_TASK; //爬杆任务101010 陀螺仪方法
			}
			
		}
		else if(get_bomb.get_bomb_enable == ENABLE)
		{
			data_for_client.execute_task = GET_BOMB_TASK;   //取弹任务222220
		}
		else if(feed_bomb.feed_bomb_enable == ENABLE)
		{
			data_for_client.execute_task = FEED_BOMB_TASK;  //补弹任务333330
		}
		else if(rescue.rescue_enable == ENABLE)
		{
			data_for_client.execute_task = RESCUE_TASK;     //救援任务444440
		}
		else data_for_client.execute_task = NORMAL_TASK;  //其他任务000000
				
		/*后三位处理*/
		if(chassis.ctrl_mode == CHASSIS_FOLLOW)
		{
			data_for_client.execute_task += FOLLOW_CHASSIS;          //底盘陀螺仪+2
		}
		else if(chassis.ctrl_mode == CHASSIS_SEPARATE)
		{
			data_for_client.execute_task += SEPARATE_CHASSIS;        //底盘机械+1
		}
		else data_for_client.execute_task += UNCONTROLLED_CHASSIS; //底盘不可控+8
		/*如果标记了陀螺仪爆炸 陀螺仪模式变成机械模式*/
		if(flag_gyro_blow_up && 
			(data_for_client.execute_task >= FOLLOW_CHASSIS) && 
		  (data_for_client.execute_task < UNCONTROLLED_CHASSIS))
		{
			data_for_client.execute_task -= 1;
		}
  }	
	
  /*-------------------------取多少箱&位置提示-------------------------*/
	{
    /*前三位处理*/
		if(flag_bomb_claw_triple)
		{
			data_for_client.get_bomb_mode = GET_TRIPLE_CASE;   //箱数显示333330
		}
		else if(flag_bomb_claw_penta)
		{
			data_for_client.get_bomb_mode = GET_PENTA_CASE;    //箱数显示555550
		}
		else if(get_bomb.get_bomb_enable)
		{
			data_for_client.get_bomb_mode = GET_SINGLE_CASE;   //箱数显示111110
		}		
    else 	data_for_client.get_bomb_mode = GET_NO_CASE;   //箱数显示000000	
		
		/*后三位处理*/
		if(climb_tube.location == ON_GROUND)
		{
			data_for_client.get_bomb_mode += LOCATED_ON_GROUND;
		}
		else if(climb_tube.location == ON_ISLAND)
		{
			data_for_client.get_bomb_mode += LOCATED_ON_ISLAND;
		}
		else data_for_client.get_bomb_mode = ROBOT_IN_DANGER;

		/*-------------------------高尔夫数量大概提示-------------------------*/
		if(get_bomb.get_pour_step == 3 && data_for_client.golf_num_add_once)//取弹的最后一步 
		{
			data_for_client.golf_num_add_once = DISABLE;
			if(climb_tube.location == ON_ISLAND) //岛上一箱+20 岛下一箱+6
			{
				data_for_client.golf_num += 20;
			}
			else data_for_client.golf_num += 6;
		}
		else if(get_bomb.get_pour_step == 0) //取弹步骤被置零 
		{
			data_for_client.golf_num_add_once = ENABLE;
		}
		else if(flag_magazine_exit)            //开弹仓数量清零
		{
			data_for_client.golf_num = 0;
		}
	}
	
	/*-------------------------状态条提示-------------------------*/
	{
		if(vision.enable)
		{
			data_for_client.status_bar = VISION_ASSIST;//状态条显示视觉辅助
		}
		else if(flag_bomb_claw_scan)
		{
			if(get_bomb.flag_scan_failed)
			{
				data_for_client.status_bar = SCAN_FAILED;
			}
			else data_for_client.status_bar = SCAN_ASSIST;  //状态条显示扫描辅助
		}
		else data_for_client.status_bar = NO_ASSIST; //状态条显示无辅助
	}
	
	send_to_client();//把上面的消息发给客户端操作手界面
	
	send_to_teammate();
}


//判断自己的性别
bool Color;
bool is_red_or_blue(void)
{
	if(RobotStateData.robot_id > 10)
	{
		return BLUE;
	}
	else return RED;
}

//选择机器人ID跟客户端ID
void confirm_ID(void)
{
	Color = is_red_or_blue();
	if(Color == BLUE)
	{
		clientHero      = 0x0111;
		clientEngineer  = 0x0112;
		clientInfantry3 = 0x0113;
		clientInfantry4 = 0x0114;
		clientInfantry5 = 0x0115;
		clientPlane     = 0x0116;

		Hero      = 11;
		Engineer  = 12;
		Infantry3 = 13;
		Infantry4 = 14;
		Infantry5 = 15;
		Plane     = 16;
		Sentry    = 17;
	}
	else if(Color == RED)
	{
		clientHero      = 0x0101;
		clientEngineer  = 0x0102;
		clientInfantry3 = 0x0103;
		clientInfantry4 = 0x0104;
		clientInfantry5 = 0x0105;
		clientPlane     = 0x0106;

		Hero      = 1;
		Engineer  = 2;
		Infantry3 = 3;
		Infantry4 = 4;
		Infantry5 = 5;
		Plane     = 6;
		Sentry    = 7;
	}
}

/*----------------------------------------------------------------------------------*/
/*                                 		CRC校验							                    			*/
/*----------------------------------------------------------------------------------*/

/*************裁判系统*********/
//crc8 generator polynomial:G(x)=x8+x5+x4+1
const unsigned char CRC8_INIT = 0xff;
const unsigned char CRC8_TAB[256] =
{
0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

uint16_t CRC_INIT = 0xffff;
const uint16_t wCRC_Table[256] =
{
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8)
{
	unsigned char ucIndex;
	while (dwLength--)
	{
		ucIndex = ucCRC8^(*pchMessage++);
		ucCRC8 = CRC8_TAB[ucIndex];
	}
	return(ucCRC8);
}
/*
** Descriptions: CRC8 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
	unsigned char ucExpected = 0;
	if ((pchMessage == 0) || (dwLength <= 2)) return 0;
	ucExpected = Get_CRC8_Check_Sum (pchMessage, dwLength-1, CRC8_INIT);
	return ( ucExpected == pchMessage[dwLength-1] );
}
/*
** Descriptions: append CRC8 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
	unsigned char ucCRC = 0;
	if ((pchMessage == 0) || (dwLength <= 2)) return;
	ucCRC = Get_CRC8_Check_Sum ( (unsigned char *)pchMessage, dwLength-1, CRC8_INIT);
	pchMessage[dwLength-1] = ucCRC;
}

/*
** Descriptions: CRC16 checksum function
** Input: Data to check,Stream length, initialized checksum
** Output: CRC checksum
*/
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
	uint8_t chData;
	if (pchMessage == NULL)
	{
		return 0xFFFF;
	}
	while(dwLength--)
	{
		chData = *pchMessage++;
		(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
	}
	return wCRC;
}
/*
** Descriptions: CRC16 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
	uint16_t wExpected = 0;
	if ((pchMessage == NULL) || (dwLength <= 2))
	{
		return 0;
	}
	wExpected = Get_CRC16_Check_Sum ( pchMessage, dwLength - 2, CRC_INIT);
	return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]);
}
/*
** Descriptions: append CRC16 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength)
{
	uint16_t wCRC = 0;
	if ((pchMessage == NULL) || (dwLength <= 2))
	{
		return;
	}
	wCRC = Get_CRC16_Check_Sum ( (u8 *)pchMessage, dwLength-2, CRC_INIT );
	pchMessage[dwLength-2] = (u8)(wCRC & 0x00ff);
	pchMessage[dwLength-1] = (u8)((wCRC >> 8)& 0x00ff);
}
//usart4通信配置  波特率115200 数据位8 停止位1 校验位无 流控制无

void CRC_init(void)
{
	/* Enable CRC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
}



