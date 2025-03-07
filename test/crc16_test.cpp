#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

extern "C" {
    # include "../src/crc16.h"
}

TEST_CASE("crc16") {
    CHECK_EQ(0,0);
}
