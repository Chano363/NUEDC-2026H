#ifndef _PROTOCOL_PARSER_H
#define _PROTOCOL_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#define PROTOCOL_FRAME_LENGTH 9
#define PROTOCOL_HEADER1 0x0A
#define PROTOCOL_HEADER2 0x03
#define PROTOCOL_HEADER3 0x04

typedef struct {
    uint8_t buffer[PROTOCOL_FRAME_LENGTH];
    uint8_t index;
    uint8_t state;
} Protocol_ParserHandleTypeDef;

void Protocol_ParserInit(Protocol_ParserHandleTypeDef *parser);
bool Protocol_ParserProcess(Protocol_ParserHandleTypeDef *parser, uint8_t data);
bool Protocol_ParserIsComplete(Protocol_ParserHandleTypeDef *parser);
void Protocol_ParserReset(Protocol_ParserHandleTypeDef *parser);

#endif