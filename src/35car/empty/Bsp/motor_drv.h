#ifndef _MOTOR_DRV_H
#define _MOTOR_DRV_H

#include <stdint.h>
#include <stdbool.h>

#define MOTOR_PWM_MAX  50

void MotorDrv_Init(void);
void MotorDrv_SetSpeed(int16_t left_speed, int16_t right_speed);
void MotorDrv_Stop(void);

#endif
