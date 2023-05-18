#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>
#include <stdbool.h>
#include "payload.h"

#define MAX_INTERVALS 7
#define BIT_0 (1 << 0)

typedef struct
{
    interval_t intervals[MAX_INTERVALS];
    uint8_t    current_size;
} interval_info_t;

typedef struct
{
    bool    status;
    uint8_t index;
} daily_time_interval_info_t;

void scheduler_handler_initialise(
    UBaseType_t data_receive_priority, UBaseType_t scheduler_priority, UBaseType_t toggle_priority);
void scheduler_receive_data_handler_task(void *pvParameters);
void scheduler_schedule_events_handler_task(void *pvParameters);
void scheduler_receive_data_handler_task_run(void);
void scheduler_schedule_events_handler_task_run(void);
// Manuall toggling
void scheduler_manual_toggling_handler_task_run(void);
void scheduler_manual_toggling_handler_task(void *pvParameters);
