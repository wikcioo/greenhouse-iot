#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t hour;
    uint16_t minute;
} time_point_t;

bool         time_is_before(time_point_t *before, time_point_t *after);
bool         time_is_between(time_point_t *time, time_point_t *start, time_point_t *end);
bool         time_equals(time_point_t *t1, time_point_t *t2);
uint16_t     time_get_diff_in_minutes(time_point_t *t1, time_point_t *t2);
time_point_t time_add_minutes(time_point_t time, uint16_t minutes);