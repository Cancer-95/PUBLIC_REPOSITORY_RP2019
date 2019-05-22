#ifndef __MY_MATH_H
#define	__MY_MATH_H

#include "system.h"

#define abs(x) ((x)>0? (x):(-(x)))

#define VAL_LIMIT(val, min, max) \
do {\
if((val) <= (min))\
{\
  (val) = (min);\
}\
else if((val) >= (max))\
{\
  (val) = (max);\
}\
} while(0)\

void critical_value_treatment(float get, float *set,float mid,float range);//临界值处理

void moving_average_filter(float data_in, u8 num, float *data_out);//滑动平均值滤波器

float constrain(float amt, float low, float high);
int16_t constrain_int16(int16_t amt, int16_t low, int16_t high);
int32_t constrain_int32(int32_t amt, int32_t low, int32_t high);

#endif
