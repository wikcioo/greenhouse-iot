#include "payload.h"
#include <stdlib.h>

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


void payload_unpack_thc_presets(uint8_t *data, range_t *temperature_range, range_t *humidity_range, range_t *co2_range)
{
}

void payload_unpack_intervals(uint8_t *data, interval_t *intervals, uint8_t *no_intervals) {}

void payload_unpack_actions(uint8_t *data, action_t *actions) {}
