#include <stddef.h>
#include "pid.h"
#include "../Bsp/motor_drv.h"
PID_HandleTypeDef PID_Create(float kp, float ki, float kd, PID_ModeTypeDef mode)
{
    PID_HandleTypeDef pid;
    pid.Kp = kp;
    pid.Ki = ki;
    pid.Kd = kd;
    pid.setpoint = 0.0f;
    pid.process_value = 0.0f;
    pid.output = 0.0f;
    pid.last_output = 0.0f;
    pid.integral = 0.0f;
    pid.last_error = 0.0f;
    pid.error_sum = 0.0f;
    pid.output_limit_high = (float)MOTOR_PWM_MAX;
    pid.output_limit_low = -(float)MOTOR_PWM_MAX;
    pid.mode = mode;
    return pid;
}

void PID_SetOutputLimit(PID_HandleTypeDef *pid, float limit)
{
    if (pid == NULL) return;
    pid->output_limit_high = limit;
    pid->output_limit_low = -limit;
}

void PID_SetSetpoint(PID_HandleTypeDef *pid, float setpoint)
{
    if (pid == NULL) return;
    pid->setpoint = setpoint;
}

float PID_Compute(PID_HandleTypeDef *pid, float process_value)
{
    if (pid == NULL) return 0.0f;
    
    float error = pid->setpoint - process_value;
    float output = 0.0f;
    
    if (pid->mode == PID_MODE_POSITION) {
        pid->integral += error;
        
        if (pid->integral > pid->output_limit_high) {
            pid->integral = pid->output_limit_high;
        } else if (pid->integral < pid->output_limit_low) {
            pid->integral = pid->output_limit_low;
        }
        
        output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * (error - pid->last_error);
    } else {
        output = pid->Kp * (error - pid->last_error) + pid->Ki * error + pid->Kd * (error - 2 * pid->last_error + pid->error_sum);
        pid->error_sum = pid->last_error;
        pid->output += output;
        output = pid->output;
    }
    
    if (output > pid->output_limit_high) {
        output = pid->output_limit_high;
    } else if (output < pid->output_limit_low) {
        output = pid->output_limit_low;
    }
    
    pid->last_error = error;
    pid->output = output;
    pid->process_value = process_value;
    
    return output;
}

void PID_Reset(PID_HandleTypeDef *pid)
{
    if (pid == NULL) return;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->error_sum = 0.0f;
    pid->output = 0.0f;
    pid->last_output = 0.0f;
}