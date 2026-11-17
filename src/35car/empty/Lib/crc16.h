#ifndef _CRC16_H
#define _CRC16_H

#include <stdint.h>

uint16_t CRC16(const uint8_t *data, uint16_t length);
uint16_t CRC16_CCITT(const uint8_t *data, uint16_t length);

#endif