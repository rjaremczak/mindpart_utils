#include "greatest.h"
#include "crc16.h"

uint8_t data[] = {0x2a, 0xa2, 0xaf};
const uint16_t crc16_ccit = 0x7e1e;

TEST calculation() {
    struct crc16 calc;
    crc16_init(&calc);
    for(int i = 0; i < sizeof(data); i++) crc16_update(&calc, data[i]);
    ASSERT_EQ(calc.val, crc16_ccit);
    PASS();
}

TEST verification() {
    struct crc16 calc;
    crc16_init(&calc);
    for(int i = 0; i < sizeof(data); i++) crc16_update(&calc, data[i]);
    ASSERT_EQ(calc.val, crc16_ccit);
    crc16_update(&calc, crc16_ccit >> 8);
    crc16_update(&calc, crc16_ccit & 0xff);
    ASSERT_EQ(calc.val, 0);
    PASS();
}

SUITE(crc16) {
    RUN_TEST(calculation);
    RUN_TEST(verification);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(crc16);
    GREATEST_MAIN_END();
}
