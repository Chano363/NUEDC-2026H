#include "odometry.h"
#include "../Bsp/encoder.h"
#include "gyro_fusion.h"
#include "../Bsp/grayscale.h"
#include "../Bsp/bsp_init.h"
#include <math.h>

static Odometry_DataDef g_odom;
static int32_t g_last_left_count = 0;
static int32_t g_last_right_count = 0;
static uint32_t g_last_update_time = 0;

void Odometry_Init(void)
{
    g_odom.x = 0.0f;
    g_odom.y = 0.0f;
    g_odom.heading = 0.0f;
    g_odom.left_speed = 0.0f;
    g_odom.right_speed = 0.0f;
    g_odom.current_edge = 0;
    g_odom.distance_on_edge = 0;
    g_odom.lap_count = 0;
    g_odom.corner_detected = false;
    
    g_last_left_count = Encoder_GetLeftCount();
    g_last_right_count = Encoder_GetRightCount();
    g_last_update_time = BSP_GetTick();
}

void Odometry_Update(void)
{
    uint32_t now = BSP_GetTick();
    uint32_t dt = now - g_last_update_time;
    
    if (dt < 10) {
        return;
    }
    
    float dt_sec = (float)dt / 1000.0f;
    g_last_update_time = now;
    
    int32_t left_count = Encoder_GetLeftCount();
    int32_t right_count = Encoder_GetRightCount();
    
    int32_t delta_left = left_count - g_last_left_count;
    int32_t delta_right = right_count - g_last_right_count;
    
    g_last_left_count = left_count;
    g_last_right_count = right_count;
    
    float delta_left_mm = (float)delta_left / ENCODER_COUNTS_PER_MM;
    float delta_right_mm = (float)delta_right / ENCODER_COUNTS_PER_MM;
    
    g_odom.left_speed = delta_left_mm / dt_sec;
    g_odom.right_speed = delta_right_mm / dt_sec;
    
    float delta_dist = (delta_left_mm + delta_right_mm) / 2.0f;
    float delta_angle = (delta_right_mm - delta_left_mm) / WHEEL_BASE;
    
    g_odom.heading = GyroFusion_GetHeading();
    float heading_rad = g_odom.heading * 0.0174533f;
    
    g_odom.x += delta_dist * cosf(heading_rad);
    g_odom.y += delta_dist * sinf(heading_rad);
    
    g_odom.distance_on_edge += (uint16_t)delta_dist;
    
    uint8_t grayscale_status = Grayscale_GetDigitalStatus();
    if (grayscale_status == 0xFF) {
        g_odom.corner_detected = true;
        g_odom.current_edge = (g_odom.current_edge + 1) % 4;
        g_odom.distance_on_edge = 0;
        
        if (g_odom.current_edge == 0) {
            g_odom.lap_count++;
        }
    } else {
        g_odom.corner_detected = false;
    }
}

float Odometry_GetX(void)
{
    return g_odom.x;
}

float Odometry_GetY(void)
{
    return g_odom.y;
}

float Odometry_GetHeading(void)
{
    return g_odom.heading;
}

uint8_t Odometry_GetCurrentEdge(void)
{
    return g_odom.current_edge;
}

uint8_t Odometry_GetLapCount(void)
{
    return g_odom.lap_count;
}

bool Odometry_IsCornerDetected(void)
{
    return g_odom.corner_detected;
}

Odometry_DataDef Odometry_GetData(void)
{
    return g_odom;
}

void Odometry_ResetPosition(void)
{
    g_odom.x = 0.0f;
    g_odom.y = 0.0f;
    g_odom.heading = 0.0f;
    g_odom.current_edge = 0;
    g_odom.distance_on_edge = 0;
    g_odom.lap_count = 0;
    
    Encoder_ResetLeftCount();
    Encoder_ResetRightCount();
    g_last_left_count = 0;
    g_last_right_count = 0;
}