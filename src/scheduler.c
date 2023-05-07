#include "scheduler.h"

#include <message_buffer.h>
#include <stdio.h>
#include <string.h>
#include <timers.h>

#include "payload.h"
#include "water_controller.h"

#define MAX_INTERVALS 14

extern MessageBufferHandle_t intervalDataMessageBufferHandle;

typedef struct
{
    interval_t intervals[MAX_INTERVALS];
    uint8_t    current_size;
} interval_info_t;

static interval_info_t interval_info = {.intervals = {0}, .current_size = 0};
static time_point_t    daily_time    = {0, 0};

void scheduler_receive_data_handler_task(void *pvParameters);
void scheduler_schedule_events_handler_task(void *pvParameters);
void vTimerCallback(TimerHandle_t xTimer);

static bool _is_daily_time_in_interval_array();
static void _debug_print_intervals();

void scheduler_handler_initialise(UBaseType_t data_receive_priority, UBaseType_t scheduler_priority)
{
    /* ======= For testing purposes only ======= */
    daily_time.hour   = 5;
    daily_time.minute = 10;

    interval_t temp_interval1 = {.start = {.hour = 5, .minute = 11}, .end = {.hour = 5, .minute = 12}};
    interval_t temp_interval2 = {.start = {.hour = 5, .minute = 14}, .end = {.hour = 5, .minute = 16}};

    interval_info.intervals[interval_info.current_size++] = temp_interval1;
    interval_info.intervals[interval_info.current_size++] = temp_interval2;
    /* ========================================= */

    xTaskCreate(
        scheduler_receive_data_handler_task, "Scheduler Data Receiver", configMINIMAL_STACK_SIZE, NULL,
        data_receive_priority, NULL);
    xTaskCreate(
        scheduler_schedule_events_handler_task, "Scheduler Event Scheduler", configMINIMAL_STACK_SIZE, NULL,
        scheduler_priority, NULL);
}

void scheduler_receive_data_handler_task(void *pvParameters)
{
    for (;;)
    {
        interval_t data;
        xMessageBufferReceive(intervalDataMessageBufferHandle, &data, sizeof(interval_t), portMAX_DELAY);
        printf(
            "Received new interval {\n\tStart = %d:%d\n\tEnd = %d%d\n}\n", data.start.hour, data.start.minute,
            data.end.hour, data.end.minute);

        if (data.start.hour == 0 && data.start.minute == 0 && data.end.hour == 0 && data.end.minute == 0)
        {
            // Reset the intervals
            memset(interval_info.intervals, 0, interval_info.current_size);
            interval_info.current_size = 0;
        }

        interval_info.intervals[interval_info.current_size++] = data;
        _debug_print_intervals();
    }
}

static bool _is_daily_time_in_interval_array()
{
    for (uint8_t i = 0; i < interval_info.current_size; i++)
    {
        if (time_is_between(&daily_time, &interval_info.intervals[i].start, &interval_info.intervals[i].end))
            return true;
    }

    return false;
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

void scheduler_schedule_events_handler_task(void *pvParameters)
{
    TickType_t       xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency    = pdMS_TO_TICKS(1000UL);

    TimerHandle_t xTimer = xTimerCreate("MinuteTimer", pdMS_TO_TICKS(60000), pdTRUE, (void *) 0, vTimerCallback);
    xTimerStart(xTimer, 0);

    // TODO: Get rid of 2500 ms delay and make printf use mutex
    vTaskDelay(pdMS_TO_TICKS(2500));
    _debug_print_intervals();

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        if (_is_daily_time_in_interval_array())
        {
            if (!water_controller_get_state())
            {
                water_controller_on();
                printf("Scheduler opened water valve\n");
                printf("Valve state: %s\n", water_controller_get_state() ? "on" : "off");
            }
        }
        else
        {
            if (water_controller_get_state())
            {
                water_controller_off();
                printf("Scheduler closed water valve\n");
                printf("Valve state: %s\n", water_controller_get_state() ? "on" : "off");
            }
        }
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