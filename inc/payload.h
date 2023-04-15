#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t *data;
    uint8_t  length;
} payload_uplink_t;

typedef enum
{
    THC_READINGS = 1,
    INTERVALS    = 2,
    THC_PRESETS  = 3,
    ACTIONS      = 4,
} payload_id_t;

payload_uplink_t payload_pack_thc(uint8_t flags, int16_t temperature, uint8_t humidity, uint16_t co2);
