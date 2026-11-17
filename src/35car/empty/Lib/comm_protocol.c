#include "comm_protocol.h"

uint8_t Comm_CalcChecksum(const uint8_t *data, uint8_t len)
{
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum & 0xFF;
}

uint8_t Comm_PackCarStatus(uint8_t *frame, const Comm_CarStatusData *data)
{
    uint8_t payload[4];
    payload[0] = data->lap_count;
    payload[1] = (uint8_t)data->current_edge;
    payload[2] = (uint8_t)data->state;
    payload[3] = data->reserved;
    
    uint8_t length = 5;
    uint8_t cmd = COMM_CMD_CAR_STATUS;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1) + Comm_CalcChecksum(payload, 4);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_CAR_STATUS;
    frame[3] = payload[0];
    frame[4] = payload[1];
    frame[5] = payload[2];
    frame[6] = payload[3];
    frame[7] = checksum & 0xFF;
    frame[8] = COMM_FOOTER;
    
    return 9;
}

uint8_t Comm_PackCarPosition(uint8_t *frame, const Comm_CarPositionData *data)
{
    uint8_t payload[6];
    payload[0] = (uint8_t)(data->x_pos & 0xFF);
    payload[1] = (uint8_t)((data->x_pos >> 8) & 0xFF);
    payload[2] = (uint8_t)(data->y_pos & 0xFF);
    payload[3] = (uint8_t)((data->y_pos >> 8) & 0xFF);
    payload[4] = (uint8_t)(data->heading & 0xFF);
    payload[5] = (uint8_t)((data->heading >> 8) & 0xFF);
    
    uint8_t length = 7;
    uint8_t cmd = COMM_CMD_CAR_POSITION;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1) + Comm_CalcChecksum(payload, 6);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_CAR_POSITION;
    for (uint8_t i = 0; i < 6; i++) {
        frame[3 + i] = payload[i];
    }
    frame[9] = checksum & 0xFF;
    frame[10] = COMM_FOOTER;
    
    return 11;
}

uint8_t Comm_PackCarEmergency(uint8_t *frame, const Comm_CarEmergencyData *data)
{
    uint8_t payload[1];
    payload[0] = data->reason;
    
    uint8_t length = 2;
    uint8_t cmd = COMM_CMD_CAR_EMERGENCY;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1) + Comm_CalcChecksum(payload, 1);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_CAR_EMERGENCY;
    frame[3] = payload[0];
    frame[4] = checksum & 0xFF;
    frame[5] = COMM_FOOTER;
    
    return 6;
}

uint8_t Comm_PackStartAiming(uint8_t *frame)
{
    uint8_t length = 1;
    uint8_t cmd = COMM_CMD_CAR_START_AIMING;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_CAR_START_AIMING;
    frame[3] = checksum & 0xFF;
    frame[4] = COMM_FOOTER;
    
    return 5;
}

uint8_t Comm_PackStopAiming(uint8_t *frame)
{
    uint8_t length = 1;
    uint8_t cmd = COMM_CMD_CAR_STOP_AIMING;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_CAR_STOP_AIMING;
    frame[3] = checksum & 0xFF;
    frame[4] = COMM_FOOTER;
    
    return 5;
}

uint8_t Comm_PackGimbalStatus(uint8_t *frame, const Comm_GimbalStatusData *data)
{
    uint8_t payload[6];
    payload[0] = (uint8_t)data->mode;
    payload[1] = (uint8_t)(data->rotation & 0xFF);
    payload[2] = (uint8_t)((data->rotation >> 8) & 0xFF);
    payload[3] = (uint8_t)(data->pitch & 0xFF);
    payload[4] = (uint8_t)((data->pitch >> 8) & 0xFF);
    payload[5] = data->locked ? 0x01 : 0x00;
    
    uint8_t length = 7;
    uint8_t cmd = COMM_CMD_GIMBAL_STATUS;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1) + Comm_CalcChecksum(payload, 6);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_GIMBAL_STATUS;
    for (uint8_t i = 0; i < 6; i++) {
        frame[3 + i] = payload[i];
    }
    frame[9] = checksum & 0xFF;
    frame[10] = COMM_FOOTER;
    
    return 11;
}

uint8_t Comm_PackHitConfirm(uint8_t *frame, const Comm_GimbalHitData *data)
{
    uint8_t payload[3];
    payload[0] = (uint8_t)(data->hit_distance & 0xFF);
    payload[1] = (uint8_t)((data->hit_distance >> 8) & 0xFF);
    payload[2] = data->confidence;
    
    uint8_t length = 4;
    uint8_t cmd = COMM_CMD_GIMBAL_HIT_CONFIRM;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1) + Comm_CalcChecksum(payload, 3);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_GIMBAL_HIT_CONFIRM;
    for (uint8_t i = 0; i < 3; i++) {
        frame[3 + i] = payload[i];
    }
    frame[6] = checksum & 0xFF;
    frame[7] = COMM_FOOTER;
    
    return 8;
}

uint8_t Comm_PackLaserOn(uint8_t *frame)
{
    uint8_t length = 1;
    uint8_t cmd = COMM_CMD_GIMBAL_LASER_ON;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_GIMBAL_LASER_ON;
    frame[3] = checksum & 0xFF;
    frame[4] = COMM_FOOTER;
    
    return 5;
}

uint8_t Comm_PackLaserOff(uint8_t *frame)
{
    uint8_t length = 1;
    uint8_t cmd = COMM_CMD_GIMBAL_LASER_OFF;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_GIMBAL_LASER_OFF;
    frame[3] = checksum & 0xFF;
    frame[4] = COMM_FOOTER;
    
    return 5;
}

uint8_t Comm_PackHeartbeat(uint8_t *frame)
{
    uint8_t length = 1;
    uint8_t cmd = COMM_CMD_HEARTBEAT;
    uint8_t checksum = Comm_CalcChecksum(&cmd, 1);
    
    frame[0] = COMM_HEADER;
    frame[1] = length;
    frame[2] = COMM_CMD_HEARTBEAT;
    frame[3] = checksum & 0xFF;
    frame[4] = COMM_FOOTER;
    
    return 5;
}

bool Comm_ParseFrame(Comm_FrameBuffer *buffer, uint8_t byte, uint8_t *cmd, uint8_t *payload, uint8_t *payload_len)
{
    switch (buffer->state) {
        case 0:
            if (byte == COMM_HEADER) {
                buffer->buffer[0] = byte;
                buffer->len = 1;
                buffer->state = 1;
            }
            break;
            
        case 1:
            buffer->buffer[buffer->len++] = byte;
            buffer->state = 2;
            break;
            
        case 2:
            buffer->buffer[buffer->len++] = byte;
            *cmd = byte;
            buffer->state = 3;
            break;
            
        case 3: {
            uint8_t expected_len = buffer->buffer[1] - 1;
            buffer->buffer[buffer->len++] = byte;
            
            if (buffer->len >= 3 + expected_len) {
                buffer->state = 4;
            }
            break;
        }
            
        case 4:
            buffer->buffer[buffer->len++] = byte;
            buffer->state = 5;
            break;
            
        case 5:
            if (byte == COMM_FOOTER) {
                buffer->buffer[buffer->len++] = byte;
                
                uint8_t expected_len = buffer->buffer[1] - 1;
                uint8_t calc_checksum = Comm_CalcChecksum(&buffer->buffer[2], expected_len + 1);
                
                if (calc_checksum == buffer->buffer[3 + expected_len]) {
                    *payload_len = expected_len;
                    for (uint8_t i = 0; i < expected_len; i++) {
                        payload[i] = buffer->buffer[3 + i];
                    }
                    
                    buffer->state = 0;
                    buffer->len = 0;
                    return true;
                }
            }
            
            buffer->state = 0;
            buffer->len = 0;
            break;
            
        default:
            buffer->state = 0;
            buffer->len = 0;
            break;
    }
    
    return false;
}