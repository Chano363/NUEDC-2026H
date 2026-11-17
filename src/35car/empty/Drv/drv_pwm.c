#include "drv_pwm.h"
#include "ti_msp_dl_config.h"

#define PWM_PERIOD 200u

static bool Drv_PWM_IsTimerA(GPTIMER_Regs *timer_inst)
{
    return timer_inst == PWM_MOTOR_L_INST;
}

void Drv_PWM_Init(void)
{
    DL_TimerA_stopCounter(PWM_MOTOR_L_INST);
    DL_TimerG_stopCounter(PWM_MOTOR_R_INST);
}

void Drv_PWM_SetDuty(GPTIMER_Regs *timer_inst, uint8_t cc_idx, uint16_t duty)
{
    if (duty > PWM_PERIOD) duty = PWM_PERIOD;

    uint16_t cc = PWM_PERIOD - duty;
    if (Drv_PWM_IsTimerA(timer_inst)) {
        DL_TimerA_setCaptureCompareValue(PWM_MOTOR_L_INST, cc, cc_idx);
    } else {
        DL_TimerG_setCaptureCompareValue(PWM_MOTOR_R_INST, cc, cc_idx);
    }
}

void Drv_PWM_Start(GPTIMER_Regs *timer_inst)
{
    if (Drv_PWM_IsTimerA(timer_inst)) {
        DL_TimerA_startCounter(PWM_MOTOR_L_INST);
    } else {
        DL_TimerG_startCounter(PWM_MOTOR_R_INST);
    }
}

void Drv_PWM_Stop(GPTIMER_Regs *timer_inst)
{
    if (Drv_PWM_IsTimerA(timer_inst)) {
        DL_TimerA_stopCounter(PWM_MOTOR_L_INST);
    } else {
        DL_TimerG_stopCounter(PWM_MOTOR_R_INST);
    }
}
