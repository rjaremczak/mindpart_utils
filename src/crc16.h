#pragma once

#include <stdint.h>

struct crc16_proc {
    uint16_t crc;
    uint16_t temp;
} __attribute__((packed));

inline static void crc16_init(struct crc16_proc* calc) {
    calc->crc = 0;
}

void crc16_byte(struct crc16_proc* calc, uint8_t byte);