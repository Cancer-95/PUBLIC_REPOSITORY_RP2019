/*****************************************************************************************************************
 * @file 		  roboremo.c
 * @version 	0.0
 * @date 		  2018.10.23
 *
 * @brief     这是一个Cancer抄袭一休哥的无线调参程序 根据手机App:Roboremo写的
 *            其中使用了串口三(空闲中断)
 *						TX		PB10			DMA1_Stream1_ch4
 *  					RX		PB11			DMA1_Stream3_ch4
 *            使用DMA进行串口数据的发送与接收，数据可以是不定长的
 *            最大接受与发送长度由U3_TXMAX_len决定
 *            这当中有一个问题： 接收不需要开DMA中断 发送需要开DMA中断（为什么？？）
 *****************************************************************************************************************/

#include "roboremo.h"

/*Roboremo调参用结构体全局可用*/
Roboremo_PID_t roboremo_pid     = {0};
Roboremo_Power_t roboremo_power = {0};

u8 USART3_RX_buf[U3_RXMAX_len]={0};
u8 USART3_TX_buf[U3_TXMAX_len]={0};
u8 USART3_RX_updated = 0;
u8 USART3_RX_buf_LENTH = 0;

void Roboremo_Init(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

//	USART_DeInit(USART3);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStructure);
	
	USART_Cmd(USART3,ENABLE);
	
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);//开启空闲中断
	
//	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=USART3_IRQn_pre;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=USART3_IRQn_sub;
//	NVIC_Init(&NVIC_InitStructure);
	NVIC_config(USART3_IRQn,USART3_IRQn_pre,USART3_IRQn_sub);

	
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能

	DMA_DeInit(DMA1_Stream1);
	DMA_DeInit(DMA1_Stream3);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  													//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART3->DR);						//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)USART3_RX_buf;								//DMA存储器地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;										//数据传输方式
  DMA_InitStructure.DMA_BufferSize = U3_RXMAX_len;													//数据传输量
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
	DMA_Init(DMA1_Stream1,&DMA_InitStructure);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  													//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART3->DR);						//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)USART3_TX_buf;								//DMA存储器地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;										//数据传输方式
  DMA_InitStructure.DMA_BufferSize = U3_TXMAX_len;													//数据传输量
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
	DMA_Init(DMA1_Stream3,&DMA_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=DMA1_Stream3_IRQn_pre;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=DMA1_Stream3_IRQn_sub;
//	NVIC_Init(&NVIC_InitStructure);
	NVIC_config(DMA1_Stream3_IRQn,DMA1_Stream3_IRQn_pre,DMA1_Stream3_IRQn_sub);
	
//	DMA_ITConfig(DMA1_Stream1,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);
	
	DMA_Cmd(DMA1_Stream1,ENABLE);
	DMA_Cmd(DMA1_Stream3,ENABLE);
}

u8 bufStartsWith(char *st)														//判断数组以何字符开始
{
	char i;
  for(i = 0; ; i++)
  {
    if (st[i] == 0) return 1;
    if (USART3_RX_buf[i] == 0) return 0;
    if (USART3_RX_buf[i] != st[i]) return 0;;
  }
}

char mode = 0;
void exeCmd(void)																				
{
  if ( bufStartsWith("IN_p") )       mode = 1;
  if ( bufStartsWith("IN_i") )    	 mode = 2;
  if ( bufStartsWith("IN_d") )    	 mode = 3;
	if ( bufStartsWith("OUT_p") )    	 mode = 4;
	if ( bufStartsWith("OUT_i") )    	 mode = 5;
	if ( bufStartsWith("OUT_d") )    	 mode = 6;
	
	if ( bufStartsWith("TT_L") )    	 mode = 7;
	if ( bufStartsWith("PW_T") )    	 mode = 8;
	else {};
}

void Roboremo_printf(const char* format, ...)
{
	va_list args;
	int n;
	DMA_Cmd(DMA1_Stream3, DISABLE);                      	//关闭DMA传输
	va_start(args, format);
	n = vsprintf((char*)USART3_TX_buf, format, args);
	va_end(args);
	
	while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE){}		//确保DMA可以被设置 
	DMA_SetCurrDataCounter(DMA1_Stream3,n);          		  //数据传输量
	DMA_Cmd(DMA1_Stream3, ENABLE);                      
}
/*内外环PID参数整定函数*/
void Roboremo_PID_tuning(float* Inner_p,float* Inner_i,float* Inner_d, float* Outter_p,float* Outter_i,float* Outter_d) //pid调参函数
{
	if(USART3_RX_updated)
	{
		USART3_RX_updated = 0;
		exeCmd();
	}
	switch(mode)//根据mode的不同调节不同的参数																		
	{
		case 1: mode = 0;
			*Inner_p = atof((char*)USART3_RX_buf + 5);
			break;
		
		case 2: mode = 0;
			*Inner_i = atof((char*)USART3_RX_buf + 5);
			break;
		
		case 3: mode = 0;
			*Inner_d = atof((char*)USART3_RX_buf + 5);
			break;
		
		case 4: mode = 0;
			*Outter_p = atof((char*)USART3_RX_buf + 6);
			break;
		
		case 5: mode = 0;
			*Outter_i = atof((char*)USART3_RX_buf + 6);
			break;
		
		case 6: mode = 0;
			*Outter_d = atof((char*)USART3_RX_buf + 6);
			break;
		
		default:
		  break;
	}
}
/*功率限制参数整定函数*/
void Roboremo_Power_tuning(float* Total_Limit,float* Power_Threshold) //功率调参函数
{
	if(USART3_RX_updated)
	{
		USART3_RX_updated = 0;
		exeCmd();
	}
	switch(mode)//根据mode的不同调节不同的参数																		
	{
		case 7: mode = 0;
			*Total_Limit = atof((char*)USART3_RX_buf + 5);
			break;
		
		case 8: mode = 0;
			*Power_Threshold = atof((char*)USART3_RX_buf + 5);
			break;
		
		default:
		  break;
	}
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET) 	//空闲中断判断数据是否通过串口传输完成
	{
		DMA_Cmd(DMA1_Stream1, DISABLE); 										//关闭DMA，防止处理期间有数据
		USART3_RX_updated = 1;																	//置1表示一次接收数据完成
		USART3->SR;  
		USART3->DR;  																						//读SR再读DR 清除IDLE
		USART3_RX_buf_LENTH =U3_RXMAX_len - DMA1_Stream1->NDTR;				//获得串口发送过来的字符的长度//DMA剩余
		
		memset(USART3_RX_buf+USART3_RX_buf_LENTH,0,DMA1_Stream1->NDTR);		//后面的全清零//重新覆盖接收数组//数组首地址  这一句话也可以直接Init
		
		DMA_ClearFlag(DMA1_Stream1, DMA_IT_TCIF1);
		DMA1_Stream1->NDTR = U3_RXMAX_len;		  								//重新写入传输量
		DMA_Cmd(DMA1_Stream1, ENABLE);     									//重新打开 
	}  
}

//void DMA1_Stream1_IRQHandler(void)											//接受的中断DMA
//{
//	if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
//	{
//		DMA_ClearFlag(DMA1_Stream1, DMA_IT_TCIF1);
//		DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
//	}
//}

void DMA1_Stream3_IRQHandler(void)											//用于发送的DMA
{
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3))
	{
		DMA_ClearFlag(DMA1_Stream3, DMA_IT_TCIF3);
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
	}
}
