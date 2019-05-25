#include "dma.h"

void DMA_Single_Memory_Config(DMA_Stream_TypeDef *DMA_Streamx,
															u32                chx,
															u32                PeripheralBaseAddr,
															u32                MemoryBaseAddr,
															u16                buf_len,
															u32                direction)
{ 
	DMA_InitTypeDef  DMA_InitStructure;
	
	if((u32)DMA_Streamx>(u32)DMA2)																						//得到当前stream属于DMA还是DMA1
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);											//DMA2时钟使能
	else 
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);											//DMA1时钟使能
				
  DMA_DeInit(DMA_Streamx);
	DMA_StructInit(&DMA_InitStructure);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}												//等待DMA可配置

	DMA_InitStructure.DMA_Channel            = chx;  																		//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = PeripheralBaseAddr;						//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr    = MemoryBaseAddr;										//DMA存储器地址
  DMA_InitStructure.DMA_DIR                = direction;																		//数据传输方向
  DMA_InitStructure.DMA_BufferSize         = buf_len;																//数据传输量
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;					//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;										//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//外设数据长度：8位
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;						//存储器数据长度：8位
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;															//使用环回模式
	DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;										//最高优先级
  DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;								//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;				//外设突发单次传输
			
  DMA_Init(DMA_Streamx, &DMA_InitStructure);	
	
	DMA_Cmd(DMA_Streamx, ENABLE);
} 

void DMA_Double_Memory_Config(DMA_Stream_TypeDef *DMA_Streamx,
															u32                chx,
															u32                PeripheralBaseAddr,
															u32                Memory0BaseAddr,//Memory0
                              u32                Memory1BaseAddr,//Memory1
															u16                buf_len,
															u32                direction)
{ 
	DMA_InitTypeDef  DMA_InitStructure;
	
	if((u32)DMA_Streamx>(u32)DMA2)																						//得到当前stream属于DMA还是DMA1
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);											//DMA2时钟使能
	else 
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);											//DMA1时钟使能
				
  DMA_DeInit(DMA_Streamx);
	DMA_StructInit(&DMA_InitStructure);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}												//等待DMA可配置

	DMA_InitStructure.DMA_Channel            = chx;  																		//通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = PeripheralBaseAddr;						//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr    = Memory0BaseAddr;										//DMA存储器地址
  DMA_InitStructure.DMA_DIR                = direction;																		//数据传输方向
  DMA_InitStructure.DMA_BufferSize         = buf_len;																//数据传输量
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;					//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;										//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//外设数据长度：8位
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;						//存储器数据长度：8位
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;															//使用环回模式
	DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;										//中等优先级
  DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;								//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;				//外设突发单次传输
			
  DMA_Init(DMA_Streamx, &DMA_InitStructure);	
	
	//配置Memory1,Memory0是第一个使用的Memory
	DMA_DoubleBufferModeConfig(DMA_Streamx, Memory1BaseAddr, DMA_Memory_0);   //first used memory configuration
  DMA_DoubleBufferModeCmd(DMA_Streamx, ENABLE);
	
	DMA_Cmd(DMA_Streamx, ENABLE);
} 


void RC_DMA_init(void)//usart2
{
	DMA_Double_Memory_Config(RC_USART_RX_DMA_Stream,
													 USART_DMA_Channel,
													 (u32)&(USART2->DR),
													 (u32)&_USART2_DMA_RX_BUF[0][0],
	                         (u32)&_USART2_DMA_RX_BUF[1][0],
								           sizeof(_USART2_DMA_RX_BUF)/2,
													 DMA_DIR_PeripheralToMemory);
}

void judgement_DMA_init(void)//uart5
{
	DMA_Single_Memory_Config(UART5_RX_DMA_Stream,
													 USART_DMA_Channel,
													 (u32)&(UART5->DR),
													 (u32)&_UART5_DMA_RX_BUF[0],
								           sizeof(_UART5_DMA_RX_BUF),
													 DMA_DIR_PeripheralToMemory);
}

void vision_DMA_init(void)//uart4
{
	DMA_Single_Memory_Config(UART4_RX_DMA_Stream,
													 USART_DMA_Channel,
													 (u32)&(UART4->DR),
													 (u32)&_UART4_DMA_RX_BUF[0],
													 sizeof(_UART4_DMA_RX_BUF),
													 DMA_DIR_PeripheralToMemory);
}

void DMA_init(void)
{
	RC_DMA_init();
	judgement_DMA_init();
	vision_DMA_init();
}

