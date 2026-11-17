#include <stdio.h>
#include "oled_app.h"
#include "tracking.h"
#include "Bsp/encoder.h"
#include "Drv/drv_oled.h"
#include "Bsp/bsp_init.h"

#define OLED_FRAME_INTERVAL_MS 200U

static uint32_t g_last_frame_tick;

static const char *const g_state_name[] = {
    "NOR",
    "LST",
    "LSR"
};

static void grayscale_to_text(uint8_t digital, char *text)
{
    for (uint8_t i = 0; i < 8; i++) {
        text[i] = (digital & (1U << i)) ? '1' : '0';
    }
    text[8] = '\0';
}

extern const char *g_dbg_cmd_name;
extern int16_t g_dbg_left_speed;
extern int16_t g_dbg_right_speed;
extern uint32_t g_dbg_elapsed_ms;
extern uint8_t g_dbg_active;

void OLED_App_Init(void)
{
    Drv_OLED_Init();
    Drv_OLED_Clear();
    Drv_OLED_ShowString(0, 0, "Debug Ready");
    Drv_OLED_Display();

    g_last_frame_tick = BSP_GetTick();
}

void OLED_App_Task(uint32_t now_ms)
{
    if ((now_ms - g_last_frame_tick) < OLED_FRAME_INTERVAL_MS) return;
    g_last_frame_tick = now_ms;

    char line[20];
    Drv_OLED_Clear();

    snprintf(line, sizeof(line), "DBG %s", g_dbg_cmd_name);
    Drv_OLED_ShowString(0, 0, line);

    snprintf(line, sizeof(line), "L:%+4d R:%+4d", g_dbg_left_speed, g_dbg_right_speed);
    Drv_OLED_ShowString(2, 0, line);

    snprintf(line, sizeof(line), "T:%4lums", g_dbg_elapsed_ms);
    Drv_OLED_ShowString(4, 0, line);

    Drv_OLED_ShowString(6, 0, g_dbg_active ? "ACTIVE" : "IDLE");

    Drv_OLED_Display();
}
