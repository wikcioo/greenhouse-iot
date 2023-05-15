#pragma once

#include <ATMEGA_FreeRTOS.h>

void lora_handler_initialise(UBaseType_t uplink_priority, UBaseType_t downlink_priority);
void uplink_handler_task(void *pvParameters);
void downlink_handler_task(void *pvParameters);
void lora_setup(void);
void uplink_handler_task_run(void);