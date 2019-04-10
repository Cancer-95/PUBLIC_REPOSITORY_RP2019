#include "vision.h"

#define    UART4_TX_LEN_HEADER    sizeof(extuart4_sendheader_t)    //帧头长
#define    UART4_TX_LEN_DATA      sizeof(extuart4_senddata_t)      //可自定义数据段长度,
#define    UART4_TX_LEN_TAIL      sizeof(extuart4_sendtail_t)	     //帧尾CRC16
#define    UART4_TX_LEN_PACKED    UART4_TX_LEN_HEADER + UART4_TX_LEN_DATA + UART4_TX_LEN_TAIL       //整包长度

#define    UART4_RX_LEN_HEADER    UART4_TX_LEN_HEADER
#define    UART4_RX_LEN_PACKED    sizeof(extuart4_recvdata_t)

//是否发了新数据给我,FALSE没有,TRUE发了新的
uint8_t UART4_Data_Update = FALSE;

//接收到的数据暂存在这里
unsigned char uart4_rx_buffer[100] = {0};

/*发送的相关结构体*/
extuart4_sendheader_t    uart4_sendheader;  //头
extuart4_senddata_t      uart4_senddata;    //发送结构体
extuart4_sendtail_t      uart4_sendtail;    //尾

/*接收的相关结构体*/
extuart4_recvdata_t      uart4_recvdata;    //接收结构体
 
/*视觉相关结构体*/
vision_t                 vision;            //视觉结构体

void vision_init(void)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); 

	// 串口4对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4);

	// UART4端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 		//上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	// UART4初始化设置
	USART_InitStructure.USART_BaudRate = 115200;					//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	    //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(UART4, &USART_InitStructure); 										            //初始化串口5
	
	USART_Cmd(UART4, ENABLE);  						                                  //使能串口1
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);                           //开启IDLE相关中断
//	USART_ClearFlag(UART4, USART_FLAG_TC);

	//uart4 NVIC 配置
	nvic_config(UART4_IRQn,UART4_IRQn_pre,UART4_IRQn_sub);// Configure NVIC 
//	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		//串口4中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //抢占优先级2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);							        //根据指定的参数初始化VIC寄存器

	//DMA串口请求中断
	USART_DMACmd( UART4, USART_DMAReq_Rx, ENABLE );
	USART_DMACmd( UART4, USART_DMAReq_Tx, ENABLE );

//UART4_DMA_Init:

	DMA_DeInit( DMA1_Stream2 );
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//方向外设到存储器

	DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)&(UART4->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)uart4_rx_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 100;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init( DMA1_Stream2, &DMA_InitStructure );	
	DMA_Cmd( DMA1_Stream2, ENABLE);  //stream2

}

/***********************************************************************/
/*                            接收函数		                           */
/***********************************************************************/

void uart4_read_data(uint8_t *data)
{
	//判断帧头数据是否为0xA5
	if(data[0] == 0xA5)
	{
		//帧头CRC8校验		
		if(Verify_CRC8_Check_Sum( data, UART4_RX_LEN_HEADER ) == TRUE)
		{
			//帧尾CRC16校验
			if(Verify_CRC16_Check_Sum( data, UART4_RX_LEN_PACKED ) == TRUE)
			{
				//接收数据拷贝
				memcpy( &uart4_recvdata, data, sizeof(extuart4_recvdata_t));	
				UART4_Data_Update = YES;//标记数据更新了
			}
			else UART4_Data_Update = NO;
		}
		else UART4_Data_Update = NO;
	}
	else UART4_Data_Update = NO;
}

int uart4_datalength = 0;
void UART4_IRQHandler(void)
{
  if(USART_GetITStatus(UART4,USART_IT_IDLE)!=RESET)//检测到空闲线路
	{		
		
		UART4->SR ;
		UART4->DR ;
			
		DMA_Cmd(DMA1_Stream2,DISABLE );
		
		uart4_datalength = 100 - DMA_GetCurrDataCounter(DMA1_Stream2);

		/*读取与赋值*/
		uart4_read_data(uart4_rx_buffer);//读取缓存区数据	
		if(UART4_Data_Update)
		{
			vision.RX_Cmd = uart4_recvdata.CmdID;
			vision.captured = uart4_recvdata.captured;
			vision.y_fdb = uart4_recvdata.y_fdb;		
		}

				
		memset(uart4_rx_buffer+uart4_datalength, 0, DMA1_Stream2->NDTR);		//后面的全清零//重新覆盖接收数组//数组首地址
 		DMA1_Stream2->NDTR=100;
		
		DMA_Cmd(DMA1_Stream2,ENABLE);//D1S2
	}
	
}

/***********************************************************************/
/*                            发送函数		                             */
/***********************************************************************/
void uart4_sendchar(u8 data)
{
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET); 
	USART_SendData(UART4,data);   
} 

uint8_t uart4_send_pack[UART4_TX_LEN_PACKED] = {0};//大于UART4_LEN_PACKED就行
void uart4_send_data( uint8_t CmdID, uint8_t data_0)
{
	int i;    //循环发送次数

	uart4_sendheader.SOF = 0xA5;
	uart4_sendheader.CmdID = CmdID;//命令ID 00不要给我发东西了 01岛下 02岛上
	
	//写入帧头
	memcpy( uart4_send_pack, &uart4_sendheader, UART4_TX_LEN_HEADER );
	
	//帧头CRC8校验协议
	Append_CRC8_Check_Sum( uart4_send_pack, UART4_TX_LEN_HEADER );//在帧头的最末尾
	
	//发送的数据
	uart4_senddata.data_0 = data_0;

	memcpy( uart4_send_pack + UART4_TX_LEN_HEADER, &uart4_senddata, UART4_TX_LEN_DATA);
	
	//帧尾CRC16校验协议
	Append_CRC16_Check_Sum( uart4_send_pack, UART4_TX_LEN_PACKED );//在帧尾
	
	//将打包好的数据通过串口移位发送出去
	for (i = 0; i < UART4_TX_LEN_PACKED; i++)
	{
		uart4_sendchar( uart4_send_pack[i] );
	}
}

/***********************************************************************/
/*                            具体任务函数		                         */
/***********************************************************************/
//void vision_param_init(void)
//{
//  vision.y_ref = 602;	
//}

void vision_send_task(void)
{
	if(vision.enable)
	{
		uart4_send_data(vision.TX_Cmd, 0);//给视觉发东西 指令：00不要给我发东西了 01岛下 02岛上
	}
}

float ppp;
float iii;
float ddd;
void vision_task(void)
{
	if(vision.enable)
	{		
		chassis.ass_mode_enable = ENABLE;        //开启车尾模式
		
		if(climb_tube.location == ON_GROUND)
		{
			if(feed_bomb.ctrl_mode == AIM_FEED_MODE)
			{
			  pidckz_param_set(&pid_vision_chassis_vy, 500, 100, 5.0f, 0.001, 10);   
        vision.y_ref = 140;	
				LED_CAMERA_ON;
				vision.TX_Cmd = 3;
			}
			else
			{
				pidckz_param_set(&pid_vision_chassis_vy, 500, 100, 1.8f, 0.0045f, 1.8f);   
				vision.y_ref = 680;	
				LED_CAMERA_ON;
				vision.TX_Cmd = 1;
				
				lifter.target_bomb_bit[L] = lifter.target_bomb_bit_vision[L];
				lifter.target_bomb_bit[R] = lifter.target_bomb_bit_vision[R];	
				from_lifter_zero = YES;
				from_lifter_bomb_entire = NO; //修正框架目标值				
				lifter.ctrl_mode = BOMB_LIFTER_BIT;//框架抬升一点(修改)	
				
				if(vision.delay_record && ((abs(lifter.target_bomb_bit[L] - lifter.total_angle[L]) < 100) || (abs(lifter.target_bomb_bit[R] - lifter.total_angle[R]) < 100)))
				{
					vision.delay_record = DISABLE;
					vision.light_delay_time = millis();
					vision.quit_delay_time = millis();
				}			
				if((millis() - vision.light_delay_time > 0) && vision.captured &&
					((abs(lifter.target_bomb_bit[L] - lifter.total_angle[L]) < 100) || 
					 (abs(lifter.target_bomb_bit[R] - lifter.total_angle[R]) < 100)))
				{
					chassis.ctrl_mode = VISION_GET_BOMB;
					if((millis() - vision.quit_delay_time > 3000) && (abs(pid_vision_chassis_vy.err)) < 20)
					{
						flag_bomb_claw_vision = 0;
				  	vision.enable = DISABLE;  //结束视觉对位
					}
					
				}				
			}

		}
		else if(climb_tube.location == ON_ISLAND)
		{
			if(feed_bomb.ctrl_mode != AIM_FEED_MODE)//岛上在补弹对准的情况下按下shift+V是没有用的
			{
				pidckz_param_set(&pid_vision_chassis_vy, 500, 100, 2.0f, 0.0045f, 1.8f);   
				vision.y_ref = 602;	
				LED_CAMERA_ON;
				vision.TX_Cmd = 2;
				
				if(vision.delay_record)
				{
					vision.delay_record = DISABLE;
					vision.light_delay_time = millis();
				}	
				if((millis() - vision.light_delay_time > 180) && vision.captured)
				{
					chassis.ctrl_mode = VISION_GET_BOMB;
				}					
			}
		}


	}
	else 
	{
		/*修正框架目标值*/
    if(get_bomb.ctrl_mode != PUSH_AIM_SCAN_MODE)//在扫描模式下就不要乱动框架值了
		{
			if(lifter.target_bomb_bit[L] != lifter.target_bomb_bit_normal[L] || 
				 lifter.target_bomb_bit[R] != lifter.target_bomb_bit_normal[R])		
			{					
				lifter.target_bomb_bit[L] = lifter.target_bomb_bit_normal[L];
				lifter.target_bomb_bit[R] = lifter.target_bomb_bit_normal[R];
				
				if((lifter.total_angle[L] > lifter.target_bomb_bit[L]) && 
					 (lifter.total_angle[R] < lifter.target_bomb_bit[R]))
				{
					from_lifter_zero = YES;
					from_lifter_bomb_entire = NO; 			
				}
				else if((lifter.total_angle[L] < lifter.target_bomb_bit[L]) && 
					      (lifter.total_angle[R] > lifter.target_bomb_bit[R]))
				{
					from_lifter_zero = NO;
					from_lifter_bomb_entire = YES; 			
				}
			}		
		}			

		LED_CAMERA_OFF;
		vision.delay_record = ENABLE;
		vision.TX_Cmd = 0;//不要给我发东西了
		vision.captured = 0;//没抓住图
	}

}
