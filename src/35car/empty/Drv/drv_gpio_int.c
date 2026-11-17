#include "drv_gpio_int.h"
#include "ti_msp_dl_config.h"
#include "../Bsp/encoder.h"
#include <stddef.h>

static GPIO_INT_Callback g_callback = NULL;

void Drv_GPIO_INT_Init(void)
{
    DL_GPIO_clearInterruptStatus(
        ENC_PORT, ENC_LA_PIN | ENC_LB_PIN | ENC_RA_PIN | ENC_RB_PIN);
    DL_GPIO_enableInterrupt(ENC_PORT, ENC_LA_PIN);
    DL_GPIO_enableInterrupt(ENC_PORT, ENC_LB_PIN);
    DL_GPIO_enableInterrupt(ENC_PORT, ENC_RA_PIN);
    DL_GPIO_enableInterrupt(ENC_PORT, ENC_RB_PIN);

    NVIC_ClearPendingIRQ(ENC_INT_IRQN);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

void Drv_GPIO_INT_Enable(GPIO_Regs *port, uint32_t pin)
{
    DL_GPIO_enableInterrupt(port, pin);
}

void Drv_GPIO_INT_Disable(GPIO_Regs *port, uint32_t pin)
{
    DL_GPIO_disableInterrupt(port, pin);
}

void Drv_GPIO_INT_Clear(GPIO_Regs *port, uint32_t pin)
{
    DL_GPIO_clearInterruptStatus(port, pin);
}

void Drv_GPIO_INT_RegisterCallback(GPIO_INT_Callback callback)
{
    g_callback = callback;
}

/* ── 硬件中断向量: GROUP1 = GPIOA 中断
   SysConfig 把 ENC/KEY 引脚挂在 GPIOA_INT_IRQn (GROUP1)
   这是真正的 IRQ Handler，不是普通函数。
   调用回调链: GROUP1_IRQHandler → ENC_ISR → Encoder_ProcessInterrupt */
void GROUP1_IRQHandler(void)
{
    if (g_callback != NULL) {
        g_callback();
    }
}
