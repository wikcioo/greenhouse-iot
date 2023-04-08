#include "payload.h"
#include <stdlib.h>

#define ACTION_WATER_ON (1 << 7)

payload_id_t payload_get_id(uint8_t *data)
{
    return (payload_id_t)((data[0] >> 2) & 0x3F);
}

uint8_t *payload_pack_thc(uint8_t flags, int16_t temperature, uint8_t humidity, uint16_t co2)
{
    uint8_t *payload = (uint8_t *) calloc(6, sizeof(uint8_t));

    payload_id_t id = THC_READINGS;
    payload[0] |= ((uint8_t) id << 2);   // 1st byte += 6bits of ID
    payload[0] |= ((flags >> 6) & 0x3);  // 1st byte += 2bits of flags

    payload[1] |= (flags & 0x3F);              // 2nd byte += 6bits of flags
    payload[1] |= ((temperature >> 9) & 0x3);  // 2nd byte += 2bits of temperature

    payload[2] |= ((temperature >> 1) & 0xFF);  // 3rd byte += 8bits of temperature

    payload[3] |= (temperature & 0x1);  // 4th byte += 1bit of temperature
    payload[3] |= (humidity & 0x7F);    // 4th byte += 7bits of humidity

    payload[4] |= ((co2 >> 4) & 0xFF);  // 5th byte += 8bits of co2

    payload[5] |= (co2 & 0xF);  // 6th byte += 4bits of co2

    return payload;
}

// 1 = ID                          - 6 bits
// 2 = temperature range low       - 11 bits
// 3 = temperature range high      - 11 bits
// 4 = humidity range low          - 7 bits
// 5 = humidity range high         - 7 bits
// 6 = co2 range low               - 12 bits
// 7 = co2 range high              - 12 bits
// X = unused bits left at the end - 6 bits

// 1 1 1 1 1 1 2 2 | 2 2 2 2 2 2 2 2 | 2 3 3 3 3 3 3 3 | 3 3 3 3 4 4 4 4 | 4 4 4 5 5 5 5 5 | 5 5 6 6 6 6 6 6 |
// 6 6 6 6 6 6 7 7 | 7 7 7 7 7 7 7 7 | 7 7 X X X X X X|
void payload_unpack_thc_presets(uint8_t *data, range_t *temperature_range, range_t *humidity_range, range_t *co2_range)
{
    temperature_range->low = 0;
    temperature_range->low |= (data[0] & 0x3);
    temperature_range->low |= (data[1] & 0xFF);
    temperature_range->low |= ((data[2] >> 7) & 0x1);

    temperature_range->high = 0;
    temperature_range->high |= (data[2] & 0x7F);
    temperature_range->high |= ((data[3] >> 4) & 0xF);

    humidity_range->low = 0;
    humidity_range->low |= (data[3] & 0xF);
    humidity_range->low |= ((data[4] >> 5) & 0x7);

    humidity_range->high = 0;
    humidity_range->high |= (data[4] & 0x1F);
    humidity_range->high |= ((data[5] >> 6) & 0x3);

    co2_range->low = 0;
    co2_range->low |= (data[5] & 0x3F);
    co2_range->low |= ((data[6] >> 2) & 0x3F);

    co2_range->high = 0;
    co2_range->high |= (data[6] & 0x3);
    co2_range->high |= (data[7] & 0xFF);
    co2_range->high |= ((data[8] >> 6) & 0x3);
}

void payload_unpack_intervals(uint8_t *data, interval_t *intervals, uint8_t *no_intervals) {}

void payload_unpack_actions(uint8_t *data, action_t *actions)
{
    uint8_t action_byte = 0;

    action_byte |= (data[0] & 0x3);
    action_byte |= ((data[1] >> 2) & 0x3F);

    if (action_byte & ACTION_WATER_ON)
    {
        actions->water_on = true;
        // TODO: save the duration of water on
    }
}
