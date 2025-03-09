#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

extern "C" {
    # include "crc16.h"
}

TEST_CASE("crc16") {
    struct crc16_proc calc;
    crc16_init(&calc);
    crc16_byte(&calc, 0x2a);
    crc16_byte(&calc, 0xa2);
    crc16_byte(&calc, 0xaf);
    CHECK_EQ(calc.crc, 0x7e1e);
}
