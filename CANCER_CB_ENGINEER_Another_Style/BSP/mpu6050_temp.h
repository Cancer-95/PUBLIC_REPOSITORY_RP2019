#ifndef __MPU6050_TEMP_H__
#define __MPU6050_TEMP_H__

#include "system.h"

void mpu6050_temp_init(void);

void imu_param_init(void);
void imu_temp_keep(void);
void mpu_get_data(void);
void imu_task(void);

void mpu6050_temp_PID(void);

extern short Temperature_Value;
#endif
