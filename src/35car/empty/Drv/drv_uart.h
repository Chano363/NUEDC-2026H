#ifndef _DRV_UART_H
#define _DRV_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "ti_msp_dl_config.h"

typedef void (*UART_RX_Callback)(uint8_t data);

void Drv_UART_Init(UART_Regs *uart_inst);
void Drv_UART_SendByte(UART_Regs *uart_inst, uint8_t data);
void Drv_UART_SendData(UART_Regs *uart_inst, const uint8_t *data, uint16_t len);
void Drv_UART_ProcessTX(void); 
void Drv_UART_RegisterCallback(UART_Regs *uart_inst, UART_RX_Callback callback);
bool Drv_UART_Receive(UART_Regs *uart_inst, uint8_t *data, uint16_t len);

#endif