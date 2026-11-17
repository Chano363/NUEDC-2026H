#ifndef _GRAYSCALE_H
#define _GRAYSCALE_H

#include <stdint.h>
#include <stdbool.h>

#define GRAYSCALE_LINE_ON     0x01
#define GRAYSCALE_LINE_LOST   0x02

typedef struct {
    uint8_t digital_status;
    int16_t deviation;
    uint8_t line_status;
} Grayscale_DataDef;

void Grayscale_Init(void);
void Grayscale_Update(Grayscale_DataDef *data);
uint8_t Grayscale_GetDigitalStatus(void);

#endif