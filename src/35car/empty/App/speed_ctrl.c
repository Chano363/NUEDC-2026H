#include "speed_ctrl.h"
#include "../Bsp/motor_drv.h"
#include "../Bsp/encoder.h"
#include "../Lib/pid.h"
#include "../Lib/math_utils.h"
#include <stddef.h>

#define SPEED_CTRL_ENCODER_DISABLED 0U

/* ══════════════════════════════════════════════════════════════
   速度环 PID 参数（增量式）
   ══════════════════════════════════════════════════════════════ */

static float g_speed_kp = 0.8f;     /* 比例系数：310电机响应中等 */
static float g_speed_ki = 0.01f;    /* 积分系数：消除稳态误差 */
static float g_speed_kd = 0.3f;     /* 微分系数：抑制超调 */

/* ══════════════════════════════════════════════════════════════
   运行状态
   ══════════════════════════════════════════════════════════════ */

static PID_HandleTypeDef g_left_pid;
static PID_HandleTypeDef g_right_pid;

/* ══════════════════════════════════════════════════════════════
   初始化
   ══════════════════════════════════════════════════════════════ */

void SpeedCtrl_Init(void)
{
    /* 使用增量式PID实现速度闭环控制 */
    g_left_pid = PID_Create(g_speed_kp, g_speed_ki, g_speed_kd, PID_MODE_INCREMENTAL);
    g_right_pid = PID_Create(g_speed_kp, g_speed_ki, g_speed_kd, PID_MODE_INCREMENTAL);

    MotorDrv_Stop();  /* 确保启动时电机停止 */
}

/* ══════════════════════════════════════════════════════════════
   20ms 执行周期（闭环PID）
   ══════════════════════════════════════════════════════════════ */

void SpeedCtrl_Update(int16_t target_left, int16_t target_right)
{
    /* 更新编码器速度 */

    /* 目标为0时停车 */
    if (target_left == 0 && target_right == 0) {
        MotorDrv_SetSpeed(0, 0);
        return;
    }

#if SPEED_CTRL_ENCODER_DISABLED
    /* 开环模式：目标速度直接映射为 PWM，跳过速度环 PID */
    int16_t left_pwm_int  = LimitIntValue(target_left,  -MOTOR_PWM_MAX, MOTOR_PWM_MAX);
    int16_t right_pwm_int = LimitIntValue(target_right, -MOTOR_PWM_MAX, MOTOR_PWM_MAX);
    MotorDrv_SetSpeed(left_pwm_int, right_pwm_int);
#else
    /* 闭环模式：编码器反馈 + 增量式 PID */
    Encoder_UpdateSpeed();
    float left_speed = (float)Encoder_GetLeftSpeed();
    float right_speed = (float)Encoder_GetRightSpeed();

    PID_SetSetpoint(&g_left_pid, (float)target_left);
    PID_SetSetpoint(&g_right_pid, (float)target_right);

    float left_pwm  = PID_Compute(&g_left_pid, left_speed);
    float right_pwm = PID_Compute(&g_right_pid, right_speed);

    int16_t left_pwm_int  = LimitIntValue((int16_t)left_pwm,  -MOTOR_PWM_MAX, MOTOR_PWM_MAX);
    int16_t right_pwm_int = LimitIntValue((int16_t)right_pwm, -MOTOR_PWM_MAX, MOTOR_PWM_MAX);

    MotorDrv_SetSpeed(left_pwm_int, right_pwm_int);
#endif
}

/* ══════════════════════════════════════════════════════════════
   重置PID（丢线切换时调用）
   ══════════════════════════════════════════════════════════════ */

void SpeedCtrl_ResetPID(void)
{
    PID_Reset(&g_left_pid);
    PID_Reset(&g_right_pid);
}

/* ══════════════════════════════════════════════════════════════
   紧急停止
   ══════════════════════════════════════════════════════════════ */

void SpeedCtrl_EmergencyStop(void)
{
    MotorDrv_Stop();
}
