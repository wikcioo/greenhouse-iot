extern "C"
{
#include "payload.h"
}

#include <gtest/gtest.h>

TEST(payload, UnpackingActions)
{
    // Packed payload in binary
    // Id         = ACTIONS    => 00_0101      (6 bits)
    // Actions    = water on   => 1000_0000    (8 bits)
    // Interval   = 15 minutes => 00_0000_1111 (10 bits)

    // Result bin = 0b0001_0110_0000_0000_0000_1111 (24 bits = 3 bytes)
    // Result hex = 0x16000F

    const char *hex_str = "16000F";
    action_t    actions = {};

    payload_unpack_actions(hex_str, &actions);
    EXPECT_EQ(payload_get_id(hex_str), ACTIONS);

    EXPECT_EQ(actions.water_on, true);
    EXPECT_EQ(actions.interval, 15);
}

TEST(payload, PackingLowerBounds)
{
    // Id THC_READINGS -> 000001

    uint8_t  flags       = 224;  // 1110 0000
    int16_t  temperature = 0;    // -50 C -> 000 0000 0000
    uint16_t humidity    = 0;    // 00 0000 0000
    uint16_t co2         = 0;    // 0 0000 0000 0000

    payload_uplink_t payload_packed = payload_pack_thc(flags, temperature, humidity, co2);
    // Packed payload 00000111 10000000 00000000 00000000 00000000 00000000
    EXPECT_EQ(payload_packed.length, 6);
    EXPECT_EQ(payload_packed.data[0], 7);
    EXPECT_EQ(payload_packed.data[1], 128);
    EXPECT_EQ(payload_packed.data[2], 0);
    EXPECT_EQ(payload_packed.data[3], 0);
    EXPECT_EQ(payload_packed.data[4], 0);
    EXPECT_EQ(payload_packed.data[5], 0);
}

TEST(payload, PackingNormalValues1)
{
    // Id THC_READINGS -> 000001

    uint8_t  flags       = 224;   // 1110 0000
    int16_t  temperature = 855;   // 35.5 C -> 011 0101 0111
    uint16_t humidity    = 450;   // 01 1100 0010
    uint16_t co2         = 1900;  // 0 0111 0110 1100

    payload_uplink_t payload_packed = payload_pack_thc(flags, temperature, humidity, co2);
    // Packed payload 00000111 10000001 10101011 10111000 01000111 01101100
    EXPECT_EQ(payload_packed.length, 6);
    EXPECT_EQ(payload_packed.data[0], 7);
    EXPECT_EQ(payload_packed.data[1], 129);
    EXPECT_EQ(payload_packed.data[2], 171);
    EXPECT_EQ(payload_packed.data[3], 184);
    EXPECT_EQ(payload_packed.data[4], 71);
    EXPECT_EQ(payload_packed.data[5], 108);
}

TEST(payload, PackingNormalValues2)
{
    // Id THC_READINGS -> 000001

    uint8_t  flags       = 224;   // 1110 0000
    int16_t  temperature = 855;   // 35.5 C -> 011 0101 0111
    uint16_t humidity    = 455;   // 01 1100 0111
    uint16_t co2         = 1900;  // 0 0111 0110 1100

    payload_uplink_t payload_packed = payload_pack_thc(flags, temperature, humidity, co2);
    // Packed payload 00000111 10000001 10101011 10111000 11100111 01101100
    EXPECT_EQ(payload_packed.length, 6);
    EXPECT_EQ(payload_packed.data[0], 7);
    EXPECT_EQ(payload_packed.data[1], 129);
    EXPECT_EQ(payload_packed.data[2], 171);
    EXPECT_EQ(payload_packed.data[3], 184);
    EXPECT_EQ(payload_packed.data[4], 231);
    EXPECT_EQ(payload_packed.data[5], 108);
}

TEST(payload, PackingUpperBounds)
{
    // Id THC_READINGS -> 000001

    uint8_t  flags       = 224;   // 1110 0000
    int16_t  temperature = 1100;  // 60 C -> 100 0100 1100
    uint16_t humidity    = 1000;  // 11 1110 1000
    uint16_t co2         = 8191;  // 1 1111 1111 1111

    payload_uplink_t payload_packed = payload_pack_thc(flags, temperature, humidity, co2);
    // Packed payload 00000111 10000010 00100110 01111101 00011111 11111111
    EXPECT_EQ(payload_packed.length, 6);
    EXPECT_EQ(payload_packed.data[0], 7);
    EXPECT_EQ(payload_packed.data[1], 130);
    EXPECT_EQ(payload_packed.data[2], 38);
    EXPECT_EQ(payload_packed.data[3], 125);
    EXPECT_EQ(payload_packed.data[4], 31);
    EXPECT_EQ(payload_packed.data[5], 255);
}

TEST(payload, UnpackingLowerBounds)
{
    // Packed payload in binary v
    // Id THC_PRESETS  000 100 -> 6 bits
    // Temp ranges
    // low = -50 -> -500 -> 0 -> 00 000 000 000 -> 11 bits
    // high = -50 -> -500 -> 0 ->  00 000 000 000 -> 11 bits
    // Hum ranges
    // low = 0 -> 0 000 000 -> 7 bits
    // high = 0 -> 0 000 000 -> 7 bits
    // Co2 ranges
    // low = 0 -> 0 000 000 000 000 -> 13 bits
    // high = 0 -> 0 000 000 000 000 -> 13 bits

    // Concatenated 000 100 00 000 000 000 00 000 000 000 0 000 000 0 000 000 000 000 000 000 000 000 000 000 000000
    // -> 72 bits -> 100000000000000000

    const char *hex_str = "100000000000000000";
    range_t     temp_range;
    range_t     hum_range;
    range_t     co2_range;

    payload_unpack_thc_presets(hex_str, &temp_range, &hum_range, &co2_range);


    EXPECT_EQ(payload_get_id(hex_str), THC_PRESETS);

    EXPECT_EQ(temp_range.low, 0);
    EXPECT_EQ(temp_range.high, 0);

    EXPECT_EQ(hum_range.low, 0);
    EXPECT_EQ(hum_range.high, 0);

    EXPECT_EQ(co2_range.low, 0);
    EXPECT_EQ(co2_range.high, 0);
}

TEST(payload, UnpackingNormalValues)
{
    // Packed payload in binary v
    // Id THC_PRESETS  000 100 -> 6 bits
    // Temp ranges
    // low = 15.5 -> 155 -> 655 -> 01 010 001 111 -> 11 bits
    // high = 45 -> 450 -> 950 ->  01 110 110 110 -> 11 bits
    // Hum ranges
    // low = 20 -> 0 010 100 -> 7 bits
    // high = 80 -> 1 010 000 -> 7 bits
    // Co2 ranges
    // low = 500 ->  0 000 111 110 100 -> 13 bits
    // high = 2000 -> 0 011 111 010 000 -> 13 bits

    // Concatenated 000 100 01 010 001 111 01 110 110 110 0 010 100 1 010 000 0 000 111 110 100 0 011 111 010 000 + 0000
    // -> 72 bits -> 1147BB629403E87D00

    const char *hex_str = "1147BB629403E87D00";
    range_t     temp_range;
    range_t     hum_range;
    range_t     co2_range;

    payload_unpack_thc_presets(hex_str, &temp_range, &hum_range, &co2_range);

    EXPECT_EQ(payload_get_id(hex_str), THC_PRESETS);

    EXPECT_EQ(temp_range.low, 655);
    EXPECT_EQ(temp_range.high, 950);

    EXPECT_EQ(hum_range.low, 20);
    EXPECT_EQ(hum_range.high, 80);

    EXPECT_EQ(co2_range.low, 500);
    EXPECT_EQ(co2_range.high, 2000);
}

TEST(payload, UnpackingNormalValues2)
{
    // Packed payload in binary v
    // Id THC_PRESETS  000 100 -> 6 bits
    // Temp ranges
    // low = 0.5 -> 5 -> 505 -> 00 111 111 001 -> 11 bits
    // high = 99.5 -> 995 -> 1495 ->  10 111 010 111 -> 11 bits
    // Hum ranges
    // low = 5 -> 0 000 101 -> 7 bits
    // high = 95 -> 1 011 111 -> 7 bits
    // Co2 ranges
    // low = 5 ->  0 000 000 000 101 -> 13 bits
    // high = 8190 -> 1 111 111 111 110 -> 13 bits

    // Concatenated 000 100 00 111 111 001 10 111 010 111 0 000 101 1 011 111 0 000 000 000 101 1 111 111 111 110 + 0000
    // -> 72 bits -> 10FCDD70B7C00BFFE0

    const char *hex_str = "10FCDD70B7C00BFFE0";
    range_t     temp_range;
    range_t     hum_range;
    range_t     co2_range;

    payload_unpack_thc_presets(hex_str, &temp_range, &hum_range, &co2_range);

    EXPECT_EQ(payload_get_id(hex_str), THC_PRESETS);

    EXPECT_EQ(temp_range.low, 505);
    EXPECT_EQ(temp_range.high, 1495);

    EXPECT_EQ(hum_range.low, 5);
    EXPECT_EQ(hum_range.high, 95);

    EXPECT_EQ(co2_range.low, 5);
    EXPECT_EQ(co2_range.high, 8190);
}

TEST(payload, UnpackingUpperBounds)
{
    // Packed payload in binary v
    // Id THC_PRESETS  000 100 -> 6 bits
    // Temp ranges
    // low = 60 -> 600 -> 1100 -> 10 001 001 100 -> 11 bits
    // high = 60 -> 600 -> 1100 ->  10 001 001 100 -> 11 bits
    // Hum ranges
    // low = 100 -> 1 100 100 -> 7 bits
    // high = 100 -> 1 100 100 -> 7 bits
    // Co2 ranges
    // low = 8191 ->  1 111 111 111 111 -> 13 bits
    // high = 8191 -> 1 111 111 111 111 -> 13 bits

    // Concatenated 000 100 10 001 001 100 10 001 001 100 1 100 100 1 100 100 1 111 111 111 111 1 111 111 111 111 + 0000
    // -> 72 bits -> 122644CC993FFFFFF0

    const char *hex_str = "122644CC993FFFFFF0";
    range_t     temp_range;
    range_t     hum_range;
    range_t     co2_range;

    payload_unpack_thc_presets(hex_str, &temp_range, &hum_range, &co2_range);

    EXPECT_EQ(payload_get_id(hex_str), THC_PRESETS);

    EXPECT_EQ(temp_range.low, 1100);
    EXPECT_EQ(temp_range.high, 1100);

    EXPECT_EQ(hum_range.low, 100);
    EXPECT_EQ(hum_range.high, 100);

    EXPECT_EQ(co2_range.low, 8191);
    EXPECT_EQ(co2_range.high, 8191);
}

TEST(payload, UnpackingIntervals0)
{
    // 0x08 -> 0b00001000 -> ID: 2
    uint8_t    data[]       = {0x8};
    uint8_t    length       = 1;
    interval_t intervals[7] = {0};

    EXPECT_EQ(payload_get_id_u8_ptr(data), INTERVALS_APPEND);
    payload_unpack_intervals(data, length, intervals);

    EXPECT_EQ(intervals[0].start.hour, 0);
    EXPECT_EQ(intervals[0].start.minute, 0);
    EXPECT_EQ(intervals[0].end.hour, 0);
    EXPECT_EQ(intervals[0].end.minute, 0);
}

TEST(payload, UnpackingIntervals1)
{
    // ID: 2
    // [0] -> Start: 12:37 -> End: 13:27
    uint8_t    data[]       = {0x9, 0x92, 0xB5, 0xB0};
    uint8_t    length       = 7;
    interval_t intervals[7] = {0};

    EXPECT_EQ(payload_get_id_u8_ptr(data), INTERVALS_APPEND);
    payload_unpack_intervals(data, length, intervals);

    EXPECT_EQ(intervals[0].start.hour, 12);
    EXPECT_EQ(intervals[0].start.minute, 37);
    EXPECT_EQ(intervals[0].end.hour, 13);
    EXPECT_EQ(intervals[0].end.minute, 27);
}

TEST(payload, UnpackingIntervals7)
{
    // ID: 2
    uint8_t    data[]       = {0x0A, 0x4F, 0x4C, 0x2A, 0x99, 0x5B, 0xD7, 0x1A, 0xE8, 0x60,
                               0x0C, 0x29, 0xA6, 0xB4, 0xF7, 0x83, 0x05, 0x22, 0x64, 0xB6};
    uint8_t    length       = 20;
    interval_t intervals[7] = {0};

    EXPECT_EQ(payload_get_id_u8_ptr(data), INTERVALS_APPEND);
    payload_unpack_intervals(data, length, intervals);

    //  [0]: 18:30 -> 19:02
    EXPECT_EQ(intervals[0].start.hour, 18);
    EXPECT_EQ(intervals[0].start.minute, 30);
    EXPECT_EQ(intervals[0].end.hour, 19);
    EXPECT_EQ(intervals[0].end.minute, 2);

    //  [1]: 21:12 -> 21:47
    EXPECT_EQ(intervals[1].start.hour, 21);
    EXPECT_EQ(intervals[1].start.minute, 12);
    EXPECT_EQ(intervals[1].end.hour, 21);
    EXPECT_EQ(intervals[1].end.minute, 47);

    //  [2]: 11:35 -> 11:40
    EXPECT_EQ(intervals[2].start.hour, 11);
    EXPECT_EQ(intervals[2].start.minute, 35);
    EXPECT_EQ(intervals[2].end.hour, 11);
    EXPECT_EQ(intervals[2].end.minute, 40);

    //  [3]: 12:00 -> 12:10
    EXPECT_EQ(intervals[3].start.hour, 12);
    EXPECT_EQ(intervals[3].start.minute, 0);
    EXPECT_EQ(intervals[3].end.hour, 12);
    EXPECT_EQ(intervals[3].end.minute, 10);

    //  [4]: 13:13 -> 13:15
    EXPECT_EQ(intervals[4].start.hour, 13);
    EXPECT_EQ(intervals[4].start.minute, 13);
    EXPECT_EQ(intervals[4].end.hour, 13);
    EXPECT_EQ(intervals[4].end.minute, 15);

    //  [5]: 15:01 -> 16:20
    EXPECT_EQ(intervals[5].start.hour, 15);
    EXPECT_EQ(intervals[5].start.minute, 1);
    EXPECT_EQ(intervals[5].end.hour, 16);
    EXPECT_EQ(intervals[5].end.minute, 20);

    //  [6]: 17:12 -> 18:54
    EXPECT_EQ(intervals[6].start.hour, 17);
    EXPECT_EQ(intervals[6].start.minute, 12);
    EXPECT_EQ(intervals[6].end.hour, 18);
    EXPECT_EQ(intervals[6].end.minute, 54);
}

TEST(payload, UnpackingMaxIntervals)
{
    // ID: 2
    uint8_t    data[]       = {0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                               0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t    length       = 20;
    interval_t intervals[7] = {0};

    payload_unpack_intervals(data, length, intervals);
    EXPECT_EQ(payload_get_id_u8_ptr(data), INTERVALS_APPEND);

    // [0]: 31:63 -> 31:61
    EXPECT_EQ(intervals[0].start.hour, 31);
    EXPECT_EQ(intervals[0].start.minute, 63);
    EXPECT_EQ(intervals[0].end.hour, 31);
    EXPECT_EQ(intervals[0].end.minute, 63);

    // [1]: 31:63 -> 31:61
    EXPECT_EQ(intervals[1].start.hour, 31);
    EXPECT_EQ(intervals[1].start.minute, 63);
    EXPECT_EQ(intervals[1].end.hour, 31);
    EXPECT_EQ(intervals[1].end.minute, 63);

    // [2]: 31:63 -> 31:61
    EXPECT_EQ(intervals[2].start.hour, 31);
    EXPECT_EQ(intervals[2].start.minute, 63);
    EXPECT_EQ(intervals[2].end.hour, 31);
    EXPECT_EQ(intervals[2].end.minute, 63);

    // [3]: 31:63 -> 31:61
    EXPECT_EQ(intervals[3].start.hour, 31);
    EXPECT_EQ(intervals[3].start.minute, 63);
    EXPECT_EQ(intervals[3].end.hour, 31);
    EXPECT_EQ(intervals[3].end.minute, 63);

    // [4]: 31:63 -> 31:61
    EXPECT_EQ(intervals[4].start.hour, 31);
    EXPECT_EQ(intervals[4].start.minute, 63);
    EXPECT_EQ(intervals[4].end.hour, 31);
    EXPECT_EQ(intervals[4].end.minute, 63);

    // [5]: 31:63 -> 31:61
    EXPECT_EQ(intervals[5].start.hour, 31);
    EXPECT_EQ(intervals[5].start.minute, 63);
    EXPECT_EQ(intervals[5].end.hour, 31);
    EXPECT_EQ(intervals[5].end.minute, 63);

    // [6]: 31:63 -> 31:61
    EXPECT_EQ(intervals[6].start.hour, 31);
    EXPECT_EQ(intervals[6].start.minute, 63);
    EXPECT_EQ(intervals[6].end.hour, 31);
    EXPECT_EQ(intervals[6].end.minute, 63);
}

TEST(payload, UnpackingEmptyIntervals)
{
    // ID: 2
    uint8_t    data[]       = {0x08, 0x00, 0x00, 0x00};
    uint8_t    length       = 4;
    interval_t intervals[7] = {0};

    /*
        The objective of this test is to verify that
        method correctly unpacks values (even if the
        data is just zeroes), therefore, index 0 of
        intervals will be populated prior to unpacking
        with data different from 0.
    */

    intervals[0].start.hour   = 10;
    intervals[0].start.minute = 11;
    intervals[0].end.hour     = 12;
    intervals[0].end.minute   = 13;

    payload_unpack_intervals(data, length, intervals);
    EXPECT_EQ(payload_get_id_u8_ptr(data), INTERVALS_APPEND);

    // [0]: 0:00 -> 0:00
    EXPECT_EQ(intervals[0].start.hour, 0);
    EXPECT_EQ(intervals[0].start.minute, 0);
    EXPECT_EQ(intervals[0].end.hour, 0);
    EXPECT_EQ(intervals[0].end.minute, 0);
}

TEST(payload, GettingIdOfPayloadForIntervalsWithClearAndAppend)
{
    uint8_t data[] = {0x0C};
    EXPECT_EQ(payload_get_id_u8_ptr(data), INTERVALS_CLS_APPEND);
}