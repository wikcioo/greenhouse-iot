#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t hour;
    uint16_t minute;
} time_point_t;

bool time_is_between(time_point_t *time, time_point_t *start, time_point_t *end);