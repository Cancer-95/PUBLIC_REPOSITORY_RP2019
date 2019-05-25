#include "Judgement.h"
#include <string.h>

//PA1		UART4_RX		DMA1	数据流2通道4
//PA0		UART4_TX		DMA1	数据流4通道4

//裁判系统数据
extFrameHeader			 FrameHeader;            //接收信息帧头
extGameRobotState_t  RobotStateData;         //比赛进程信息
extRobotHurt_t 			 HurtData;               //伤害数据
extShootData_t 			 ShootData;              //实时射击数据
extPowerHeatData_t 	 PowerHeatData;          //实时功率热量数据
extRfidDetect_t 		 RfidData;               //场地交互数据
extGameResult_t 		 GameResultData;         //比赛胜负数据
extGetBuff_t 			   BuffData;               //BUFF获取数据 
extGameRobotPos_t    GameRobotPosition;      //机器人位置信息
extShowData_t 			 TeamCustomData;         //自定义数据

//发送
extFrameHeader 	 		 TxFrameHeader;				   //发送信息帧头
tTxGameInfo			 	   TxGameInfo;	

unsigned char JudgementRxBuffer[200]={0};
unsigned char JudgementTxBuffer[100]={0};
int JudgementDataLength = 0;


void Judgement_Init(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1,ENABLE);
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
		USART_InitStructure.USART_BaudRate            = 115200;
		USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits            = USART_StopBits_1;
		USART_InitStructure.USART_Parity              = USART_Parity_No;
		USART_InitStructure.USART_Mode 	              = USART_Mode_Tx | USART_Mode_Rx;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(UART5,&USART_InitStructure);

		USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);
		nvic_config(UART5_IRQn,UART5_IRQn_pre,UART5_IRQn_sub);// Configure NVIC 
		
		USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);
		USART_DMACmd(UART5,USART_DMAReq_Tx,ENABLE);
		USART_Cmd(UART5,ENABLE);
	}
	/* -------------- Configure DMA -----------------------------------------*/
	{
		DMA_InitTypeDef DMA_InitStructure;
		DMA_DeInit(DMA1_Stream0);
		DMA_DeInit(DMA1_Stream7);
		
		DMA_InitStructure.DMA_Channel = DMA_Channel_4;		
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART5->DR);
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)JudgementRxBuffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = 100;
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
			
		DMA_InitStructure.DMA_Channel = DMA_Channel_4;  													//通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(UART5->DR);					  	//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)JudgementTxBuffer;						//DMA存储器地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;										//数据传输方式
    DMA_InitStructure.DMA_BufferSize = 100;												          	//数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;					//外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;										//存储器增量模式
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//外设数据长度：8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;						//存储器数据长度：8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;															//使用普通模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;										//最高优先级
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                    //
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;             //
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;               //存储器突发单次传输
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;				//外设突发单次传输
		DMA_Init(DMA1_Stream7,&DMA_InitStructure);
				
		nvic_config(DMA1_Stream7_IRQn,DMA1_Stream7_IRQn_pre,DMA1_Stream7_IRQn_sub);//发送开DMA中断？？
		DMA_ITConfig(DMA1_Stream7,DMA_IT_TC,ENABLE);

		DMA_Cmd(DMA1_Stream0,ENABLE);
		DMA_Cmd(DMA1_Stream7,ENABLE);	
	}
}	

/*------------------------------------------------------------------------------------------------*/
/*										读取裁判系统数据 										  */
/*------------------------------------------------------------------------------------------------*/

PowerProtect_t PowerProtect;
Shooter_t Shooter;

int DataPackageLossCheck = 0;

void Receive_Data_From_Judgement(unsigned char *JudgementData)
{
	int CmdID = 0;
	
	DataPackageLossCheck++;
	memcpy(&FrameHeader, JudgementData, sizeof(extFrameHeader));

	if((*JudgementData == 0xA5)
	&& (Verify_CRC8_Check_Sum(JudgementData, sizeof(extFrameHeader)) == 1)
	&& (Verify_CRC16_Check_Sum(JudgementData, sizeof(extFrameHeader) + CMD_ID_SIZE + FrameHeader.DataLength + FRAME_TAIL_SIZE) == 1))
	{
		CmdID = (JudgementData[6] << 8 | JudgementData[5]);

		DataPackageLossCheck--;
		
		switch (CmdID)
		{
			case Robot_State_Data:            //机器人状态
			{
				memcpy(&RobotStateData, JudgementData + 7, sizeof(extGameRobotState_t));
			}break;

			case Hurt_Data:                   //伤害数据
			{
				memcpy(&HurtData, JudgementData + 7, sizeof(extRobotHurt_t));
			}break;

			case Real_Time_Shoot_Data:       //实时射击数据
			{
				memcpy(&ShootData, JudgementData + 7, sizeof(extShootData_t));
				if (ShootData.bulletType == 1)
				{
			    Shooter.Shooted_Num ++;
					Shooter.Shoot_Time2 = millis();//记录裁判系统的时间
					Shooter.Shoot_Delay = Shooter.Shoot_Time2 - Shooter.Shoot_Time1;//发射延时计算

					ShootData.bulletType = 0;
				}
			}break;

			case Real_Time_Power_Heat_Data:  //实时功率和热量数据
			{
				memcpy(&PowerHeatData, JudgementData + 7, sizeof(extPowerHeatData_t));
				/*关于枪口热量的数据的获取*/
				Shooter.Current_heat17 = PowerHeatData.shooterHeat0;
				/*关于底盘功率的数据的获取*/
			  PowerProtect.Judge_power = PowerHeatData.chassisPower;//实时功率
				PowerProtect.Judge_power_remain = PowerHeatData.chassisPowerBuffer;//功率缓冲
				PowerProtect.Judge_chassisVolt = PowerHeatData.chassisVolt;//输出电压
				PowerProtect.Judge_chassisCurrent =  PowerHeatData.chassisCurrent;//输出电流
				if (PowerProtect.Judge_chassisVolt*PowerProtect.Judge_chassisCurrent != 0)
				{
					PowerProtect.Judge_connect = 1;
//					PowerProtect.Protect_num = 0;//
				}
			}break;

			case Real_Time_Rfid_Detect_Data: //实时场地交互数据
			{
				memcpy(&RfidData, JudgementData + 7, sizeof(extRfidDetect_t));	
			}break;
			
			case Game_Result_Data:           //比赛结果数据
			{
				memcpy(&GameResultData, JudgementData + 7, sizeof(extGameResult_t));	
			}break;

			case Get_Buff_Data:              //Buff数据
			{
				memcpy(&BuffData, JudgementData + 7, sizeof(extGetBuff_t));			
			}break;
			
			case Game_Robot_Pos_Data:        //机器人位置与枪口朝向数据
			{
				memcpy(&GameRobotPosition, JudgementData + 7, sizeof(extGameRobotPos_t));
			}break;
		}
		
		if(*(JudgementData + sizeof(extFrameHeader) + CMD_ID_SIZE + FrameHeader.DataLength + FRAME_TAIL_SIZE) == 0xA5)
		{
			Receive_Data_From_Judgement(JudgementData + sizeof(extFrameHeader) + CMD_ID_SIZE + FrameHeader.DataLength + FRAME_TAIL_SIZE);
		}
	}
}
/*
		C语言学习:
	照理来说下面这句话应该传一个地址进去
	数组名不代表整个数组 而代表数组首元素的地址 其实也可以传 &JudgementRxBuffer[0] 进去	
*/

void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5,USART_IT_IDLE)!=RESET)
	{		
		DMA_Cmd(DMA1_Stream0,DISABLE);	
		
		JudgementDataLength = UART5->SR ;
		JudgementDataLength = UART5->DR ;
		

		
		JudgementDataLength = 200 - DMA_GetCurrDataCounter(DMA1_Stream0);
		
//		DMA1_Stream0 -> NDTR = 200;

		Receive_Data_From_Judgement(JudgementRxBuffer);
		
		DMA_Cmd(DMA1_Stream0,ENABLE);
	}
}
/*------------------------------------------------------------------------------------------------*/
/*										 发送自定义数据 										  */
/*------------------------------------------------------------------------------------------------*/

void Send_Data_To_Judgement(void)
{
//	static u8 i = 0;
		
	static u8 Data_Len = 0;
	
	TxFrameHeader.SQF = 0xA5;											         //数据帧起始
	TxFrameHeader.DataLength = sizeof(TeamCustomData);	   //数据帧内DATA长度
	TxFrameHeader.Seq = 0;											          	//包序号
	memcpy(JudgementTxBuffer, &TxFrameHeader, sizeof(extFrameHeader));
	Append_CRC8_Check_Sum(JudgementTxBuffer, sizeof(extFrameHeader));	//帧头CRC8校验
	
	TxGameInfo.CmdID = 0x0100;														//命令ID
	if (system_mode != SAFETY_MODE)//非安全模式
	{
		TxGameInfo.ShowData.data1 = (float)Shooter.Shooted_Num;						                 	//数据1——已发射数量
		TxGameInfo.ShowData.data2 = (float)Shooter.Current_heat17;						//数据2
		TxGameInfo.ShowData.data3 = (float)PowerHeatData.chassisPowerBuffer;	//数据3
		TxGameInfo.ShowData.mask = (uint16_t)(3<<2 | 7);	                    //数据4
	}
	else //遥控器丢控
	{
		TxGameInfo.ShowData.data1 = (float)88888;					
		TxGameInfo.ShowData.data2 = (float)88888;					
		TxGameInfo.ShowData.data3 = (float)88888;						
		TxGameInfo.ShowData.mask = (uint16_t)00000;	                          //数据4
	}			
	memcpy(JudgementTxBuffer + 5, &TxGameInfo, sizeof(TxGameInfo));			
	Append_CRC16_Check_Sum(JudgementTxBuffer, sizeof(TxFrameHeader) + sizeof(TxGameInfo));	
	
	Data_Len = sizeof(extFrameHeader)+sizeof(TxGameInfo); 

//	for(i = 0 ; i < Data_Len ; i++)
//	{
//		USART_SendData(UART5,(uint16_t)JudgementTxBuffer[i]);
//		
//		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET);
//	}	
	
	while (DMA_GetCmdStatus(DMA1_Stream7) != DISABLE){}		                  //确保DMA可以被配置
	DMA_SetCurrDataCounter(DMA1_Stream7,Data_Len);          		            //数据传输量
	DMA_Cmd(DMA1_Stream7, ENABLE);                                	        //开启DMA传输
		
}


/*用于发送的DMA*/
void DMA1_Stream7_IRQHandler(void)										              	
{
	if(DMA_GetITStatus(DMA1_Stream7, DMA_IT_TCIF7))
	{
		DMA_ClearFlag(DMA1_Stream7, DMA_IT_TCIF7);
		DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
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

unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned
char ucCRC8)
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
		(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) &
		0x00ff];
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



