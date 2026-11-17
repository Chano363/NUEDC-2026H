#include "drv_timer.h"
#include "ti_msp_dl_config.h"
#include <stddef.h>

static TIMER_Callback g_callback = NULL;

void Drv_TIMER_Init(GPTIMER_Regs *timer_inst, uint32_t period_ms)
{
    (void)period_ms;
    DL_Timer_startCounter(timer_inst);
}

void Drv_TIMER_Start(GPTIMER_Regs *timer_inst)
{
    DL_Timer_startCounter(timer_inst);
}

void Drv_TIMER_Stop(GPTIMER_Regs *timer_inst)
{
    DL_Timer_stopCounter(timer_inst);
}

void Drv_TIMER_EnableInterrupt(GPTIMER_Regs *timer_inst)
{
    DL_Timer_enableInterrupt(timer_inst, DL_TIMER_INTERRUPT_ZERO_EVENT);
}

void Drv_TIMER_DisableInterrupt(GPTIMER_Regs *timer_inst)
{
    DL_Timer_disableInterrupt(timer_inst, DL_TIMER_INTERRUPT_ZERO_EVENT);
}

void Drv_TIMER_RegisterCallback(TIMER_Callback callback)
{
    g_callback = callback;
}

void TIMER_MAIN_ISR(void)
{
    if (g_callback != NULL) {
        g_callback();
    }
}
