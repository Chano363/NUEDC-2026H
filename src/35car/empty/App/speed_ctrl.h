#ifndef _SPEED_CTRL_H
#define _SPEED_CTRL_H

#include <stdint.h>

/* ══════════════════════════════════════════════════════════════
   API 接口
   ══════════════════════════════════════════════════════════════ */

void SpeedCtrl_Init(void);
void SpeedCtrl_Update(int16_t target_left, int16_t target_right);
void SpeedCtrl_ResetPID(void);
void SpeedCtrl_EmergencyStop(void);

#endif