#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "time_point.h"

#define PAYLOAD_ID_TO_NAME(x)          \
    (x == 1   ? "THC_READINGS"         \
     : x == 2 ? "INTERVALS_APPEND"     \
     : x == 3 ? "INTERVALS_CLS_APPEND" \
     : x == 4 ? "THC_PRESETS"          \
     : x == 5 ? "ACTIONS"              \
     : x == 6 ? "INVALID"              \
              : "UNKNOWN")

typedef enum
{
    THC_READINGS         = 1,
    INTERVALS_APPEND     = 2,
    INTERVALS_CLS_APPEND = 3,
    THC_PRESETS          = 4,
    ACTIONS              = 5,
    INVALID              = 6,
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

typedef struct
{
    time_point_t start;
    time_point_t end;
} interval_t;

payload_id_t     payload_get_id_u8_ptr(uint8_t *data);
payload_id_t     payload_get_id(const char *hex_str);
payload_uplink_t payload_pack_thc(uint8_t flags, int16_t temperature, uint16_t humidity, uint16_t co2);

void payload_unpack_thc_presets(const char *hex_str, range_t *temp_range, range_t *hum_range, range_t *co2_range);
void payload_unpack_thc_presets_u8_ptr(uint8_t *data, range_t *temp_range, range_t *hum_range, range_t *co2_range);
void payload_unpack_actions(const char *hex_str, action_t *actions);
void payload_unpack_actions_u8_ptr(uint8_t *data, action_t *actions);
void payload_unpack_intervals(uint8_t *data, uint8_t length, interval_t *intervals);