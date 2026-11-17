#include "gyro_fusion.h"
#include "../Bsp/gyro_uart.h"
#include "../Lib/math_utils.h"

static GyroFusion_DataDef gyro_data;
static float angle_offset = 0.0f;

void GyroFusion_Init(void)
{
    gyro_data.angle_deg = 0.0f;
    gyro_data.dps = 0.0f;
    gyro_data.fused_angle = 0.0f;
    angle_offset = 0.0f;
}

void GyroFusion_Update(void)
{
    GyroData_t raw_data;
    if (Gyro_GetData(&raw_data)) {
        gyro_data.angle_deg = raw_data.angle_deg;
        gyro_data.dps = raw_data.dps;
        gyro_data.fused_angle = WrapAngle180(gyro_data.angle_deg - angle_offset);
    }
}

float GyroFusion_GetAngle(void)
{
    return gyro_data.fused_angle;
}

float GyroFusion_GetHeading(void)
{
    return gyro_data.fused_angle;
}

GyroFusion_DataDef GyroFusion_GetData(void)
{
    return gyro_data;
}
