#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "system.h"
void MPU_tempPID_init(void);

void imu_param_init(void);
void imu_temp_keep(void);
void mpu_get_data(void);
void imu_task(void);

void Tempeture_PID(void);

extern short Temperature_Value;
#endif
