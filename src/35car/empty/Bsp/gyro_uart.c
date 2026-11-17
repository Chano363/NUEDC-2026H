#include "gyro_uart.h"
#include "../Drv/drv_uart.h"
#include "../Lib/protocol_parser.h"
#include "ti_msp_dl_config.h"

static Protocol_ParserHandleTypeDef g_parser;
static GyroData_t g_gyro_data;

void Gyro_Init(void)
{
    Protocol_ParserInit(&g_parser);
    Drv_UART_Init(UART_GYRO_INST);
    Drv_UART_RegisterCallback(UART_GYRO_INST, Gyro_ProcessByte);
    
    g_gyro_data.angle_deg = 0.0f;
    g_gyro_data.dps = 0.0f;
    g_gyro_data.valid = false;
}

bool Gyro_GetData(GyroData_t *data)
{
    if (data == NULL) return false;
    *data = g_gyro_data;
    return g_gyro_data.valid;
}

void Gyro_ProcessByte(uint8_t byte)
{
    if (Protocol_ParserProcess(&g_parser, byte)) {
        uint8_t *frame = g_parser.buffer;
        
        int16_t angle_raw = (int16_t)(frame[3] << 8) | frame[4];
        int16_t dps_raw = (int16_t)(frame[5] << 8) | frame[6];
        
        g_gyro_data.angle_deg = (float)angle_raw / 100.0f;
        g_gyro_data.dps = (float)dps_raw / 100.0f;
        g_gyro_data.valid = true;
        
        Protocol_ParserReset(&g_parser);
    }
}