#pragma once

#include <ATMEGA_FreeRTOS.h>

#include "payload.h"

void scheduler_handler_initialise(UBaseType_t data_receive_priority, UBaseType_t scheduler_priority);
void scheduler_receive_data_handler_task(void *pvParameters);
void scheduler_schedule_events_handler_task(void *pvParameters);
void scheduler_receive_data_handler_task_run(void);
void scheduler_schedule_events_handler_task_run(void);