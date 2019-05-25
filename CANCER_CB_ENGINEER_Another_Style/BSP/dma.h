#ifndef __DMA_H
#define __DMA_H

#include "system.h"

#define USART2_RX_DMA_Stream		          DMA1_Stream5
#define UART4_RX_DMA_Stream		    	      DMA1_Stream2
#define UART5_RX_DMA_Stream			          DMA1_Stream0

#define RC_USART_RX_DMA_Stream		        DMA1_Stream5
#define VISION_UART_RX_DMA_Stream		    	DMA1_Stream2
#define JUDGEMENT_UART_RX_DMA_Stream			DMA1_Stream0


//#define USART2_TX_DMA_Stream		DMA1_Stream6
//#define UART4_TX_DMA_Stream			DMA1_Stream4
//#define UART5_TX_DMA_Stream			DMA1_Stream7

#define USART_DMA_Channel				DMA_Channel_4

void DMA_init(void);

#endif
