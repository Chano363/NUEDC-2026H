#ifndef _BOARD_COMM_H
#define _BOARD_COMM_H

#include <stdint.h>
#include <stdbool.h>
#include "../Lib/comm_protocol.h"

void BoardComm_Init(void);
void BoardComm_SendCarStatus(const Comm_CarStatusData *data);
void BoardComm_SendCarPosition(const Comm_CarPositionData *data);
void BoardComm_SendCarEmergency(const Comm_CarEmergencyData *data);
void BoardComm_SendStartAiming(void);
void BoardComm_SendStopAiming(void);
void BoardComm_SendHeartbeat(void);
void BoardComm_Process(void);

bool BoardComm_IsGimbalConnected(void);
Comm_GimbalStatusData BoardComm_GetGimbalStatus(void);
bool BoardComm_GetHitConfirm(Comm_GimbalHitData *data);

#endif