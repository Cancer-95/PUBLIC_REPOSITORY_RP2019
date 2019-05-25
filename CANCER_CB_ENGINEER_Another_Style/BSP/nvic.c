#include "nvic.h"

void NVIC_config(u8 IRQChannel,
	               u8 PreemptionPriority,
                 u8 SubPriority)
{
	NVIC_InitTypeDef  nvic;
	nvic.NVIC_IRQChannel = IRQChannel;
	nvic.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
	nvic.NVIC_IRQChannelSubPriority = SubPriority;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}
