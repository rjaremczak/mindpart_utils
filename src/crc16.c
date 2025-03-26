#include "crc16.h"

void crc16_update(struct crc16* crc, uint8_t byte) {
    crc->tmp = ((crc->val >> 8) ^ byte) << 8;
    for(uint8_t i = 0; i < 8; i++) {
        if(crc->tmp & 0x8000)
            crc->tmp = (crc->tmp << 1) ^ 0x1021;
        else
            crc->tmp = crc->tmp << 1;
    }
    crc->val = crc->tmp ^ (crc->val << 8);
}