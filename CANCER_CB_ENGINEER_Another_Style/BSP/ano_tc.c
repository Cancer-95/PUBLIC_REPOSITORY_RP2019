/*****************************************************************************************************************
 * @file 		  ano_tc.c
 * @version 	0.0
 * @date 		  2018.10.23
 *
 * @brief     这是一个Cancer抄袭一休哥的无线调参程序 根据匿名上位机写的
 *            其中使用了串口六的发送端(空闲中断)
 *						TX		PC6			DMA2_Stream1_ch5
 *  					RX		PC7			DMA2_Stream6_ch5 
 *            使用DMA进行串口数据的发送与接收，数据可以是不定长的
 *            最大接受与发送长度由U6_TXMAX_len决定
 *            这当中有一个问题： 接收不需要开DMA中断 发送需要开DMA中断（为什么？？）
 *****************************************************************************************************************/
#include "ano_tc.h"

u8 USART6_RX_buf[U6_RXMAX_len]={0};
u8 USART6_TX_buf[U6_TXMAX_len]={0};
u8 USART6_RX_updated = 0;
u8 USART6_RX_buf_LENTH = 0;

void ANO_TC_Init(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	// GPIOC时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE); //串口6时钟使能

//	USART_DeInit(USART3);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); 
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART6,&USART_InitStructure);		
	
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//开启空闲中断
	
//	NVIC_InitStructure.NVIC_IRQChannel=USART6_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=USART6_IRQn_pre;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=USART6_IRQn_sub;
//	NVIC_Init(&NVIC_InitStructure);
  NVIC_config(USART6_IRQn,USART6_IRQn_pre,USART6_IRQn_sub);

	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(USART6,ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能

	DMA_DeInit(DMA2_Stream1);
	DMA_DeInit(DMA2_Stream6);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;  													//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART6->DR);						//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)USART6_RX_buf;								//DMA存储器地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;										//数据传输方式
  DMA_InitStructure.DMA_BufferSize = U6_RXMAX_len;													//数据传输量
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
	DMA_Init(DMA2_Stream1,&DMA_InitStructure);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;  													//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART6->DR);						//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)USART6_TX_buf;								//DMA存储器地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;										//数据传输方式
  DMA_InitStructure.DMA_BufferSize = U6_TXMAX_len;													//数据传输量
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
	DMA_Init(DMA2_Stream6,&DMA_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream6_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=DMA2_Stream6_IRQn_pre;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=DMA2_Stream6_IRQn_sub;
//	NVIC_Init(&NVIC_InitStructure);
	NVIC_config(DMA2_Stream6_IRQn,DMA2_Stream6_IRQn_pre,DMA2_Stream6_IRQn_sub);//发送开DMA中断？？

//	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA2_Stream6,DMA_IT_TC,ENABLE);

	DMA_Cmd(DMA2_Stream1,ENABLE);
	DMA_Cmd(DMA2_Stream6,ENABLE);
}

//传送数据给匿名地面站
void ANO_TC_Report(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 i;
	DMA_Cmd(DMA2_Stream6, DISABLE);                              //关闭DMA传输
//	memset(USART6_TX_buf,0,23);
	USART6_TX_buf[0]=0XAA;															 				 //帧头
	USART6_TX_buf[1]=0XAA;															 				 //帧头
	USART6_TX_buf[2]=0XF1;															 				 //功能字
	USART6_TX_buf[3]=18;																 				 //数据长度
	
	USART6_TX_buf[4]=(aacx>>8)&0XFF;
	USART6_TX_buf[5]=aacx&0XFF;
	USART6_TX_buf[6]=(aacy>>8)&0XFF;
	USART6_TX_buf[7]=aacy&0XFF;
	USART6_TX_buf[8]=(aacz>>8)&0XFF;
	USART6_TX_buf[9]=aacz&0XFF; 
	USART6_TX_buf[10]=(gyrox>>8)&0XFF;
	USART6_TX_buf[11]=gyrox&0XFF;
	USART6_TX_buf[12]=(gyroy>>8)&0XFF;
	USART6_TX_buf[13]=gyroy&0XFF;
	USART6_TX_buf[14]=(gyroz>>8)&0XFF;
	USART6_TX_buf[15]=gyroz&0XFF;	
	USART6_TX_buf[16]=(roll>>8)&0XFF;
	USART6_TX_buf[17]=roll&0XFF;
	USART6_TX_buf[18]=(pitch>>8)&0XFF;
	USART6_TX_buf[19]=pitch&0XFF;
	USART6_TX_buf[20]=(yaw>>8)&0XFF;
	USART6_TX_buf[21]=yaw&0XFF;
	USART6_TX_buf[22]=0;																					//校验数置零
	for(i=0;i<22;i++)USART6_TX_buf[22]+=USART6_TX_buf[i];					//计算校验和
	
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}		        //确保DMA可以被配置
	DMA_SetCurrDataCounter(DMA2_Stream6,23);          		        //数据传输量
	DMA_Cmd(DMA2_Stream6, ENABLE);                      	        //开启DMA传输
}

void ANO_TC_Report16(short test)
{
	u8 i;
	DMA_Cmd(DMA2_Stream6, DISABLE);                              //关闭DMA传输
//	memset(USART6_TX_buf,0,7);
	USART6_TX_buf[0]=0XAA;															 				 //帧头
	USART6_TX_buf[1]=0XAA;															 				 //帧头
	USART6_TX_buf[2]=0XF1;															 				 //功能字
	USART6_TX_buf[3]=2;																 				   //数据长度
	
	USART6_TX_buf[4]=(test>>8)&0XFF;
	USART6_TX_buf[5]=test&0XFF;
	USART6_TX_buf[6]=0;																			    	//校验数置零
	for(i=0;i<6;i++)USART6_TX_buf[6]+=USART6_TX_buf[i];				  	//计算校验和
	
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}		        //确保DMA可以被配置
	DMA_SetCurrDataCounter(DMA2_Stream6,7);          		          //数据传输量
	DMA_Cmd(DMA2_Stream6, ENABLE);                      	        //开启DMA传输
}

void ANO_TC_Report32(int32_t test)
{
	u8 i;
	DMA_Cmd(DMA2_Stream6, DISABLE);                                //关闭DMA传输
//	memset(USART6_TX_buf,0,7);
	USART6_TX_buf[0]=0XAA;															 			  	 //帧头
	USART6_TX_buf[1]=0XAA;															 			  	 //帧头
	USART6_TX_buf[2]=0XF1;															 			  	 //功能字
	USART6_TX_buf[3]=4;																 				     //数据长度
	
	USART6_TX_buf[4]=(test>>24)&0XFF;
  USART6_TX_buf[5]=(test>>16)&0XFF;
	USART6_TX_buf[6]=(test>>8)&0XFF;
	USART6_TX_buf[7]=test&0XFF;
	USART6_TX_buf[8]=0;																				      //校验数置零
	for(i=0;i<8;i++)USART6_TX_buf[8]+=USART6_TX_buf[i];				    	//计算校验和
	
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}		          //确保DMA可以被配置
	DMA_SetCurrDataCounter(DMA2_Stream6,9);          		            //数据传输量
	DMA_Cmd(DMA2_Stream6, ENABLE);                                	//开启DMA传输
}

void USART6_IRQHandler(void)
{
	if(USART_GetITStatus(USART6,USART_IT_IDLE) != RESET) 	            //空闲中断判断数据是否通过串口传输完成
	{
		DMA_Cmd(DMA2_Stream1, DISABLE); 										            //关闭DMA，防止处理期间有数据
		USART6_RX_updated = 1;															         		//置1表示一次接收数据完成
		USART6->SR;  
		USART6->DR;  																						        //读SR再读DR 清除IDLE
		USART6_RX_buf_LENTH =U6_RXMAX_len - DMA2_Stream1->NDTR;				  //获得串口发送过来的字符的长度//DMA剩余
		
		memset(USART6_RX_buf+USART6_RX_buf_LENTH,0,DMA2_Stream1->NDTR);	//后面的全清零//重新覆盖接收数组//数组首地址  这一句话也可以直接Init
		
		DMA_ClearFlag(DMA2_Stream1, DMA_IT_TCIF1);
		DMA2_Stream1->NDTR = U6_RXMAX_len;		  								        //重新写入传输量
		DMA_Cmd(DMA2_Stream1, ENABLE);     									            //重新打开 
	}  
}

//void DMA2_Stream1_IRQHandler(void)											        //接受的中断DMA
//{
//	if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
//	{
//		DMA_ClearFlag(DMA2_Stream1, DMA_IT_TCIF1);
//		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
//	}
//}

void DMA2_Stream6_IRQHandler(void)										              	//用于发送的DMA
{
	if(DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6))
	{
		DMA_ClearFlag(DMA2_Stream6, DMA_IT_TCIF6);
		DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
	}
}
