#ifndef _ODOMETRY_H
#define _ODOMETRY_H

#include <stdint.h>
#include <stdbool.h>

/* TODO: 根据实际硬件校准以下参数 */
#define SQUARE_EDGE_LENGTH 1000   /* mm, 赛道边长 */
#define WHEEL_BASE 200            /* mm, 两轮间距 */
#define ENCODER_COUNTS_PER_MM 10  /* 编码器脉冲数/毫米 */

typedef struct {
    float x;
    float y;
    float heading;
    float left_speed;
    float right_speed;
    uint8_t current_edge;
    uint16_t distance_on_edge;
    uint8_t lap_count;
    bool corner_detected;
} Odometry_DataDef;

void Odometry_Init(void);
void Odometry_Update(void);
float Odometry_GetX(void);
float Odometry_GetY(void);
float Odometry_GetHeading(void);
uint8_t Odometry_GetCurrentEdge(void);
uint8_t Odometry_GetLapCount(void);
bool Odometry_IsCornerDetected(void);
Odometry_DataDef Odometry_GetData(void);
void Odometry_ResetPosition(void);

#endif