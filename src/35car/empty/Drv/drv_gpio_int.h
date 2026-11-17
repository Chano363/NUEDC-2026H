#ifndef _DRV_GPIO_INT_H
#define _DRV_GPIO_INT_H

#include <stdint.h>
#include <stdbool.h>
#include "ti_msp_dl_config.h"

typedef void (*GPIO_INT_Callback)(void);

void Drv_GPIO_INT_Init(void);
void Drv_GPIO_INT_Enable(GPIO_Regs *port, uint32_t pin);
void Drv_GPIO_INT_Disable(GPIO_Regs *port, uint32_t pin);
void Drv_GPIO_INT_Clear(GPIO_Regs *port, uint32_t pin);
void Drv_GPIO_INT_RegisterCallback(GPIO_INT_Callback callback);

#endif
