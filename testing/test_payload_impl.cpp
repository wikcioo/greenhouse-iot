#include <gtest/gtest.h>

extern "C" {
	#include "payload.h"
}

TEST(payload, PackingLowerBounds)
{
    // Id THC_READINGS -> 000001

    uint8_t  flags       = 224;     // 1110 0000
    int16_t  temperature = 0;       // -50 C -> 000 0000 0000
    uint8_t  humidity    = 0;       // 000 0000
    uint16_t co2         = 0;       // 0000 0000 0000

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

TEST(payload, PackingNormalValues) {
	//Id THC_READINGS -> 000001

	uint8_t  flags = 224;     // 1110 0000
	int16_t temperature = 855; 	// 35.5 C -> 011 0101 0111
	uint8_t humidity = 45; 		// 010 1101
	uint16_t co2 = 1900;		// 0111 0110 1100

	payload_uplink_t payload_packed = payload_pack_thc(flags, temperature, humidity, co2);
	// Packed payload 00000111 10000001 10101011 10101101 01110110 11000000
    EXPECT_EQ(payload_packed.length, 6);
    EXPECT_EQ(payload_packed.data[0], 7);
    EXPECT_EQ(payload_packed.data[1], 129);
    EXPECT_EQ(payload_packed.data[2], 171);
    EXPECT_EQ(payload_packed.data[3], 173);
    EXPECT_EQ(payload_packed.data[4], 118);
    EXPECT_EQ(payload_packed.data[5], 192);
}

TEST(payload, PackingUpperBounds)
{
    // Id THC_READINGS -> 000001

    uint8_t  flags       = 224;   // 1110 0000
    int16_t  temperature = 1100;   // 60 C -> 100 0100 1100
    uint8_t  humidity    = 100;    // 110 0100
    uint16_t co2         = 4095;  // 1111 1111 1111

    payload_uplink_t payload_packed = payload_pack_thc(flags, temperature, humidity, co2);
    // Packed payload 00000111 10000010 00100110 01100100 11111111 11110000
    EXPECT_EQ(payload_packed.length, 6);
    EXPECT_EQ(payload_packed.data[0], 7);
    EXPECT_EQ(payload_packed.data[1], 130);
    EXPECT_EQ(payload_packed.data[2], 38);
    EXPECT_EQ(payload_packed.data[3], 100);
    EXPECT_EQ(payload_packed.data[4], 255);
    EXPECT_EQ(payload_packed.data[5], 240);
}

TEST(payload, Unpacking) {
    // Packed payload in binary v
    // Id THC_PRESETS  000 011 -> 6 bits
    // Temp ranges 
        // low = 15.5 -> 155 -> 655 -> 01 010 001 111 -> 11 bits
        // high = 45 -> 450 -> 950 ->  01 110 110 110 -> 11 bits
    // Hum ranges
        // low = 20 -> 0 010 100 -> 7 bits
        // high = 80 -> 1 010 000 -> 7 bits
    // Co2 ranges
        // low = 500 ->  000 111 110 100 -> 12 bits
        // high = 2000 -> 011 111 010 000 -> 12 bits

    // Concatenated 000 011 01 010 001 111 01 110 110 110 0 010 100 1 010 000 000 111 110 100 011 111 010 000 + 000 000 -> 72 bits -> 0D47BB629407D1F400

    char *hex_str = "0D47BB629407D1F400";
    range_t temp_range;
    range_t hum_range;
    range_t co2_range;

    payload_unpack_thc_presets(hex_str, &temp_range, &hum_range, &co2_range);

    EXPECT_EQ(payload_get_id(hex_str), 3);

    EXPECT_EQ(temp_range.low, 655);
    EXPECT_EQ(temp_range.high, 950);

    EXPECT_EQ(hum_range.low, 20);
    EXPECT_EQ(hum_range.high, 80);

    EXPECT_EQ(co2_range.low, 500);
    EXPECT_EQ(co2_range.high, 2000);
}