#ifndef _PID_H
#define _PID_H

#include <stdint.h>

typedef enum {
    PID_MODE_POSITION = 0,
    PID_MODE_INCREMENTAL
} PID_ModeTypeDef;

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float setpoint;
    float process_value;
    float output;
    float last_output;
    float integral;
    float last_error;
    float error_sum;
    float output_limit_high;
    float output_limit_low;
    PID_ModeTypeDef mode;
} PID_HandleTypeDef;

PID_HandleTypeDef PID_Create(float kp, float ki, float kd, PID_ModeTypeDef mode);
void PID_SetOutputLimit(PID_HandleTypeDef *pid, float limit);
void PID_SetSetpoint(PID_HandleTypeDef *pid, float setpoint);
float PID_Compute(PID_HandleTypeDef *pid, float process_value);
void PID_Reset(PID_HandleTypeDef *pid);

#endif