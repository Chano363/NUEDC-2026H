#include "board_comm.h"
#include "drv_uart.h"
#include "bsp_init.h"
#include "ti_msp_dl_config.h"

static Comm_FrameBuffer g_frame_buffer;
static Comm_GimbalStatusData g_gimbal_status;
static Comm_GimbalHitData g_hit_data;
static bool g_gimbal_connected = false;
static bool g_hit_received = false;
static uint32_t g_last_heartbeat_time = 0;

void BoardComm_Init(void)
{
    g_frame_buffer.len = 0;
    g_frame_buffer.state = 0;
    
    g_gimbal_status.mode = GIMBAL_MODE_STOP;
    g_gimbal_status.rotation = 0;
    g_gimbal_status.pitch = 0;
    g_gimbal_status.locked = false;
    
    g_gimbal_connected = false;
    g_hit_received = false;
}

static void BoardComm_SendFrame(uint8_t *frame, uint8_t len)
{
    Drv_UART_SendData(UART_COMM_INST, frame, len);
}

void BoardComm_SendCarStatus(const Comm_CarStatusData *data)
{
    uint8_t frame[16];
    uint8_t len = Comm_PackCarStatus(frame, data);
    BoardComm_SendFrame(frame, len);
}

void BoardComm_SendCarPosition(const Comm_CarPositionData *data)
{
    uint8_t frame[16];
    uint8_t len = Comm_PackCarPosition(frame, data);
    BoardComm_SendFrame(frame, len);
}

void BoardComm_SendCarEmergency(const Comm_CarEmergencyData *data)
{
    uint8_t frame[16];
    uint8_t len = Comm_PackCarEmergency(frame, data);
    BoardComm_SendFrame(frame, len);
}

void BoardComm_SendStartAiming(void)
{
    uint8_t frame[16];
    uint8_t len = Comm_PackStartAiming(frame);
    BoardComm_SendFrame(frame, len);
}

void BoardComm_SendStopAiming(void)
{
    uint8_t frame[16];
    uint8_t len = Comm_PackStopAiming(frame);
    BoardComm_SendFrame(frame, len);
}

void BoardComm_SendHeartbeat(void)
{
    uint8_t frame[16];
    uint8_t len = Comm_PackHeartbeat(frame);
    BoardComm_SendFrame(frame, len);
}

void BoardComm_Process(void)
{
    uint8_t byte;
    uint8_t cmd;
    uint8_t payload[COMM_MAX_PAYLOAD_LEN];
    uint8_t payload_len;
    
    while (Drv_UART_Receive(UART_COMM_INST, &byte, 1)) {
        if (Comm_ParseFrame(&g_frame_buffer, byte, &cmd, payload, &payload_len)) {
            g_gimbal_connected = true;
            g_last_heartbeat_time = BSP_GetTick();
            
            switch (cmd) {
                case COMM_CMD_GIMBAL_STATUS:
                    if (payload_len >= 6) {
                        g_gimbal_status.mode = (Comm_GimbalModeTypeDef)payload[0];
                        g_gimbal_status.rotation = (int16_t)(payload[1] | (payload[2] << 8));
                        g_gimbal_status.pitch = (int16_t)(payload[3] | (payload[4] << 8));
                        g_gimbal_status.locked = payload[5] != 0;
                    }
                    break;
                    
                case COMM_CMD_GIMBAL_HIT_CONFIRM:
                    if (payload_len >= 3) {
                        g_hit_data.hit_distance = (uint16_t)(payload[0] | (payload[1] << 8));
                        g_hit_data.confidence = payload[2];
                        g_hit_received = true;
                    }
                    break;
                    
                case COMM_CMD_GIMBAL_LASER_ON:
                    break;
                    
                case COMM_CMD_GIMBAL_LASER_OFF:
                    break;
                    
                case COMM_CMD_HEARTBEAT:
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    if (BSP_GetTick() - g_last_heartbeat_time > 2000) {
        g_gimbal_connected = false;
    }
}

bool BoardComm_IsGimbalConnected(void)
{
    return g_gimbal_connected;
}

Comm_GimbalStatusData BoardComm_GetGimbalStatus(void)
{
    return g_gimbal_status;
}

bool BoardComm_GetHitConfirm(Comm_GimbalHitData *data)
{
    if (g_hit_received) {
        *data = g_hit_data;
        g_hit_received = false;
        return true;
    }
    return false;
}