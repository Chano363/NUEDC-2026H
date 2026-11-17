#include "encoder.h"
#include "../Drv/drv_gpio_int.h"
#include "ti_msp_dl_config.h"

static volatile int32_t g_left_count = 0;
static volatile int32_t g_right_count = 0;
static uint8_t g_last_la = 0;
static uint8_t g_last_lb = 0;
static uint8_t g_last_ra = 0;
static uint8_t g_last_rb = 0;

/* ── 速度计算 ── */
static int32_t g_last_left_count = 0;
static int32_t g_last_right_count = 0;
static int32_t g_left_speed = 0;    /* counts/20ms */
static int32_t g_right_speed = 0;

void Encoder_Init(void)
{
    Drv_GPIO_INT_RegisterCallback(Encoder_ProcessInterrupt);
    
    g_last_la = DL_GPIO_readPins(ENC_PORT, ENC_LA_PIN) != 0;
    g_last_lb = DL_GPIO_readPins(ENC_PORT, ENC_LB_PIN) != 0;
    g_last_ra = DL_GPIO_readPins(ENC_PORT, ENC_RA_PIN) != 0;
    g_last_rb = DL_GPIO_readPins(ENC_PORT, ENC_RB_PIN) != 0;

    g_last_left_count  = 0;
    g_last_right_count = 0;
    g_left_speed  = 0;
    g_right_speed = 0;

    Drv_GPIO_INT_Init();
}

void Encoder_ResetLeftCount(void)
{
    g_left_count = 0;
    g_last_left_count = 0;
    g_left_speed = 0;
}

void Encoder_ResetRightCount(void)
{
    g_right_count = 0;
    g_last_right_count = 0;
    g_right_speed = 0;
}

int32_t Encoder_GetLeftCount(void)
{
    return g_left_count;
}

int32_t Encoder_GetRightCount(void)
{
    return g_right_count;
}

void Encoder_UpdateSpeed(void)
{
    int32_t cur_left  = g_left_count;
    int32_t cur_right = g_right_count;

    g_left_speed  = cur_left  - g_last_left_count;
    g_right_speed = cur_right - g_last_right_count;

    g_last_left_count  = cur_left;
    g_last_right_count = cur_right;
}

int32_t Encoder_GetLeftSpeed(void)
{
    return g_left_speed;
}

int32_t Encoder_GetRightSpeed(void)
{
    return g_right_speed;
}

void Encoder_ProcessInterrupt(void)
{
    uint8_t la = DL_GPIO_readPins(ENC_PORT, ENC_LA_PIN) != 0;
    uint8_t lb = DL_GPIO_readPins(ENC_PORT, ENC_LB_PIN) != 0;
    uint8_t ra = DL_GPIO_readPins(ENC_PORT, ENC_RA_PIN) != 0;
    uint8_t rb = DL_GPIO_readPins(ENC_PORT, ENC_RB_PIN) != 0;
    
    /* 四相解码 — 极性已校准：前进=正计数 */
    if (la != g_last_la) {
        if (la != lb) {
            g_left_count++;
        } else {
            g_left_count--;
        }
        g_last_la = la;
    }
    
    if (lb != g_last_lb) {
        if (la == lb) {
            g_left_count++;
        } else {
            g_left_count--;
        }
        g_last_lb = lb;
    }
    
    if (ra != g_last_ra) {
        if (ra != rb) {
            g_right_count--;
        } else {
            g_right_count++;
        }
        g_last_ra = ra;
    }
    
    if (rb != g_last_rb) {
        if (ra == rb) {
            g_right_count--;
        } else {
            g_right_count++;
        }
        g_last_rb = rb;
    }
    
    Drv_GPIO_INT_Clear(ENC_PORT, ENC_LA_PIN);
    Drv_GPIO_INT_Clear(ENC_PORT, ENC_LB_PIN);
    Drv_GPIO_INT_Clear(ENC_PORT, ENC_RA_PIN);
    Drv_GPIO_INT_Clear(ENC_PORT, ENC_RB_PIN);
}
