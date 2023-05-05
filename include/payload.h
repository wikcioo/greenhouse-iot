#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    THC_READINGS = 1,
    INTERVALS    = 2,
    THC_PRESETS  = 3,
    ACTIONS      = 4,
    INVALID      = 5,
} payload_id_t;

typedef struct
{
    uint8_t *data;
    uint8_t  length;
} payload_uplink_t;

typedef struct
{
    uint16_t low;
    uint16_t high;
} range_t;

typedef struct
{
    bool     water_on;
    uint16_t interval;
} action_t;

payload_id_t     payload_get_id(const char *hex_str);
payload_uplink_t payload_pack_thc(uint8_t flags, int16_t temperature, uint16_t humidity, uint16_t co2);

void payload_unpack_thc_presets(const char *hex_str, range_t *temp_range, range_t *hum_range, range_t *co2_range);
void payload_unpack_actions(const char *hex_str, action_t *actions);