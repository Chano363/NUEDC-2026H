#ifndef _GYRO_UART_H
#define _GYRO_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float angle_deg;
    float dps;
    bool valid;
} GyroData_t;

void Gyro_Init(void);
bool Gyro_GetData(GyroData_t *data);
void Gyro_ProcessByte(uint8_t byte);

#endif