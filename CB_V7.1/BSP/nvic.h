#ifndef __NVIC_H
#define __NVIC_H	
#include "system.h"

/*************************PreemptionPriority*************************/
#define USART2_IRQn_pre				      0						//using 遥控器接收
#define CAN1_RX0_IRQn_pre						0           //using CAN1接收中断
#define CAN2_RX0_IRQn_pre						0           //using CAN2接收中断
#define USART3_IRQn_pre							0						//using Roboremo串口三空闲中断

#define DMA1_Stream3_IRQn_pre				1						//using Roboremo串口三TX
#define UART5_IRQn_pre							1						//using 裁判系统串口五空闲中断
#define DMA1_Stream7_IRQn_pre				1						//using 裁判系统串口五TX
#define USART6_IRQn_pre							1						//using 匿名串口六空闲中断

#define DMA2_Stream6_IRQn_pre       2           //using 匿名串口六TX
#define UART4_IRQn_pre			   			2           //using 视觉接口串口四空闲中断
#define DMA1_Stream2_IRQn_pre				2           //using 视觉接口串口四TX
#define TIM7_IRQn_pre								2


/*****************************SubPriority*****************************/
#define USART2_IRQn_sub				      0						//using 遥控器接收
#define CAN1_RX0_IRQn_sub						1           //using CAN1接收中断
#define CAN2_RX0_IRQn_sub						2           //using CAN2接收中断
#define USART3_IRQn_sub							3						//using Roboremo串口三空闲中断

#define DMA1_Stream3_IRQn_sub				0						//using Roboremo串口三TX
#define UART5_IRQn_sub							1						//using 裁判系统串口五空闲中断
#define DMA1_Stream7_IRQn_sub				2						//using 裁判系统串口五TX
#define USART6_IRQn_sub							3						//using 匿名串口六空闲中断

#define DMA2_Stream6_IRQn_sub       0           //using 匿名串口六TX
#define UART4_IRQn_sub  						1           //using 视觉接口串口四空闲中断
#define DMA1_Stream2_IRQn_sub				2           //using 视觉接口串口四TX
#define TIM7_IRQn_sub								3
 							   
void nvic_config(u8 IRQChannel,u8 PreemptionPriority,u8 SubPriority);
								 
#endif 

