#ifndef _IMU_TASK_H
#define _IMU_TASK_H

#include "bsp_sys.h"

void imu_param_init(void);
void imu_temp_keep(void);
void mpu_get_data(void);
void imu_task(void);

#endif
