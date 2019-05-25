#include "my_math.h"

/*这是一个失败的临界值处理函数 说到底还是数学得学好 此教训留着作纪念*/
//void Critical_Value_Treatment(float get, float *set,float mid)//临界值处理
//{
//	if((*set < mid) && (get > *set + mid))
//		*set = *set + mid * 2;
//	else if((*set > mid) && (get < *set - mid))
//		*set = *set - mid * 2;
//	else 
//		*set = *set;
//}

/**
	* @brief      临界值处理
	* @param[in]  range: 编码器(陀螺仪)总范围 即首尾读数之差的绝对值 如0~360或-180~180 range都为360
	* @param[in]  mid: 范围数值零点 如0~360则mid为180 如-180~180则mid为0
	* @note       以下是改进的临界值处理函数 增加了一个入口参数
	*/
void Critical_Value_Treatment(float get, float *set,float mid,float range)//临界值处理
{
	if((*set < mid) && (get > *set + range / 2))
		*set = *set + range;
	else if((*set > mid) && (get < *set - range / 2))
		*set = *set - range;
	else 
		*set = *set;
}


void Moving_Average_Filter(float data_in, u8 num, float *data_out)//滑动平均值滤波器(num:取样个数)
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

