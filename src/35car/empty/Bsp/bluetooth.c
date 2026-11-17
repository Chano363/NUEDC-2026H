#include "bluetooth.h"
#include "../Drv/drv_uart.h"
#include "../Lib/ringbuf.h"
#include "ti_msp_dl_config.h"
#include <string.h>

static uint8_t g_rx_buffer[BLUETOOTH_CMD_BUFFER_SIZE];
static RingBuf_HandleTypeDef g_rx_ringbuf;
static Bluetooth_CmdDef g_cmd;

void Bluetooth_Init(void)
{
    RingBuf_Init(&g_rx_ringbuf, g_rx_buffer, BLUETOOTH_CMD_BUFFER_SIZE);
    Drv_UART_Init(UART_BT_INST);
    Drv_UART_RegisterCallback(UART_BT_INST, Bluetooth_ProcessByte);
    
    g_cmd.cmd_length = 0;
    g_cmd.has_command = false;
}

void Bluetooth_SendString(const char *str)
{
    if (str == NULL) return;
    Drv_UART_SendData(UART_BT_INST, (const uint8_t *)str, (uint16_t)strlen(str));
}

void Bluetooth_SendData(const uint8_t *data, uint16_t len)
{
    Drv_UART_SendData(UART_BT_INST, data, len);
}

void Bluetooth_ProcessByte(uint8_t byte)
{
    /* 'D' 是所有命令的结束位 */
    if (byte == 'D') {
        g_cmd.cmd_buffer[g_cmd.cmd_length++] = 'D';  /* 'D' 也作为命令最后一个字符 */
        g_cmd.has_command = true;
        g_cmd.cmd_buffer[g_cmd.cmd_length] = '\0';
        /* cmd_length 保留，由 ClearCommand 重置 */
    } else {
        /* 命令待处理时不再接收新字节 */
        if (g_cmd.has_command) return;
        if (byte != '\r' && byte != '\n' && g_cmd.cmd_length < BLUETOOTH_CMD_BUFFER_SIZE - 1) {
            g_cmd.cmd_buffer[g_cmd.cmd_length++] = byte;
        }
    }
}

bool Bluetooth_HasCommand(void)
{
    return g_cmd.has_command;
}

void Bluetooth_GetCommand(Bluetooth_CmdDef *cmd)
{
    if (cmd == NULL) return;
    memcpy(cmd->cmd_buffer, g_cmd.cmd_buffer, g_cmd.cmd_length + 1);
    cmd->cmd_length = g_cmd.cmd_length;
    cmd->has_command = g_cmd.has_command;
}

void Bluetooth_ClearCommand(void)
{
    g_cmd.cmd_length = 0;
    g_cmd.has_command = false;
    memset(g_cmd.cmd_buffer, 0, BLUETOOTH_CMD_BUFFER_SIZE);
}