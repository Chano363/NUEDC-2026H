#include <stddef.h>
#include "ringbuf.h"

bool RingBuf_Init(RingBuf_HandleTypeDef *rb, uint8_t *buffer, uint16_t size)
{
    if (rb == NULL || buffer == NULL || size == 0) return false;
    
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    
    return true;
}

bool RingBuf_Push(RingBuf_HandleTypeDef *rb, uint8_t data)
{
    if (rb == NULL || RingBuf_IsFull(rb)) return false;
    
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;
    rb->count++;
    
    return true;
}

bool RingBuf_Pop(RingBuf_HandleTypeDef *rb, uint8_t *data)
{
    if (rb == NULL || data == NULL || RingBuf_IsEmpty(rb)) return false;
    
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;
    
    return true;
}

bool RingBuf_IsFull(RingBuf_HandleTypeDef *rb)
{
    return (rb != NULL) && (rb->count >= rb->size);
}

bool RingBuf_IsEmpty(RingBuf_HandleTypeDef *rb)
{
    return (rb == NULL) || (rb->count == 0);
}

uint16_t RingBuf_GetCount(RingBuf_HandleTypeDef *rb)
{
    return (rb != NULL) ? rb->count : 0;
}

void RingBuf_Clear(RingBuf_HandleTypeDef *rb)
{
    if (rb == NULL) return;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}