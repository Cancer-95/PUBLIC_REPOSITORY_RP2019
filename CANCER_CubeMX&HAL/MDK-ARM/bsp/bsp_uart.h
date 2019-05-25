/****************************************************************************
 *  Copyright (C) 2019 Cancer.Chen
 ***************************************************************************/
/** @file bsp_uart.h
 *  @version 1.0
 *  @date Apr 2019
 *
 *  @brief uart receive data from DBus/judge_system etc.
 *
 *  @copyright 2019 Cancer.Chen. All rights reserved.
 *
 */

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "bsp_sys.h"

#define UART_RX_DMA_SIZE       1024

#define DBUS_MAX_LEN 50
#define DBUS_BUFLEN  18

void dbus_uart_init(void);
//void computer_uart_init(void);
//void judgement_uart_init(void);

void uart_receive_handler(UART_HandleTypeDef *huart);

uint8_t dma_current_memory_target(DMA_Stream_TypeDef *dma_stream);
uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream);

////extern uint8_t judge_dma_rxbuff[2][UART_RX_DMA_SIZE];
////extern uint8_t pc_dma_rxbuff[2][UART_RX_DMA_SIZE];

#endif
