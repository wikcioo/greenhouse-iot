#include "scheduler.h"

#include <message_buffer.h>
#include <stdio.h>

#include "payload.h"

extern MessageBufferHandle_t intervalDataMessageBufferHandle;

static volatile time_point_t daily_time = {0, 0};
static interval_t            intervals[7];

void scheduler_receive_data_handler_task(void *pvParameters);
void scheduler_schedule_events_handler_task(void *pvParameters);

void scheduler_handler_initialise(UBaseType_t data_receive_priority, UBaseType_t scheduler_priority)
{
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
            "Received new interval\n\tStart = %d:%d\n\tEnd = %d%d\n", data.start.hour, data.start.second, data.end.hour,
            data.end.second);
    }
}

void scheduler_schedule_events_handler_task(void *pvParameters)
{
    TickType_t       xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency    = pdMS_TO_TICKS(1000UL);

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void vApplicationTickHook(void)
{
    static uint32_t ticks = 0;

    ticks++;

    if (ticks == configTICK_RATE_HZ)
    {
        ticks = 0;
        daily_time.second++;
        if (daily_time.second == 60)
        {
            daily_time.hour++;
            daily_time.second = 0;
        }
    }
}