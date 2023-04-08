#include "payload.h"
#include <stdlib.h>

payload_id_t payload_get_id(uint8_t *data)
{
    return (payload_id_t)((data[0] >> 2) & 0x3F);
}

uint8_t *payload_pack_thc(uint8_t flags, int16_t temperature, uint8_t humidity, uint16_t co2)
{
    return NULL;
}

void payload_unpack_thc_presets(uint8_t *data, range_t *temperature_range, range_t *humidity_range, range_t *co2_range)
{
}

void payload_unpack_intervals(uint8_t *data, interval_t *intervals, uint8_t *no_intervals) {}

void payload_unpack_actions(uint8_t *data, action_t *actions) {}
