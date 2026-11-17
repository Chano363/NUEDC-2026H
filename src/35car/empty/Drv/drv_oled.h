#ifndef _DRV_OLED_H
#define _DRV_OLED_H

#include <stdint.h>
#include <stdbool.h>

#define OLED_WIDTH  128
#define OLED_HEIGHT  64
#define OLED_PAGES    8   /* 64 / 8 */

/**
 * @brief 初始化 OLED (SSD1306, I2C1)
 *        硬件I2C引脚：PB2(SCL), PB3(SDA)
 */
void Drv_OLED_Init(void);

/**
 * @brief 清屏
 */
void Drv_OLED_Clear(void);

/**
 * @brief 设置光标位置
 * @param page 页 (0~7)
 * @param col  列 (0~127)
 */
void Drv_OLED_SetCursor(uint8_t page, uint8_t col);

/**
 * @brief 在当前位置显示一个ASCII字符（8×16点阵）
 * @param ch 字符
 */
void Drv_OLED_ShowChar(char ch);

/**
 * @brief 在当前页显示字符串（8×16点阵，自动换行）
 * @param str 字符串
 */
void Drv_OLED_ShowString(uint8_t page, uint8_t col, const char *str);

/**
 * @brief 刷新显示（将帧缓冲区发送到OLED）
 */
void Drv_OLED_Display(void);

#endif /* _DRV_OLED_H */
