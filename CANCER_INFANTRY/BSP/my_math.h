#ifndef __MY_MATH_H
#define	__MY_MATH_H

#include "system.h"

//void Critical_Value_Treatment(float get, float *set,float mid);//临界值处理

void Critical_Value_Treatment(float get, float *set,float mid,float range);//临界值处理

void Moving_Average_Filter(float data_in, u8 num, float *data_out);//滑动平均值滤波器

#endif
