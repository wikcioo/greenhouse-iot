extern "C"
{
#include "payload.h"
}

#include <gtest/gtest.h>

TEST(payload, UnpackingActions)
{
    // Packed payload in binary
    // Id         = ACTIONS    => 00_0100      (6 bits)
    // Actions    = water on   => 1000_0000    (8 bits)
    // Interval   = 15 minutes => 00_0000_1111 (10 bits)

    // Result bin = 0b0001_0010_0000_0000_0000_1111 (24 bits = 3 bytes)
    // Result hex = 0x12000F

    const char *hex_str = "12000F";
    action_t    actions = {};

    payload_unpack_actions(hex_str, &actions);

    EXPECT_EQ(actions.water_on, true);
    EXPECT_EQ(actions.interval, 15);
}