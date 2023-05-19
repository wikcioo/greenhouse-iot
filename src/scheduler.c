#include "scheduler.h"

#include <message_buffer.h>
#include <stdio.h>
#include <string.h>
#include <timers.h>

#include "payload.h"
#include "water_controller.h"

extern MessageBufferHandle_t intervalDataMessageBufferHandle;

interval_info_t interval_info = {.intervals = {0}, .current_size = 0};
time_point_t    daily_time    = {0, 0};

void vTimerCallback(TimerHandle_t xTimer);

static daily_time_interval_info_t _is_daily_time_in_interval_array();
static void                       _debug_print_intervals();

void scheduler_handler_initialise(UBaseType_t data_receive_priority, UBaseType_t scheduler_priority)
{
#ifndef TEST_ENV
    /* ======= For testing purposes only ======= */
    daily_time.hour   = 5;
    daily_time.minute = 10;

    interval_t temp_interval = {.start = {.hour = 5, .minute = 11}, .end = {.hour = 5, .minute = 12}};

    interval_info.intervals[interval_info.current_size++] = temp_interval;

    temp_interval.start.hour   = 5;
    temp_interval.start.minute = 14;
    temp_interval.end.hour     = 24;
    temp_interval.end.minute   = 0;

    interval_info.intervals[interval_info.current_size++] = temp_interval;
    /* ========================================= */
#endif

    xTaskCreate(
        scheduler_receive_data_handler_task, "Scheduler Data Receiver", configMINIMAL_STACK_SIZE, NULL,
        data_receive_priority, NULL);
    xTaskCreate(
        scheduler_schedule_events_handler_task, "Scheduler Event Scheduler", configMINIMAL_STACK_SIZE, NULL,
        scheduler_priority, NULL);
}

void scheduler_receive_data_handler_task_run(void)
{
    interval_t data;
    xMessageBufferReceive(intervalDataMessageBufferHandle, &data, sizeof(interval_t), portMAX_DELAY);
    printf("Received interval {%d:%d | %d:%d}\n", data.start.hour, data.start.minute, data.end.hour, data.end.minute);

    if (data.start.hour == 0 && data.start.minute == 0 && data.end.hour == 0 && data.end.minute == 0)
    {
        // Reset the intervals
        memset(interval_info.intervals, 0, interval_info.current_size);
        interval_info.current_size = 0;
    }
    else
    {
        interval_info.intervals[interval_info.current_size++] = data;
    }
}

void scheduler_receive_data_handler_task(void *pvParameters)
{
    for (;;)
    {
        scheduler_receive_data_handler_task_run();
    }
}

static daily_time_interval_info_t _is_daily_time_in_interval_array()
{
    for (uint8_t i = 0; i < interval_info.current_size; i++)
    {
        if (time_is_between(&daily_time, &interval_info.intervals[i].start, &interval_info.intervals[i].end))
            return (daily_time_interval_info_t){.index = i, .status = true};
    }

    return (daily_time_interval_info_t){.index = -1, .status = false};
}

static void _debug_print_intervals()
{
    printf("Intervals {\n\tcurrent_size: %u\n", interval_info.current_size);

    for (uint8_t i = 0; i < interval_info.current_size; i++)
    {
        printf(
            "\t[%u]: start = %u:%u | end = %u:%u\n", i, interval_info.intervals[i].start.hour,
            interval_info.intervals[i].start.minute, interval_info.intervals[i].end.hour,
            interval_info.intervals[i].end.minute);
    }

    puts("}");
}

static time_point_t _get_next_interval_start_after_daily_time()
{
    time_point_t next_interval_start;
    for (uint8_t i = 0; i < interval_info.current_size; i++)
    {
        if (time_is_before(&daily_time, &interval_info.intervals[i].start))
        {
            const uint16_t _hour   = interval_info.intervals[i].start.hour;
            const uint16_t _minute = interval_info.intervals[i].start.minute;
            next_interval_start    = (time_point_t){_hour, _minute};
            return next_interval_start;
        }
    }

    return (time_point_t){.hour = 24, .minute = 0};
}

void scheduler_schedule_events_handler_task_run(void)
{
    daily_time_interval_info_t info = _is_daily_time_in_interval_array();
    if (info.status)
    {
        if (!water_controller_get_state())
        {
            water_controller_on();
            printf("Scheduler opened water valve\n");
            printf("Valve state: %s\n", water_controller_get_state() ? "on" : "off");
        }

        time_point_t current_interval_end = interval_info.intervals[info.index].end;
        uint16_t     minutes_to_sleep     = time_get_diff_in_minutes(&daily_time, &current_interval_end);
        uint32_t     ms_to_sleep          = minutes_to_sleep * 60000;

        if (current_interval_end.hour == 24 && current_interval_end.minute == 0)
        {
            puts("Sleeping until midnight");
        }

        printf("Inside sleep %u minutes\n", minutes_to_sleep);

        TickType_t xLastWakeTime = xTaskGetTickCount();
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(ms_to_sleep));
    }
    else
    {
        if (water_controller_get_state())
        {
            water_controller_off();
            printf("Scheduler closed water valve\n");
            printf("Valve state: %s\n", water_controller_get_state() ? "on" : "off");
        }

        time_point_t next_interval_start = _get_next_interval_start_after_daily_time();
        uint16_t     minutes_to_sleep    = time_get_diff_in_minutes(&daily_time, &next_interval_start);
        uint32_t     ms_to_sleep         = minutes_to_sleep * 60000;

        if (next_interval_start.hour == 24 && next_interval_start.minute == 0)
        {
            puts("Sleeping until midnight");
        }

        printf("Outside sleep %u minutes\n", minutes_to_sleep);

        TickType_t xLastWakeTime = xTaskGetTickCount();
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(ms_to_sleep));
    }
}

void scheduler_schedule_events_handler_task(void *pvParameters)
{
    TimerHandle_t xTimer = xTimerCreate("MinuteTimer", pdMS_TO_TICKS(60000), pdTRUE, (void *) 0, vTimerCallback);
    xTimerStart(xTimer, 0);

    // TODO: Get rid of 2500 ms delay and make printf use mutex
    vTaskDelay(pdMS_TO_TICKS(2500));
    _debug_print_intervals();

    for (;;)
    {
        scheduler_schedule_events_handler_task_run();
    }
}

void vTimerCallback(TimerHandle_t xTimer)
{
    daily_time.minute++;

    if (daily_time.minute == 60)
    {
        daily_time.hour++;
        daily_time.minute = 0;
    }

    if (daily_time.hour == 24)
    {
        daily_time.hour = 0;
    }

    printf("Daily time incremented to {\n\tHour: %u\n\tMinute: %u\n}\n", daily_time.hour, daily_time.minute);
}