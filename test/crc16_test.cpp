#include <cstdint>
#include <stdint.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

extern "C" {
#include "crc16.h"
}

uint8_t data[] = {0x2a, 0xa2, 0xaf};
const uint16_t crc16_ccit = 0x7e1e;

TEST_CASE("calculation") {
    struct crc16 calc;
    crc16_init(&calc);
    for(int i = 0; i < sizeof(data); i++) crc16_update(&calc, data[i]);
    CHECK_EQ(calc.val, crc16_ccit);
}

TEST_CASE("verification") {
    struct crc16 calc;
    crc16_init(&calc);
    for(int i = 0; i < sizeof(data); i++) crc16_update(&calc, data[i]);
    CHECK_EQ(calc.val, crc16_ccit);
    crc16_update(&calc, crc16_ccit >> 8);
    crc16_update(&calc, crc16_ccit & 0xff);
    CHECK_EQ(calc.val, 0);
}
