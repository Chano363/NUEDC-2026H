#ifndef _DRV_TIMER_H
#define _DRV_TIMER_H

#include <stdint.h>
#include "ti_msp_dl_config.h"

typedef void (*TIMER_Callback)(void);

void Drv_TIMER_Init(GPTIMER_Regs *timer_inst, uint32_t period_ms);
void Drv_TIMER_Start(GPTIMER_Regs *timer_inst);
void Drv_TIMER_Stop(GPTIMER_Regs *timer_inst);
void Drv_TIMER_EnableInterrupt(GPTIMER_Regs *timer_inst);
void Drv_TIMER_DisableInterrupt(GPTIMER_Regs *timer_inst);
void Drv_TIMER_RegisterCallback(TIMER_Callback callback);

#endif
