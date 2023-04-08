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

#endif /* PAYLOAD_H_ */
