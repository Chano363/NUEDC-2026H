#ifndef _AIMING_CTRL_H
#define _AIMING_CTRL_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float car_angle;
    float target_angle;
    float pan_angle;
    float tilt_angle;
    bool is_locked;
} AimingCtrl_DataDef;

void AimingCtrl_Init(void);
void AimingCtrl_Update(void);
void AimingCtrl_SetCarAngle(float angle);
void AimingCtrl_LockTarget(float target_x, float target_y);
AimingCtrl_DataDef AimingCtrl_GetStatus(void);

#endif