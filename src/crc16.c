#include "crc16.h"

void crc16_calc(struct crc16_calc* calc, uint8_t byte) {
    calc->temp = ((calc->crc >> 8) ^ byte) << 8;
    for(uint8_t i = 0; i < 8; i++) {
        if(calc->temp & 0x8000)
            calc->temp = (calc->temp << 1) ^ 0x1021;
        else
            calc->temp = calc->temp << 1;
    }
    calc->crc = calc->temp ^ (calc->crc << 8);
}