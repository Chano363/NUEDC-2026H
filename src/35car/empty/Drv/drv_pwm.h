#ifndef _DRV_PWM_H
#define _DRV_PWM_H

#include <stdint.h>
#include "ti_msp_dl_config.h"

void Drv_PWM_Init(void);
void Drv_PWM_SetDuty(GPTIMER_Regs *timer_inst, uint8_t cc_idx, uint16_t duty);
void Drv_PWM_Start(GPTIMER_Regs *timer_inst);
void Drv_PWM_Stop(GPTIMER_Regs *timer_inst);

#endif
