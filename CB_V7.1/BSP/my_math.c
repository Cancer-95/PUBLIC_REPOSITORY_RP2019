#include "my_math.h"

/**
	* @brief      临界值处理
	* @param[in]  range: 编码器(陀螺仪)总范围 即首尾读数之差的绝对值 如0~360或-180~180 range都为360
	* @param[in]  mid: 范围数值零点 如0~360则mid为180 如-180~180则mid为0
	* @note       以下是改进的临界值处理函数 增加了一个入口参数
	*/
void critical_value_treatment(float get, float *set,float mid,float range)//临界值处理
{
	if((*set < mid) && (get > *set + range / 2))
		*set = *set + range;
	else if((*set > mid) && (get < *set - range / 2))
		*set = *set - range;
	else 
		*set = *set;
}

void moving_average_filter(float data_in, u8 num, float *data_out)//滑动平均值滤波器(num:取样个数)
{
	u8 i;
	float buf[num+1];
	float sum = 0;

	buf[num] = data_in;
	for(i = 0; i < num; i++)
	{
		buf[i] = buf[i+1];
		sum += buf[i];
	}
	*data_out = sum / num;
}

//限幅
float constrain(float amt, float low, float high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}
 
int16_t constrain_int16(int16_t amt, int16_t low, int16_t high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}

int32_t constrain_int32(int32_t amt, int32_t low, int32_t high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}

int constrain_int(int amt,int low,int high)
{
	if (amt < low)
		return low;
	else if (amt > high)
		return high;
	else
		return amt;
}


