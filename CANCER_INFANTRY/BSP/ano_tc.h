#ifndef __ANO_TC_H
#define __ANO_TC_H	 
#include "system.h"

#define U6_TXMAX_len 128
#define U6_RXMAX_len 128

void ANO_TC_Init(void);

void ANO_TC_Report(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);

void ANO_TC_Report16(short test);
void ANO_TC_Report32(int32_t test);

#endif

