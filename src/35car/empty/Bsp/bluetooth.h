#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#include <stdint.h>
#include <stdbool.h>

#define BLUETOOTH_CMD_BUFFER_SIZE 64

typedef struct {
    uint8_t cmd_buffer[BLUETOOTH_CMD_BUFFER_SIZE];
    uint16_t cmd_length;
    bool has_command;
} Bluetooth_CmdDef;

void Bluetooth_Init(void);
void Bluetooth_SendString(const char *str);
void Bluetooth_SendData(const uint8_t *data, uint16_t len);
void Bluetooth_ProcessByte(uint8_t byte);
bool Bluetooth_HasCommand(void);
void Bluetooth_GetCommand(Bluetooth_CmdDef *cmd);
void Bluetooth_ClearCommand(void);

#endif