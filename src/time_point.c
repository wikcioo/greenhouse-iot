#include "time_point.h"

bool time_is_before(time_point_t *before, time_point_t *after)
{
    if (before->hour < after->hour)
        return true;

    if (before->hour == after->hour)
        return before->minute < after->minute;

    return false;
}

bool time_is_between(time_point_t *time, time_point_t *start, time_point_t *end)
{
    if (time->hour < start->hour || time->hour > end->hour)
        return false;

    if (time->hour == start->hour && time->minute < start->minute)
        return false;

    if (time->hour == end->hour && time->minute >= end->minute)
        return false;

    return true;
}

uint16_t time_get_diff_in_minutes(time_point_t *t1, time_point_t *t2)
{
    if (!time_is_before(t1, t2))
    {
        // swap t1 and t2
        time_point_t *temp = t1;

        t1 = t2;
        t2 = temp;
    }
    if (t1->hour == t2->hour)
    {
        return t2->minute - t1->minute;
    }

    return (60 - t1->minute) + ((t2->hour - (t1->hour + 1)) * 60) + t2->minute;
}
time_point_t time_get_time_from_minutes(uint16_t minutes_count)
{
    return (time_point_t){.hour = minutes_count / 60, .minute = minutes_count % 60};
}

time_point_t time_get_sum_of_time_with_minutes(time_point_t t1, uint16_t minutes_of_duration)
{
    time_point_t t2          = {.hour = minutes_of_duration / 60, .minute = minutes_of_duration % 60};
    uint8_t      sum_minutes = t1.minute + t2.minute;
    if (sum_minutes >= 60)
    {
        return (time_point_t){.hour = t1.hour + t2.hour + 1, .minute = sum_minutes - 60};
    }
    else
    {
        return (time_point_t){.hour = t1.hour + t2.hour, .minute = sum_minutes};
    }
}