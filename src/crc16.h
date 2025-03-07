#pragma once

#include <stdint.h>

struct crc16_calc {
    uint16_t crc;
    uint16_t temp;
};

inline static void crc16_init(struct crc16_calc* calc) {
    calc->crc = 0;
}

void crc16_calc(struct crc16_calc* calc, uint8_t byte);