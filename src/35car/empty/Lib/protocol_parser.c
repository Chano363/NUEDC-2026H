#include <stddef.h>
#include "protocol_parser.h"
#include "crc16.h"

void Protocol_ParserInit(Protocol_ParserHandleTypeDef *parser)
{
    if (parser == NULL) return;
    parser->index = 0;
    parser->state = 0;
}

bool Protocol_ParserProcess(Protocol_ParserHandleTypeDef *parser, uint8_t data)
{
    if (parser == NULL) return false;
    
    switch (parser->state) {
        case 0:
            if (data == PROTOCOL_HEADER1) {
                parser->buffer[0] = data;
                parser->state = 1;
            }
            break;
            
        case 1:
            if (data == PROTOCOL_HEADER2) {
                parser->buffer[1] = data;
                parser->state = 2;
            } else {
                parser->state = 0;
            }
            break;
            
        case 2:
            if (data == PROTOCOL_HEADER3) {
                parser->buffer[2] = data;
                parser->index = 3;
                parser->state = 3;
            } else {
                parser->state = 0;
            }
            break;
            
        case 3:
            parser->buffer[parser->index++] = data;
            if (parser->index >= PROTOCOL_FRAME_LENGTH) {
                uint16_t calculated_crc = CRC16(&parser->buffer[0], 7);
                uint16_t received_crc = (uint16_t)(parser->buffer[7] << 8) | parser->buffer[8];
                
                if (calculated_crc == received_crc) {
                    parser->state = 4;
                    return true;
                } else {
                    parser->state = 0;
                }
            }
            break;
            
        case 4:
            parser->state = 0;
            break;
            
        default:
            parser->state = 0;
            break;
    }
    
    return false;
}

bool Protocol_ParserIsComplete(Protocol_ParserHandleTypeDef *parser)
{
    return (parser != NULL) && (parser->state == 4);
}

void Protocol_ParserReset(Protocol_ParserHandleTypeDef *parser)
{
    if (parser == NULL) return;
    parser->index = 0;
    parser->state = 0;
}