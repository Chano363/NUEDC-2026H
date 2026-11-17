#ifndef _RINGBUF_H
#define _RINGBUF_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} RingBuf_HandleTypeDef;

bool RingBuf_Init(RingBuf_HandleTypeDef *rb, uint8_t *buffer, uint16_t size);
bool RingBuf_Push(RingBuf_HandleTypeDef *rb, uint8_t data);
bool RingBuf_Pop(RingBuf_HandleTypeDef *rb, uint8_t *data);
bool RingBuf_IsFull(RingBuf_HandleTypeDef *rb);
bool RingBuf_IsEmpty(RingBuf_HandleTypeDef *rb);
uint16_t RingBuf_GetCount(RingBuf_HandleTypeDef *rb);
void RingBuf_Clear(RingBuf_HandleTypeDef *rb);

#endif