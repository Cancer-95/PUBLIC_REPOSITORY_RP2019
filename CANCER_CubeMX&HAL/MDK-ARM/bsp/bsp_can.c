/****************************************************************************
 *  Copyright (C) 2019 Cancer.Chen
 ***************************************************************************/
/** @file bsp_can.c
 *  @version 1.0
 *  @date Apr 2019
 *
 *  @brief receive external can device message
 *
 *  @copyright 2019 Cancer.Chen
 *
 */

#include "bsp_can.h"

moto_measure_t moto_chassis[4];
moto_measure_t moto_trigger;

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *_hcan)
{
  switch (_hcan->pRxMsg->StdId)
  {
    case CAN_3510_M1_ID:
    case CAN_3510_M2_ID:
    case CAN_3510_M3_ID:
    case CAN_3510_M4_ID:
    {
      static uint8_t i;
      i = _hcan->pRxMsg->StdId - CAN_3510_M1_ID;

      moto_chassis[i].msg_cnt++ <= 50 ? get_moto_offset(&moto_chassis[i], _hcan) : encoder_data_handler(&moto_chassis[i], _hcan);
    }
    break;
    
		case CAN_TRIGGER_MOTOR_ID:
    {
      if (_hcan == &TRIGGER_CAN)
      {
        moto_trigger.msg_cnt++ <= 10 ? get_moto_offset(&moto_trigger, _hcan) : encoder_data_handler(&moto_trigger, _hcan);
      }
      else
      {
      }
    }
    break;
		

    default:
    {
    }
    break;
  }
  
  __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
  __HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  __HAL_CAN_ENABLE_IT(hcan, CAN_IT_FMP0);
}

/**
  * @brief     get motor rpm and calculate motor round_count/total_encoder/total_angle
  * @param     ptr: Pointer to a moto_measure_t structure
  * @attention this function should be called after get_moto_offset() function
  */
void encoder_data_handler(moto_measure_t* ptr, CAN_HandleTypeDef* hcan)
{
  ptr->last_ecd = ptr->ecd;
  ptr->ecd      = (uint16_t)(hcan->pRxMsg->Data[0] << 8 | hcan->pRxMsg->Data[1]);
  
  if (ptr->ecd - ptr->last_ecd > 4096)
  {
    ptr->round_cnt--;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd - 8192;
  }
  else if (ptr->ecd - ptr->last_ecd < -4096)
  {
    ptr->round_cnt++;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd + 8192;
  }
  else
  {
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd;
  }

  ptr->total_ecd = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;
  /* total angle, unit is degree */
  ptr->total_angle = ptr->total_ecd / ENCODER_ANGLE_RATIO;
  
  ptr->speed_rpm     = (int16_t)(hcan->pRxMsg->Data[2] << 8 | hcan->pRxMsg->Data[3]);
  ptr->given_current = (int16_t)(hcan->pRxMsg->Data[4] << 8 | hcan->pRxMsg->Data[5]);


}

/**
  * @brief     get motor initialize offset value
  * @param     ptr: Pointer to a moto_measure_t structure
  * @retval    None
  * @attention this function should be called after system can init
  */
void get_moto_offset(moto_measure_t* ptr, CAN_HandleTypeDef* hcan)
{
    ptr->ecd        = (uint16_t)(hcan->pRxMsg->Data[0] << 8 | hcan->pRxMsg->Data[1]);
    ptr->offset_ecd = ptr->ecd;
}
/**
  * @brief   can filter initialization
  * @param   CAN_HandleTypeDef
  * @retval  None
  */
void can_device_init(void)
{
  //can1 &can2 use same filter config
  CAN_FilterConfTypeDef  can_filter;
  static CanTxMsgTypeDef Tx1Message;
  static CanRxMsgTypeDef Rx1Message;
  static CanTxMsgTypeDef Tx2Message;
  static CanRxMsgTypeDef Rx2Message;

  can_filter.FilterNumber         = 0;
  can_filter.FilterMode           = CAN_FILTERMODE_IDMASK;
  can_filter.FilterScale          = CAN_FILTERSCALE_32BIT;
  can_filter.FilterIdHigh         = 0x0000;
  can_filter.FilterIdLow          = 0x0000;
  can_filter.FilterMaskIdHigh     = 0x0000;
  can_filter.FilterMaskIdLow      = 0x0000;
  can_filter.FilterFIFOAssignment = CAN_FilterFIFO0;
  can_filter.BankNumber           = 14;
  can_filter.FilterActivation     = ENABLE;
  HAL_CAN_ConfigFilter(&hcan1, &can_filter);
  //while (HAL_CAN_ConfigFilter(&hcan1, &can_filter) != HAL_OK);
  
  can_filter.FilterNumber         = 14;
  HAL_CAN_ConfigFilter(&hcan2, &can_filter);
  //while (HAL_CAN_ConfigFilter(&hcan2, &can_filter) != HAL_OK);
    
  hcan1.pTxMsg = &Tx1Message;
  hcan1.pRxMsg = &Rx1Message;
  hcan2.pTxMsg = &Tx2Message;
  hcan2.pRxMsg = &Rx2Message;
}



void can_receive_start(void)
{
  HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
  HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0); 
}

/**
  * @brief  send calculated current to motor
  * @param  3510 motor ESC id
  */
void send_chassis_cur(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
  CHASSIS_CAN.pTxMsg->StdId   = 0x200;
  CHASSIS_CAN.pTxMsg->IDE     = CAN_ID_STD;
  CHASSIS_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
  CHASSIS_CAN.pTxMsg->DLC     = 0x08;
  CHASSIS_CAN.pTxMsg->Data[0] = iq1 >> 8;
  CHASSIS_CAN.pTxMsg->Data[1] = iq1;
  CHASSIS_CAN.pTxMsg->Data[2] = iq2 >> 8;
  CHASSIS_CAN.pTxMsg->Data[3] = iq2;
  CHASSIS_CAN.pTxMsg->Data[4] = iq3 >> 8;
  CHASSIS_CAN.pTxMsg->Data[5] = iq3;
  CHASSIS_CAN.pTxMsg->Data[6] = iq4 >> 8;
  CHASSIS_CAN.pTxMsg->Data[7] = iq4;
  HAL_CAN_Transmit(&CHASSIS_CAN, 10);
}
