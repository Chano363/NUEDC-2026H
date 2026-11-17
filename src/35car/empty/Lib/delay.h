#ifndef _DELAY_H
#define _DELAY_H

#include <stdint.h>

/**
 * @brief 阻塞式毫秒延时
 * @param ms 延时毫秒数
 * @note 基于SysTick实现，会阻塞CPU，仅用于初始化阶段
 */
void delay_ms(uint32_t ms);

/**
 * @brief 阻塞式微秒延时（粗略）
 * @param us 延时微秒数
 * @note 基于CPU循环计数，精度有限，仅用于调试
 */
void delay_us(uint32_t us);

#endif /* _DELAY_H */