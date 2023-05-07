#include "time_point.h"

bool time_is_between(time_point_t *time, time_point_t *start, time_point_t *end)
{
    if (time->hour < start->hour || time->hour > end->hour)
        return false;

    if (time->hour == start->hour && time->minute < start->minute)
        return false;

    if (time->hour == end->hour && time->minute > end->minute)
        return false;

    return true;
}