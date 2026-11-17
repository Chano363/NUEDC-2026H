#ifndef _KEY_H
#define _KEY_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    KEY_ID_START = 0,
    KEY_ID_MODE,
    KEY_ID_EMERGENCY,
    KEY_ID_MAX
} Key_IDTypeDef;

typedef enum {
    KEY_STATE_RELEASED = 0,
    KEY_STATE_PRESSED,
    KEY_STATE_HOLD
} Key_StateTypeDef;

void Key_Init(void);

/* Call this from the main loop. now_ms must come from the 1 ms SysTick. */
void Key_Update(uint32_t now_ms);

bool Key_IsPressed(Key_IDTypeDef key_id);
Key_StateTypeDef Key_GetState(Key_IDTypeDef key_id);
bool Key_GetRawPressed(Key_IDTypeDef key_id);
uint32_t Key_GetPressDuration(uint32_t now_ms, Key_IDTypeDef key_id);

/* One-shot events. The event is cleared after it is read. */
bool Key_GetShortPressEvent(Key_IDTypeDef key_id);
bool Key_GetLongPressEvent(Key_IDTypeDef key_id);

/* Persistent counters for diagnostics. They are not cleared when read. */
uint16_t Key_GetShortPressCount(Key_IDTypeDef key_id);
uint16_t Key_GetLongPressCount(Key_IDTypeDef key_id);

/* Key test: display KEY1/KEY2/KEY3 states on OLED */
void key_test(void);

#endif
