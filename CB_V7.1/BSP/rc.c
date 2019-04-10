/*****************************************************************************************************************
 * @file 		  rc.c 
 * @version 	0.0
 * @date 		  2018.10.23
 *
 * @brief     这是一个Cancer抄袭官方开源步兵的一个遥控器控制程序 能用但还没经过充分测试 不知道会不会有bug
 *            其中使用了串口二的接收端(空闲中断)
 *  					RX		PA3			DMA1_Stream5_ch4
 *****************************************************************************************************************/

#include "rc.h"

static uint8_t _USART2_DMA_RX_BUF[2][USART2_DMA_RX_BUF_LEN];
RC_Ctl_t RC_Ctl;

void RC_init()
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart2;
	DMA_InitTypeDef dma;
//	NVIC_InitTypeDef nvic;
	
/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_USART2);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_3 ;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio);
	
	USART_DeInit(USART2);
	USART_StructInit(&usart2);
	usart2.USART_BaudRate = 100000;
	usart2.USART_WordLength = USART_WordLength_8b;
	usart2.USART_StopBits = USART_StopBits_1;
	usart2.USART_Parity = USART_Parity_Even;
	usart2.USART_Mode = USART_Mode_Rx;
	usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2,&usart2);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);

/* -------------- Configure DMA -----------------------------------------*/

	DMA_DeInit(DMA1_Stream5);
	DMA_StructInit(&dma);
	dma.DMA_Channel = DMA_Channel_4;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)&_USART2_DMA_RX_BUF[0][0];
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma.DMA_BufferSize = sizeof(_USART2_DMA_RX_BUF)/2;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_Medium;
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5,&dma);
	
	//配置Memory1,Memory0是第一个使用的Memory
  DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t)&_USART2_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
  DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);

	DMA_Cmd(DMA1_Stream5,ENABLE);
	
/* -------------- Configure NVIC ---------------------------------------*/	
//	nvic.NVIC_IRQChannel = USART2_IRQn;
//	nvic.NVIC_IRQChannelPreemptionPriority = USART2_IRQn_pre;
//	nvic.NVIC_IRQChannelSubPriority = USART2_IRQn_sub;
//	nvic.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&nvic);	
	nvic_config(USART2_IRQn,USART2_IRQn_pre,USART2_IRQn_sub);
	
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//usart rx idle interrupt  enabled
	USART_Cmd(USART2,ENABLE);
}


void delay_ms1(unsigned int t)
{
	int i;
	for( i=0;i<t;i++)
	{
		int a=42000;
		while(a--);
	}
}

void RemoteDataProcess(uint8_t *pData)
{	
	if(pData == NULL)
  {
    return;
  }
    
  RC_Ctl.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
  RC_Ctl.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
  RC_Ctl.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |((int16_t)pData[4] << 10)) & 0x07FF;
  RC_Ctl.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
   
  RC_Ctl.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
  RC_Ctl.rc.s2 = ((pData[5] >> 4) & 0x0003);

  RC_Ctl.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
  RC_Ctl.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
  RC_Ctl.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    

  RC_Ctl.mouse.press_l = pData[12];
  RC_Ctl.mouse.press_r = pData[13];
 
  RC_Ctl.key.v = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);

/*SHIFT+CTRL+B重启CPU*/
//	if((RC_Ctl.rc.s1 != 1)&&(RC_Ctl.rc.s1 != 2)&&(RC_Ctl.rc.s1 != 3))		
	if((RC_Ctl.key.v & 0x10) == KEY_PRESSED_OFFSET_SHIFT && (RC_Ctl.key.v & 0x20) == KEY_PRESSED_OFFSET_CTRL && (RC_Ctl.key.v & 0x8000) == KEY_PRESSED_OFFSET_B)	
	{
		delay_ms1(2);
		__set_FAULTMASK(1);//关闭总中断
		NVIC_SystemReset();//请求重启CPU
	}   
}

//串口接收中断服务函数
int8_t RC_connect=0;
void USART2_IRQHandler(void)
{
	static uint32_t this_time_rx_len = 0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)USART2->SR;
		(void)USART2->DR;
   	RC_connect=0;//清除丢控计数
		
		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream5) == 0)
		{
			DMA_Cmd(DMA1_Stream5, DISABLE);
			this_time_rx_len = USART2_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
			DMA1_Stream5->NDTR = (uint16_t)USART2_DMA_RX_BUF_LEN;         //relocate the dma memory pointer to the beginning position
			DMA1_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA1_Stream5, ENABLE);
      if(this_time_rx_len == RC_FRAME_LENGTH)
			{
				RemoteDataProcess(_USART2_DMA_RX_BUF[0]);
			}
		}
		//Target is Memory1
		else 
		{
			DMA_Cmd(DMA1_Stream5, DISABLE);
			this_time_rx_len = USART2_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
			DMA1_Stream5->NDTR = (uint16_t)USART2_DMA_RX_BUF_LEN;          //relocate the dma memory pointer to the beginning position
			DMA1_Stream5->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA1_Stream5, ENABLE);
      if(this_time_rx_len == RC_FRAME_LENGTH)
			{
				RemoteDataProcess(_USART2_DMA_RX_BUF[1]);
			}
		}
	}       
}

void RC_offline_handler()//丢控保护
{
	RC_connect++;
	if(RC_connect > 15)
	{
		RC_connect = 15;				
	  RC_Ctl.rc.ch0 = RC_Ctl.rc.ch1 = RC_Ctl.rc.ch2 = RC_Ctl.rc.ch3 = RC_CH_VALUE_OFFSET;
	  RC_Ctl.rc.s1 	= RC_Ctl.rc.s2  = RC_SW_OFFLINE;
	}
}


