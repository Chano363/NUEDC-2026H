#include "bluetooth_cmd.h"
#include "../Bsp/bluetooth.h"
#include "tracking.h"
#include "speed_ctrl.h"
#include <string.h>
#include <stdio.h>

void BluetoothCmd_Init(void)
{
}

void BluetoothCmd_Process(void)
{
    if (!Bluetooth_HasCommand()) {
        return;
    }

    Bluetooth_CmdDef cmd;
    Bluetooth_GetCommand(&cmd);

    /* 命令格式: "N=1D", "N=2D", "N=5D" 等 */
    if (cmd.cmd_length >= 4
        && cmd.cmd_buffer[0] == 'N'
        && cmd.cmd_buffer[1] == '=')
    {
        /* 解析中间数字 */
        int laps = 0;
        for (uint16_t i = 2; i < cmd.cmd_length - 1; i++) {
            if (cmd.cmd_buffer[i] >= '0' && cmd.cmd_buffer[i] <= '9') {
                laps = laps * 10 + (cmd.cmd_buffer[i] - '0');
            }
        }

        if (laps >= 1 && laps <= 10) {
            Tracking_Start((uint8_t)laps);
            char resp[32];
            snprintf(resp, sizeof(resp), "OK: N=%d laps, START\r\n", laps);
            Bluetooth_SendString(resp);
        } else {
            Bluetooth_SendString("ERR: laps 1~10\r\n");
        }
    }
    /* "STOPD" 命令（D 是统一结尾，只判断 STOP） */
    else if (cmd.cmd_length >= 4
             && cmd.cmd_buffer[0] == 'S'
             && cmd.cmd_buffer[1] == 'T'
             && cmd.cmd_buffer[2] == 'O'
             && cmd.cmd_buffer[3] == 'P') {
        Tracking_Stop();
        SpeedCtrl_EmergencyStop();
        Bluetooth_SendString("STOPPED\r\n");
    }
    else {
        Bluetooth_SendString("NOT SUPPORTED\r\n");
        /* 回显收到的命令，帮助调试 */
        char echo[80];
        int len = snprintf(echo, sizeof(echo),
            "GOT[%d]: ", cmd.cmd_length);
        for (uint16_t i = 0; i < cmd.cmd_length; i++) {
            len += snprintf(echo + len, sizeof(echo) - len,
                "0x%02X ", cmd.cmd_buffer[i]);
        }
        len += snprintf(echo + len, sizeof(echo) - len, "\r\n");
        Bluetooth_SendData((const uint8_t *)echo, (uint16_t)len);
    }

    Bluetooth_ClearCommand();
}
