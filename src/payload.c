#include "payload.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

payload_id_t payload_get_id(uint8_t *data)
{
    uint8_t id = (payload_id_t) ((data[0] >> 2) & 0x3F);
    if (id > ACTIONS)
        return INVALID;
    return id;
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

void payload_unpack_thc_presets(uint8_t *data, range_t *temp_range, range_t *hum_range, range_t *co2_range)
{
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

void payload_unpack_actions(uint8_t *data, action_t *actions)
{
    actions->water_on = (data[0] >> 1) & 1;

    actions->duration = 0;
    actions->duration |= (data[1] & 0x3) << 8;
    actions->duration |= data[2] & 0xFF;
}

void payload_unpack_intervals(uint8_t *data, uint8_t length, interval_t *intervals)
{
    uint8_t start_byte_ref[7] = {0, 3, 6, 9, 11, 14, 16};
    uint8_t offset_ref[7]     = {6, 4, 2, 0, 6, 4, 10};

    unsigned long tmp;
    uint16_t      start_time;
    uint16_t      end_time;

    if (length > 1)
    {
        for (uint8_t i = 0; i < ((length + 1) / 3); i++)
        {
            tmp =
                (((unsigned long) data[start_byte_ref[i]] << 24) | ((unsigned long) data[start_byte_ref[i] + 1] << 16) |
                 ((unsigned long) data[start_byte_ref[i] + 2] << 8) | data[start_byte_ref[i] + 3])
                << offset_ref[i];

            start_time = (tmp & 0xFFE00000) >> 21;
            end_time   = (tmp & 0x1FFC00) >> 10;

            intervals[i].start.hour   = (start_time >> 6) & 0x1F;
            intervals[i].start.minute = start_time & 0x3F;
            intervals[i].end.hour     = (end_time >> 6) & 0x1F;
            intervals[i].end.minute   = end_time & 0x3F;
        }
    }
}