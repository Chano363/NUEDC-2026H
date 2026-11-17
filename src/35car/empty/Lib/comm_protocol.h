#ifndef _COMM_PROTOCOL_H
#define _COMM_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#define COMM_HEADER          0xAA
#define COMM_FOOTER          0x55
#define COMM_MAX_PAYLOAD_LEN 32

typedef enum {
    COMM_CMD_CAR_STATUS       = 0x10,
    COMM_CMD_CAR_POSITION     = 0x11,
    COMM_CMD_CAR_EMERGENCY    = 0x12,
    COMM_CMD_CAR_START_AIMING = 0x13,
    COMM_CMD_CAR_STOP_AIMING  = 0x14,
    
    COMM_CMD_GIMBAL_STATUS     = 0x15,
    COMM_CMD_GIMBAL_HIT_CONFIRM = 0x16,
    COMM_CMD_GIMBAL_LASER_ON   = 0x17,
    COMM_CMD_GIMBAL_LASER_OFF  = 0x18,
    COMM_CMD_HEARTBEAT         = 0x19
} Comm_CmdTypeDef;

typedef enum {
    COMM_CAR_STATE_IDLE = 0,
    COMM_CAR_STATE_RUNNING,
    COMM_CAR_STATE_STOPPED,
    COMM_CAR_STATE_EMERGENCY
} Comm_CarStateTypeDef;

typedef enum {
    CAR_EDGE_AB = 0,
    CAR_EDGE_BC,
    CAR_EDGE_CD,
    CAR_EDGE_DA
} Car_EdgeTypeDef;

typedef struct {
    uint8_t lap_count;
    Car_EdgeTypeDef current_edge;
    Comm_CarStateTypeDef state;
    uint8_t reserved;
} Comm_CarStatusData;

typedef struct {
    int16_t x_pos;
    int16_t y_pos;
    int16_t heading;
} Comm_CarPositionData;

typedef struct {
    uint8_t reason;
} Comm_CarEmergencyData;

typedef enum {
    GIMBAL_MODE_STOP = 0,
    GIMBAL_MODE_TRACKING,
    GIMBAL_MODE_CALIBRATION,
    GIMBAL_MODE_LOCKED
} Comm_GimbalModeTypeDef;

typedef struct {
    Comm_GimbalModeTypeDef mode;
    int16_t rotation;
    int16_t pitch;
    bool locked;
} Comm_GimbalStatusData;

typedef struct {
    uint16_t hit_distance;
    uint8_t confidence;
} Comm_GimbalHitData;

typedef struct {
    uint8_t buffer[COMM_MAX_PAYLOAD_LEN + 6];
    uint8_t len;
    uint8_t state;
} Comm_FrameBuffer;

uint8_t Comm_CalcChecksum(const uint8_t *data, uint8_t len);
uint8_t Comm_PackCarStatus(uint8_t *frame, const Comm_CarStatusData *data);
uint8_t Comm_PackCarPosition(uint8_t *frame, const Comm_CarPositionData *data);
uint8_t Comm_PackCarEmergency(uint8_t *frame, const Comm_CarEmergencyData *data);
uint8_t Comm_PackStartAiming(uint8_t *frame);
uint8_t Comm_PackStopAiming(uint8_t *frame);
uint8_t Comm_PackGimbalStatus(uint8_t *frame, const Comm_GimbalStatusData *data);
uint8_t Comm_PackHitConfirm(uint8_t *frame, const Comm_GimbalHitData *data);
uint8_t Comm_PackLaserOn(uint8_t *frame);
uint8_t Comm_PackLaserOff(uint8_t *frame);
uint8_t Comm_PackHeartbeat(uint8_t *frame);

bool Comm_ParseFrame(Comm_FrameBuffer *buffer, uint8_t byte, uint8_t *cmd, uint8_t *payload, uint8_t *payload_len);

#endif