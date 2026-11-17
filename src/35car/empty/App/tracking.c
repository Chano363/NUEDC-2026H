/* ══════════════════════════════════════════════════════════════
   tracking.c — 简化版循迹策略
   参照开源 Car_OPEN 方案：只有 base_speed + PD转向 + 丢线保护
   ══════════════════════════════════════════════════════════════ */
#include "tracking.h"
#include "../Bsp/grayscale.h"
#include "speed_ctrl.h"
#include <stdlib.h>
#include "../Lib/pid.h"
#include "../Bsp/motor_drv.h"

/* ══════════════════════════════════════════════════════════════
   位置环 PID 参数（位置式）
   ══════════════════════════════════════════════════════════════ */

static float g_pos_kp = 0.5f;  /* 比例系数 */
static float g_pos_ki = 0.01f; /* 积分系数 */
static float g_pos_kd = 0.9f;  /* 微分系数 */

/* ══════════════════════════════════════════════════════════════
   循迹运动参数
   ══════════════════════════════════════════════════════════════ */

static int16_t g_base_speed = 100;  /* 基准速度（居中直行） */
static int16_t g_speed_min = 25;    /* 最低速度 */
static int16_t g_search_speed = 80; /* 丢线原地旋转速度 */

#define LOST_STRAIGHT_TICKS 10 /* 丢线后直线行驶的周期数 */

/* ── 内部状态 ── */
typedef enum
{
    STATE_NORMAL,        /* 正常循迹 */
    STATE_LOST_STRAIGHT, /* 丢线后直线行驶 */
    STATE_LOST_SEARCH    /* 丢线后转弯搜索 */
} TrackingState;

static Tracking_OutputDef g_output;
static TrackingState g_state = STATE_NORMAL;
static uint8_t g_lost_straight_count = 0; /* 直线行驶计数器 */
static int16_t g_deviation = 0;
static int16_t g_last_seen_deviation = 0;
static int16_t g_recent_deviation = 0;
static uint8_t g_digital_status = 0;
static int8_t g_last_turn_dir = 0;
static uint8_t g_line_status = 0xFF;
static PID_HandleTypeDef g_pos_pid; /* 位置环 PID（全量式）*/                                                                                                                                                                                         

/* ══════════════════════════════════════════════════════════════
   初始化 / 启停
   ══════════════════════════════════════════════════════════════ */

void Tracking_Init(void)
{
    g_output = (Tracking_OutputDef){0};
    g_output.running = false;

    /* 创建位置环 PID（位置式） */
    g_pos_pid = PID_Create(g_pos_kp, g_pos_ki, g_pos_kd, PID_MODE_POSITION);
}

void Tracking_Start(uint8_t laps)
{
    (void)laps;
    g_output.running = true;
    g_output.finished = false;
    g_state = STATE_NORMAL; /* 重置状态机 */
    g_lost_straight_count = 0;
    g_deviation = 0;
    g_last_seen_deviation = 0;
    g_recent_deviation = 0;
    g_last_turn_dir = 0;
    g_line_status = 0xFF;
    g_output.target_left = 0;
    g_output.target_right = 0;
    PID_Reset(&g_pos_pid);
    SpeedCtrl_ResetPID();
}

void Tracking_Stop(void)
{
    g_output.running = false;
    g_output.target_left = 0;
    g_output.target_right = 0;
}

static int8_t get_deviation_turn_dir(int16_t dev)
{
    // 不可改动
    if (dev < -3)
        return 1; // 小车偏右
    if (dev > 3)
        return -1; // 小车偏左
    return 0;
}

/* ══════════════════════════════════════════════════════════════
   20ms 主周期
   ══════════════════════════════════════════════════════════════ */

void Tracking_Update(void)
{
    /* ── 传感器读取 ── */
    Grayscale_DataDef gs;
    Grayscale_Update(&gs);
    g_digital_status = gs.digital_status;
    g_deviation = gs.deviation;
    g_line_status = gs.line_status;
    if (!g_output.running || g_output.finished)
    {
        g_output.target_left = 0;
        g_output.target_right = 0;
        return;
    }

    /* ── 速度：偏差大→速度低 ── */
    int16_t abs_dev = abs(g_deviation);
    int16_t speed = g_base_speed - abs_dev;
    if (speed < g_speed_min)
        speed = g_speed_min;

    /* ── 转向输出 ── */
    int16_t yaw = 0;

    /* ── 状态机：丢线处理 ── */
    switch (g_state)
    {
    case STATE_NORMAL:
        /* 正常循迹 */
        if (g_line_status == GRAYSCALE_LINE_LOST)
        {
            /* 检测到丢线 → 进入直线行驶阶段，重置位置PID+速度PID避免积分滞后 */
            g_state = STATE_LOST_STRAIGHT;
            g_lost_straight_count = LOST_STRAIGHT_TICKS;
            PID_Reset(&g_pos_pid); /* 位置环清零（防积分饱和）*/
            SpeedCtrl_ResetPID();
            yaw = 0;              /* 直线行驶，不转向 */
            speed = g_base_speed; /* 保持原速度 */
        }
        else
        {
            /* 在线：位置环 PID（位置式）*/
            /* 积分分离：|偏差|>30 时禁用积分，防 windup */
            if (abs(g_deviation) > 30)
            {
                g_pos_pid.Ki = 0.0f;
            }
            else
            {
                g_pos_pid.Ki = g_pos_ki;
            }
            PID_SetSetpoint(&g_pos_pid, 0.0f);

            /* PID 计算输出绝对值，符号由 get_deviation_turn_dir 决定 */
            int16_t yaw_abs = abs((int16_t)PID_Compute(&g_pos_pid, (float)g_deviation));
            int8_t turn_dir = get_deviation_turn_dir(g_deviation);
            yaw = turn_dir * yaw_abs;
            speed = g_base_speed;

            /* 记录历史偏差（用于丢线后搜索方向）*/
            g_last_seen_deviation = g_deviation;
            g_recent_deviation = (int16_t)((g_recent_deviation * 3 + g_deviation) / 4);

            /* 只在偏差较大时更新 turn_dir，避免弯道修正时频繁翻转 */
            if (abs(g_deviation) > 15)
            {
                g_last_turn_dir = turn_dir;
            }
        }
        break;

    case STATE_LOST_STRAIGHT:
        /* 丢线后直线行驶阶段 */
        yaw = 0;              /* 直线行驶，不转向 */
        speed = g_base_speed; /* 保持原速度 */

        g_lost_straight_count--;
        if (g_lost_straight_count == 0)
        {
            /* 直线阶段结束 → 进入转弯搜索阶段，重置PID */
            g_state = STATE_LOST_SEARCH;
            SpeedCtrl_ResetPID();
        }

        /* 如果在直线阶段重新找到线，立即回到正常状态 */
        if (g_line_status == GRAYSCALE_LINE_ON)
        {
            g_state = STATE_NORMAL;
            PID_Reset(&g_pos_pid);
            SpeedCtrl_ResetPID();
        }
        break;

    case STATE_LOST_SEARCH:
        /* 丢线后原地旋转搜索阶段（一正一反，围绕中心旋转） */
        /* 只有中间两个传感器(S4/S5)检测到黑线才停止旋转 */
        if (g_digital_status & 0x18)
        { /* bit3=S4, bit4=S5 00011000 */
            /* 重新找到线 → 回到正常循迹，重置位置PID+速度PID */
            g_state = STATE_NORMAL;
            PID_Reset(&g_pos_pid);
            SpeedCtrl_ResetPID();
        }
        else
        {
            /* ── 中心原地旋转：直接输出目标值 ── */
            /* 旋转方向由丢线前最后的 turn_dir 决定 */
            int8_t lost_dir = get_deviation_turn_dir(g_recent_deviation);
            if (lost_dir == 0)
                lost_dir = get_deviation_turn_dir(g_last_seen_deviation);
            if (lost_dir == 0)
                lost_dir = g_last_turn_dir;
            /* 左轮正转lost_dir方向，右轮反转 → 原地旋转 */
            g_output.target_left = lost_dir * g_search_speed;
            g_output.target_right = -(lost_dir * g_search_speed);
        }
        break;
    }

    /* ── 差速（STATE_LOST_SEARCH 已直接输出，无需聚合）── */
    if (g_state != STATE_LOST_SEARCH)
    {
        g_output.target_left = speed + yaw;
        g_output.target_right = speed - yaw;
    }
}

/* ══════════════════════════════════════════════════════════════
   数据查询
   ══════════════════════════════════════════════════════════════ */

Tracking_OutputDef Tracking_GetOutput(void) { return g_output; }
int16_t Tracking_GetDeviation(void) { return g_deviation; }
uint8_t Tracking_GetDigitalStatus(void) { return g_digital_status; }

uint8_t Tracking_GetState(void) { return (uint8_t)g_state; }

/* ══════════════════════════════════════════════════════════════
   蓝牙调参接口
   ══════════════════════════════════════════════════════════════ */

void Tracking_SetKp(float kp) { g_pos_kp = kp; g_pos_pid.Kp = kp; }
void Tracking_SetKi(float ki) { g_pos_ki = ki; g_pos_pid.Ki = ki; }
void Tracking_SetKd(float kd) { g_pos_kd = kd; g_pos_pid.Kd = kd; }

void Tracking_SetBaseSpeed(int16_t speed)   { g_base_speed = speed; }
void Tracking_SetSearchSpeed(int16_t speed)  { g_search_speed = speed; }

float    Tracking_GetKp(void)          { return g_pos_kp; }
float    Tracking_GetKi(void)          { return g_pos_ki; }
float    Tracking_GetKd(void)          { return g_pos_kd; }
int16_t  Tracking_GetBaseSpeed(void)    { return g_base_speed; }
int16_t  Tracking_GetSearchSpeed(void)  { return g_search_speed; }
