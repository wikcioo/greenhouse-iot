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

bool time_equals(time_point_t *t1, time_point_t *t2)
{
    return t1->hour == t2->hour && t1->minute == t2->minute;
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

time_point_t time_add_minutes(time_point_t time, uint16_t minutes)
{
    time.hour += minutes / 60;
    time.minute += minutes % 60;

    if (time.minute >= 60)
    {
        time.hour++;
        time.minute = time.minute % 60;
    }

    return time;
}