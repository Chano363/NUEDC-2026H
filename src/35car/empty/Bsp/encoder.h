#ifndef _ENCODER_H
#define _ENCODER_H

#include <stdint.h>
#include "ti_msp_dl_config.h"

/* 编码器引脚由 SysConfig ENC 组生成:
 *   ENC_PORT, ENC_LA_PIN, ENC_LB_PIN, ENC_RA_PIN, ENC_RB_PIN
 */

void Encoder_Init(void);
void Encoder_ResetLeftCount(void);
void Encoder_ResetRightCount(void);
int32_t Encoder_GetLeftCount(void);
int32_t Encoder_GetRightCount(void);
void Encoder_ProcessInterrupt(void);

/* ── 速度计算 ── */
/* 每 20ms 控制周期调用一次，计算增量速度 */
void Encoder_UpdateSpeed(void);

/* 返回最近一次速度计算结果（counts/20ms） */
int32_t Encoder_GetLeftSpeed(void);
int32_t Encoder_GetRightSpeed(void);

#endif
