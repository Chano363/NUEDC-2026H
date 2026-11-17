#include "key.h"
#include "ti_msp_dl_config.h"
#include "Drv/drv_oled.h"
#include "Bsp/bsp_init.h"
#include <stdio.h>

#define KEY_DEBOUNCE_MS   20U
#define KEY_LONG_PRESS_MS 800U

/* Keep the board-specific KEY3 setup here; the key algorithm remains generic. */

static Key_StateTypeDef g_key_state[KEY_ID_MAX];
static bool g_raw_pressed[KEY_ID_MAX];
static bool g_stable_pressed[KEY_ID_MAX];
static bool g_short_press_event[KEY_ID_MAX];
static bool g_long_press_event[KEY_ID_MAX];
static uint32_t g_raw_change_tick[KEY_ID_MAX];
static uint32_t g_press_tick[KEY_ID_MAX];
static uint32_t g_last_press_duration[KEY_ID_MAX];
static uint16_t g_short_press_count[KEY_ID_MAX];
static uint16_t g_long_press_count[KEY_ID_MAX];

static bool key_read_pressed(Key_IDTypeDef key_id)
{
    switch (key_id) {
    case KEY_ID_START:
        return DL_GPIO_readPins(KEY_PORT, KEY_KEY1_PIN) == 0;
    case KEY_ID_MODE:
        return DL_GPIO_readPins(KEY_PORT, KEY_KEY2_PIN) == 0;
    case KEY_ID_EMERGENCY:
        return DL_GPIO_readPins(KEY_PORT, KEY_KEY3_PIN) == 0;
    default:
        return false;
    }
}

static uint16_t increment_counter(uint16_t value)
{
    return (value == UINT16_MAX) ? value : (uint16_t)(value + 1U);
}

void Key_Init(void)
{
    for (uint8_t i = 0; i < KEY_ID_MAX; i++) {
        g_key_state[i] = KEY_STATE_RELEASED;
        g_raw_pressed[i] = key_read_pressed((Key_IDTypeDef)i);
        g_stable_pressed[i] = false;
        g_short_press_event[i] = false;
        g_long_press_event[i] = false;
        g_raw_change_tick[i] = 0;
        g_press_tick[i] = 0;
        g_last_press_duration[i] = 0;
        g_short_press_count[i] = 0;
        g_long_press_count[i] = 0;
    }
}

void Key_Update(uint32_t now_ms)
{
    for (uint8_t i = 0; i < KEY_ID_MAX; i++) {
        Key_IDTypeDef key_id = (Key_IDTypeDef)i;
        bool raw_pressed = key_read_pressed(key_id);

        if (raw_pressed != g_raw_pressed[i]) {
            g_raw_pressed[i] = raw_pressed;
            g_raw_change_tick[i] = now_ms;
        }

        /* A state changes only after the raw level has been stable. */
        if (raw_pressed != g_stable_pressed[i]
            && (now_ms - g_raw_change_tick[i]) >= KEY_DEBOUNCE_MS) {
            g_stable_pressed[i] = raw_pressed;

            if (raw_pressed) {
                g_key_state[i] = KEY_STATE_PRESSED;
                g_press_tick[i] = now_ms;
            } else {
                g_last_press_duration[i] = now_ms - g_press_tick[i];
                if (g_key_state[i] == KEY_STATE_PRESSED) {
                    g_short_press_event[i] = true;
                    g_short_press_count[i] = increment_counter(g_short_press_count[i]);
                }
                g_key_state[i] = KEY_STATE_RELEASED;
            }
        }

        /* Long press is recognized while the key is still held. */
        if (g_stable_pressed[i]
            && g_key_state[i] == KEY_STATE_PRESSED
            && (now_ms - g_press_tick[i]) >= KEY_LONG_PRESS_MS) {
            g_key_state[i] = KEY_STATE_HOLD;
            g_long_press_event[i] = true;
            g_long_press_count[i] = increment_counter(g_long_press_count[i]);
        }
    }
}

bool Key_IsPressed(Key_IDTypeDef key_id)
{
    return (key_id < KEY_ID_MAX) && g_stable_pressed[key_id];
}

Key_StateTypeDef Key_GetState(Key_IDTypeDef key_id)
{
    if (key_id >= KEY_ID_MAX) return KEY_STATE_RELEASED;
    return g_key_state[key_id];
}

bool Key_GetRawPressed(Key_IDTypeDef key_id)
{
    return (key_id < KEY_ID_MAX) && g_raw_pressed[key_id];
}

uint32_t Key_GetPressDuration(uint32_t now_ms, Key_IDTypeDef key_id)
{
    if (key_id >= KEY_ID_MAX) return 0;
    if (g_stable_pressed[key_id]) return now_ms - g_press_tick[key_id];
    return g_last_press_duration[key_id];
}

bool Key_GetShortPressEvent(Key_IDTypeDef key_id)
{
    bool event;

    if (key_id >= KEY_ID_MAX) return false;
    event = g_short_press_event[key_id];
    g_short_press_event[key_id] = false;
    return event;
}

bool Key_GetLongPressEvent(Key_IDTypeDef key_id)
{
    bool event;

    if (key_id >= KEY_ID_MAX) return false;
    event = g_long_press_event[key_id];
    g_long_press_event[key_id] = false;
    return event;
}

uint16_t Key_GetShortPressCount(Key_IDTypeDef key_id)
{
    return (key_id < KEY_ID_MAX) ? g_short_press_count[key_id] : 0;
}

uint16_t Key_GetLongPressCount(Key_IDTypeDef key_id)
{
    return (key_id < KEY_ID_MAX) ? g_long_press_count[key_id] : 0;
}

/* ══════════════════════════════════════════════════════════════
   key_test — 按键测试：在OLED上实时显示各按键状态
   直接使用 Key_GetState 实时显示 REL / PRE / HLD
   ══════════════════════════════════════════════════════════════ */

void key_test(void)
{
    char line[20];

    Drv_OLED_Clear();
    for (uint8_t i = 0; i < KEY_ID_MAX; i++) {
        Key_IDTypeDef key_id = (Key_IDTypeDef)i;
        const char *s = "REL";
        Key_StateTypeDef st = Key_GetState(key_id);
        if (st == KEY_STATE_PRESSED) s = "PRE";
        if (st == KEY_STATE_HOLD)    s = "HLD";

        snprintf(line, sizeof(line), "KEY%u:%s", (unsigned)(i + 1U), s);
        Drv_OLED_ShowString((uint8_t)(i * 2U), 0, line);
    }
    Drv_OLED_Display();
}
