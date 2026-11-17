/* Encoder GPIO and interrupt diagnostic. Turn both wheels by hand. */
#include <stdio.h>
#include "ti_msp_dl_config.h"
#include "Bsp/bsp_init.h"
#include "Bsp/encoder.h"
#include "Bsp/motor_drv.h"
#include "Drv/drv_oled.h"

static void delay_ms(uint32_t ms)
{
    uint32_t start = BSP_GetTick();
    while ((BSP_GetTick() - start) < ms) {
    }
}

int main(void)
{
    char line[18];

    SYSCFG_DL_init();
    MotorDrv_Init();
    BSP_SysTick_Init();
    Encoder_Init();
    Drv_OLED_Init();

    Encoder_ResetLeftCount();
    Encoder_ResetRightCount();

    while (1) {
        bool la = DL_GPIO_readPins(ENC_PORT, ENC_LA_PIN) != 0;
        bool lb = DL_GPIO_readPins(ENC_PORT, ENC_LB_PIN) != 0;
        bool ra = DL_GPIO_readPins(ENC_PORT, ENC_RA_PIN) != 0;
        bool rb = DL_GPIO_readPins(ENC_PORT, ENC_RB_PIN) != 0;

        Drv_OLED_Clear();
        snprintf(line, sizeof(line), "LA:%d LB:%d", la, lb);
        Drv_OLED_ShowString(0, 0, line);
        snprintf(line, sizeof(line), "RA:%d RB:%d", ra, rb);
        Drv_OLED_ShowString(2, 0, line);
        snprintf(line, sizeof(line), "LC:%ld", (long)Encoder_GetLeftCount());
        Drv_OLED_ShowString(4, 0, line);
        snprintf(line, sizeof(line), "RC:%ld", (long)Encoder_GetRightCount());
        Drv_OLED_ShowString(6, 0, line);
        Drv_OLED_Display();

        delay_ms(50);
    }
}
