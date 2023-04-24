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

payload_id_t payload_get_id(const char *hex_str)
{
    uint8_t *data = hex_str_to_u8_ptr(hex_str);
    return (payload_id_t) ((data[0] >> 2) & 0x3F);
}

payload_uplink_t payload_pack_thc(uint8_t flags, int16_t temperature, uint8_t humidity, uint16_t co2)
{
    static uint8_t payload[6];

    payload_id_t id = THC_READINGS;
    payload[0] |= ((uint8_t) id << 2);   // 1st byte += 6bits of ID
    payload[0] |= ((flags >> 6) & 0x3);  // 1st byte += 2bits of flags

    payload[1] |= ((flags & 0x3F) << 2);       // 2nd byte += 6bits of flags
    payload[1] |= ((temperature >> 9) & 0x3);  // 2nd byte += 2bits of temperature

    payload[2] |= ((temperature >> 1) & 0xFF);  // 3rd byte += 8bits of temperature

    payload[3] |= ((temperature & 0x1) << 7);  // 4th byte += 1bit of temperature
    payload[3] |= (humidity & 0x7F);           // 4th byte += 7bits of humidity

    payload[4] |= ((co2 >> 4) & 0xFF);  // 5th byte += 8bits of co2

    payload[5] |= (co2 & 0xF) << 4;  // 6th byte += 4bits of co2

    return (payload_uplink_t){payload, 6};
}