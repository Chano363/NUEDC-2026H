#ifndef _BSP_INIT_H
#define _BSP_INIT_H

#include <stdint.h>

/**
 * @brief 板级初始化
 */
void BSP_Init(void);

/**
 * @brief 单独初始化 SysTick（供硬件测试用，不执行 MotorDrv_Init）
 */
void BSP_SysTick_Init(void);

/**
 * @brief 获取系统时间
 * @return 自启动以来的毫秒数
 */
uint32_t BSP_GetTick(void);

/**
 * @brief 检查是否到达控制周期（基于SysTick，20ms）
 * @return 1表示到达控制周期，0表示未到达
 */
uint8_t BSP_ControlTick(void);

#endif /* _BSP_INIT_H */
