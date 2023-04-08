#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    THC_READINGS = 1,
    INTERVALS    = 2,
    THC_PRESETS  = 3,
    ACTIONS      = 4,
} payload_id_t;

typedef struct
{
    bool     water_on;
    uint32_t water_on_duration_in_sec;
} action_t;

typedef struct
{
    uint8_t start;
    uint8_t end;
} interval_t;

typedef struct
{
    int32_t low;
    int32_t high;
} range_t;

payload_id_t payload_get_id(uint8_t *data);
uint8_t *    payload_pack_thc(uint8_t flags, int16_t temperature, uint8_t humidity, uint16_t co2);

void payload_unpack_thc_presets(uint8_t *data, range_t *temperature_range, range_t *humidity_range, range_t *co2_range);
void payload_unpack_intervals(uint8_t *data, interval_t *intervals, uint8_t *no_intervals);
void payload_unpack_actions(uint8_t *data, action_t *actions);

#endif /* PAYLOAD_H_ */
