#pragma once

#include <stdint.h>

struct crc16 {
    uint16_t val;
    uint16_t tmp;
} __attribute__((packed));

inline static void crc16_init(struct crc16* crc) {
    crc->val = 0;
}

void crc16_update(struct crc16* calc, uint8_t byte);