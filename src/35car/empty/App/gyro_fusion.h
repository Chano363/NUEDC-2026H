#ifndef _GYRO_FUSION_H
#define _GYRO_FUSION_H

#include <stdint.h>

typedef struct {
    float angle_deg;
    float dps;
    float fused_angle;
    float heading;
} GyroFusion_DataDef;

void GyroFusion_Init(void);
void GyroFusion_Update(void);
float GyroFusion_GetAngle(void);
float GyroFusion_GetHeading(void);
GyroFusion_DataDef GyroFusion_GetData(void);

#endif