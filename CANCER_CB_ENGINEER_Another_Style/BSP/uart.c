#include "uart.h"

/*常规设置*/
void USART_config(USART_TypeDef* USARTx, u32 BaudRate) //常规设置
{
	USART_InitTypeDef USART_InitStructure;
	
	if(USARTx == USART2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	  NVIC_config(USART2_IRQn, USART2_IRQn_pre, USART2_IRQn_sub);
	}
	if(USARTx == UART4)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	  NVIC_config(UART4_IRQn, UART4_IRQn_pre, UART4_IRQn_sub);
	}	
	if(USARTx == UART5)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
    NVIC_config(UART5_IRQn,UART5_IRQn_pre,UART5_IRQn_sub);
	}

  USART_DeInit(USARTx);
  USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate            = BaudRate;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	
	if(USARTx == RC_USART)//遥控器的配置稍稍有点不一样
	{
	  USART_InitStructure.USART_Parity              = USART_Parity_Even;
		USART_InitStructure.USART_Mode                = USART_Mode_Rx;
	}
	else 
	{
		USART_InitStructure.USART_Parity              = USART_Parity_No;
		USART_InitStructure.USART_Mode                = USART_Mode_Tx|USART_Mode_Rx;
	}

  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	USART_Init(USARTx,&USART_InitStructure);
	USART_Cmd(USARTx,ENABLE);
	
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) != SET);
}

void RC_USART_init(void)
{
	USART_config(RC_USART, 100000);
	
	USART_ITConfig(RC_USART,USART_IT_IDLE,ENABLE);

	USART_DMACmd(RC_USART,USART_DMAReq_Rx,ENABLE);
}

void judgement_UART_init(void)
{
	USART_config(JUDGEMENT_UART, 115200);
	
	USART_ITConfig(JUDGEMENT_UART, USART_IT_IDLE, ENABLE);									
	
	USART_DMACmd(JUDGEMENT_UART, USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(JUDGEMENT_UART, USART_DMAReq_Tx,ENABLE);
}

void vision_UART_init(void)
{
	USART_config(VISION_UART, 115200);
	
	USART_ITConfig(VISION_UART, USART_IT_IDLE, ENABLE);									
	
	USART_DMACmd(VISION_UART, USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(VISION_UART, USART_DMAReq_Tx,ENABLE);
}

void UART_init(void)
{
	RC_USART_init();
	judgement_UART_init();
	vision_UART_init();
}

