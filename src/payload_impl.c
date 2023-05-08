#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "payload.h"

static uint8_t *hex_str_to_u8_ptr(const char *hex_str)
{
    size_t   length = strlen(hex_str);
    uint8_t *data   = (uint8_t *) calloc(length / 2, sizeof(uint8_t));

    for (size_t i = 0; i < length; i += 2)
    {
        char    hex_byte[3] = {hex_str[i], hex_str[i + 1], '\0'};
        uint8_t hex_value;
        sscanf(hex_byte, "%hhx", &hex_value);
        data[i / 2] = hex_value;
    }

    return data;
}

payload_id_t payload_get_id_u8_ptr(uint8_t *data)
{
    uint8_t id = (payload_id_t) ((data[0] >> 2) & 0x3F);
    if (id > ACTIONS)
        return INVALID;
    return id;
}

payload_id_t payload_get_id(const char *hex_str)
{
    uint8_t *data = hex_str_to_u8_ptr(hex_str);
    return (payload_id_t) ((data[0] >> 2) & 0x3F);
}

payload_uplink_t payload_pack_thc(uint8_t flags, int16_t temperature, uint16_t humidity, uint16_t co2)
{
    static uint8_t payload[6];
    memset(payload, 0, 6);

    payload_id_t id = THC_READINGS;
    payload[0] |= ((uint8_t) id << 2);   // 1st byte += 6bits of ID
    payload[0] |= ((flags >> 6) & 0x3);  // 1st byte += 2bits of flags

    payload[1] |= ((flags & 0x3F) << 2);       // 2nd byte += 6bits of flags
    payload[1] |= ((temperature >> 9) & 0x3);  // 2nd byte += 2bits of temperature

    payload[2] |= ((temperature >> 1) & 0xFF);  // 3rd byte += 8bits of temperature

    payload[3] |= ((temperature & 0x1) << 7);  // 4th byte += 1bit of temperature
    payload[3] |= ((humidity >> 3) & 0x7F);    // 4th byte += 7bits of humidity

    payload[4] |= ((humidity & 0x7) << 5);  // 5th byte += 3bits of humidity
    payload[4] |= ((co2 >> 8) & 0x1F);      // 5th byte += 5bits of co2

    payload[5] |= ((co2) &0xFF);  // 6th byte += 8bits of co2

    return (payload_uplink_t){payload, 6};
}

void payload_unpack_thc_presets(const char *hex_str, range_t *temp_range, range_t *hum_range, range_t *co2_range)
{
    uint8_t *data = hex_str_to_u8_ptr(hex_str);

    temp_range->low = 0;
    temp_range->low |= (data[0] & 0x3) << 9;
    temp_range->low |= (data[1] & 0xFF) << 1;
    temp_range->low |= (data[2] >> 7) & 0x1;

    temp_range->high = 0;
    temp_range->high |= (data[2] & 0x7F) << 4;
    temp_range->high |= (data[3] >> 4) & 0xF;

    hum_range->low = 0;
    hum_range->low |= (data[3] & 0xF) << 3;
    hum_range->low |= (data[4] >> 5) & 0x7;

    hum_range->high = 0;
    hum_range->high |= (data[4] & 0x1F) << 2;
    hum_range->high |= (data[5] >> 6) & 0x3;

    co2_range->low = 0;
    co2_range->low |= (data[5] & 0x3F) << 7;
    co2_range->low |= (data[6] >> 1) & 0x7F;

    co2_range->high = 0;
    co2_range->high |= (data[6] & 0x1) << 12;
    co2_range->high |= (data[7] & 0xFF) << 4;
    co2_range->high |= (data[8] >> 4) & 0xF;
}

void payload_unpack_actions(const char *hex_str, action_t *actions)
{
    uint8_t *data = hex_str_to_u8_ptr(hex_str);

    actions->water_on = (data[0] >> 1) & 1;

    actions->interval = 0;
    actions->interval |= (data[1] & 0x3) << 8;
    actions->interval |= data[2] & 0xFF;
}

void payload_unpack_actions_u8_ptr(uint8_t *data, action_t *actions)
{
    actions->water_on = (data[0] >> 1) & 1;

    actions->interval = 0;
    actions->interval |= (data[1] & 0x3) << 8;
    actions->interval |= data[2] & 0xFF;
}