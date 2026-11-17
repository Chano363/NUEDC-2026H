#include "delay.h"
#include "../Bsp/bsp_init.h"

/**
 * @brief 阻塞式毫秒延时
 *
 * 使用SysTick计时器实现精确的毫秒级延时。
 * 注意：此函数会阻塞CPU，不要在实时控制循环中使用！
 *
 * @param ms 要延时的毫秒数
 */
void delay_ms(uint32_t ms)
{
    uint32_t start = BSP_GetTick();
    while (BSP_GetTick() - start < ms) {
        /* 等待时间到达 */
        __asm volatile("nop");  /* 空循环，避免编译器优化 */
    }
}

/**
 * @brief 阻塞式微秒延时（粗略）
 *
 * 基于CPU频率计算循环次数，实现粗略的微秒级延时。
 * 注意：精度受CPU频率和编译器优化影响，仅适用于调试场景。
 *
 * 假设CPU频率为32MHz：
 * - 每条循环指令约需3个时钟周期
 * - 每微秒需要32个时钟周期
 * - 因此每微秒需要约10次循环
 *
 * @param us 要延时的微秒数
 */
void delay_us(uint32_t us)
{
    volatile uint32_t count;
    /* 32MHz CPU：每微秒约需要32/3 ≈ 10次循环 */
    count = us * 10;

    while (count-- > 0) {
        __asm volatile("nop");
    }
}