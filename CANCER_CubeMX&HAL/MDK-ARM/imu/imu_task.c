#include "imu_task.h"

void imu_param_init(void)
{
	pidckz_reset(&pid_imu_tmp);
	pidckz_param_set(&pid_imu_tmp, 1000, 120, 12.0f, 0.1f, 240.0f);      
}

void imu_temp_keep(void)
{
  imu.temp_ref = 5000;
//	imu.temp = MPU_Get_Temperature();	                   //读取温度
  pidckz_calc(&pid_imu_tmp, imu.temp, imu.temp_ref);
	TIM1->CCR1 = constrain(pid_imu_tmp.out, 0, +1000); 
}

void mpu_get_data(void)
{
	MPU_Get_Gyroscope(&imu.gx,&imu.gy,&imu.gz);       	//读取角速度		
	mpu_dmp_get_data(&imu.roll,&imu.pit,&imu.yaw); 	    //读取欧拉角
	imu.temp = MPU_Get_Temperature();	                  //读取温度
}

void imu_task(void)//必要时两个可以拆开放在不同的时间戳
{
  mpu_get_data();	
  imu_temp_keep();
}

