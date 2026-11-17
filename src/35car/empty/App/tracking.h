#ifndef _TRACKING_H
#define _TRACKING_H

#include <stdint.h>
#include <stdbool.h>

/* ══════════════════════════════════════════════════════════════
   循迹策略输出（供 speed_ctrl 使用）
   ══════════════════════════════════════════════════════════════ */

typedef struct {
    int16_t target_left;       /* 左轮目标值 */
    int16_t target_right;      /* 右轮目标值 */
    bool running;              /* 是否在运行 */
    bool finished;             /* 是否完成 */
} Tracking_OutputDef;

/* ══════════════════════════════════════════════════════════════
   API
   ══════════════════════════════════════════════════════════════ */

void Tracking_Init(void);
void Tracking_Update(void);
void Tracking_Start(uint8_t laps);
void Tracking_Stop(void);

Tracking_OutputDef Tracking_GetOutput(void);
int16_t Tracking_GetDeviation(void);
uint8_t Tracking_GetDigitalStatus(void);
uint8_t Tracking_GetState(void);  /* 获取状态机当前状态 */

/* ══════════════════════════════════════════════════════════════
   蓝牙调参接口
   ══════════════════════════════════════════════════════════════ */

/* 位置环 PID 参数设置（实时生效） */
void Tracking_SetKp(float kp);
void Tracking_SetKi(float ki);
void Tracking_SetKd(float kd);

/* 运动参数设置 */
void Tracking_SetBaseSpeed(int16_t speed);
void Tracking_SetSearchSpeed(int16_t speed);

/* 参数查询 */
float Tracking_GetKp(void);
float Tracking_GetKi(void);
float Tracking_GetKd(void);
int16_t Tracking_GetBaseSpeed(void);
int16_t Tracking_GetSearchSpeed(void);

#endif