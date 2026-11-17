#include "aiming_ctrl.h"
#include "gyro_fusion.h"
#include "../Lib/math_utils.h"

static AimingCtrl_DataDef aiming_data;

void AimingCtrl_Init(void)
{
    aiming_data.car_angle = 0.0f;
    aiming_data.target_angle = 0.0f;
    aiming_data.pan_angle = 0.0f;
    aiming_data.tilt_angle = 0.0f;
    aiming_data.is_locked = false;
}

void AimingCtrl_Update(void)
{
    aiming_data.car_angle = GyroFusion_GetAngle();
    
    if (aiming_data.is_locked) {
        aiming_data.pan_angle = WrapAngle180(aiming_data.target_angle - aiming_data.car_angle);
    }
}

void AimingCtrl_SetCarAngle(float angle)
{
    aiming_data.car_angle = angle;
}

void AimingCtrl_LockTarget(float target_x, float target_y)
{
    (void)target_x;
    (void)target_y;
    aiming_data.is_locked = true;
}

AimingCtrl_DataDef AimingCtrl_GetStatus(void)
{
    return aiming_data;
}