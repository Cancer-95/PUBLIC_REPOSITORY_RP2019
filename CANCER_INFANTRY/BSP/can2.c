#include "can2.h"

/*----CAN2_TX-----PB13----*/
/*----CAN2_RX-----PB12----*/


int16_t Trigger_Motor[2];

CanRxMsg CAN2_RxMessage;    //发送缓冲区
CanTxMsg CAN2_TxMessage;    //接受缓冲区

void CAN2_Init(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);  //CAN2若要单独使用必须先使能CAN1时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 ;
    gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_100MHz;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2); 

	  nvic_config(CAN2_RX0_IRQn,CAN2_RX0_IRQn_pre,CAN2_RX0_IRQn_sub);

    CAN_DeInit(CAN2);
    CAN_StructInit(&can);

    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;    
    can.CAN_AWUM = DISABLE;    
    can.CAN_NART = DISABLE;    
    can.CAN_RFLM = DISABLE;    
    can.CAN_TXFP = ENABLE;     
    can.CAN_Mode = CAN_Mode_Normal; 
		
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
    can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN2, &can);
    
    can_filter.CAN_FilterNumber=14;
    can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filter.CAN_FilterIdHigh=0x0000;
    can_filter.CAN_FilterIdLow=0x0000;
    can_filter.CAN_FilterMaskIdHigh=0x0000;
    can_filter.CAN_FilterMaskIdLow=0x0000;
    can_filter.CAN_FilterFIFOAssignment=0;//the message which pass the filter save in fifo0 CAN_Filter_FIFO0
    can_filter.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filter);
    
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);    ////FIFO0消息挂号中断允许
		CAN_ITConfig(CAN2,CAN_IT_TME,ENABLE);
} 

void CAN2_Send(uint32_t Equipment_ID,int16_t Data)
{				
  CAN2_TxMessage.StdId = Equipment_ID;		 //使用的标准ID
  CAN2_TxMessage.IDE = CAN_ID_STD;				 //标准模式
  CAN2_TxMessage.RTR = CAN_RTR_DATA;			 //发送的是数据
  CAN2_TxMessage.DLC = 8;							 //数据长度为8字节
	
  CAN2_TxMessage.Data[0] = 0;
  CAN2_TxMessage.Data[1] = 0;
  CAN2_TxMessage.Data[2] = 0;
  CAN2_TxMessage.Data[3] = 0;
	CAN2_TxMessage.Data[4] = Data>>8;      
	CAN2_TxMessage.Data[5] = Data;
  CAN2_TxMessage.Data[6] = 0;
  CAN2_TxMessage.Data[7] = 0;
	
	CAN_Transmit(CAN2, &CAN2_TxMessage);
}

void CAN2_RX0_IRQHandler(void)
{
		if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET) 
    {				 
				CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
				CAN_Receive(CAN2, CAN_FIFO0, &CAN2_RxMessage);

				if(CAN2_RxMessage.StdId == 0x207)			//Trigger_Motor RM2006
				{
						Trigger_Motor[TRI_ANGLE] = (int16_t)(CAN2_RxMessage.Data[0]<<8|CAN2_RxMessage.Data[1]);					  //机械角度	0~8191(0x1FFF)
						Trigger_Motor[TRI_SPEED] = (int16_t)(CAN2_RxMessage.Data[2]<<8 | CAN2_RxMessage.Data[3]);		//实际电流转矩测量值	-13000 ~ 13000			
				}		
		}
}




