#include "bsp_init.h"
#include "motor_drv.h"
#include "ti_msp_dl_config.h"
#include <stdint.h>

static volatile uint32_t g_tick_ms = 0;
static volatile uint8_t g_ctrl_tick_flag = 0;
static volatile uint8_t g_ctrl_tick_count = 0;

void BSP_SysTick_Init(void)
{
    DL_SYSTICK_disable();
    DL_SYSTICK_init(32000);  /* 32 MHz / 32000 = 1 ms */
    DL_SYSTICK_enableInterrupt();
    DL_SYSTICK_enable();

    /* Control loop uses SysTick, so keep TIMER_SYS disabled. */
    NVIC_DisableIRQ(TIMER_SYS_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(TIMER_SYS_INST_INT_IRQN);
    DL_Timer_disableInterrupt(TIMER_SYS_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
    DL_Timer_stopCounter(TIMER_SYS_INST);
}

void BSP_Init(void)
{
    MotorDrv_Init();
    BSP_SysTick_Init();
}

uint32_t BSP_GetTick(void)
{
    return g_tick_ms;
}

uint8_t BSP_ControlTick(void)
{
    if (g_ctrl_tick_flag) {
        g_ctrl_tick_flag = 0;
        return 1;
    }
    return 0;
}

void SysTick_Handler(void)
{
    g_tick_ms++;

    g_ctrl_tick_count++;
    if (g_ctrl_tick_count >= 20) {
        g_ctrl_tick_count = 0;
        g_ctrl_tick_flag = 1;
    }
}
