#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>
#include <stdbool.h>

#include "payload.h"

#define BIT_0 (1 << 0)

typedef struct
{
    bool     is_water_valve_open;
    uint16_t temp;
    uint16_t hum;
    uint16_t co2;
} sensor_data_t;

typedef struct
{
    range_t *temp_range;
    range_t *hum_range;
    range_t *co2_range;
} preset_data_t;

void hc_receive_preset_data_handler_task(void *pvParameters);
void hc_toggle_handler_task(void *pvParameters);
void hc_handler_task(void *pvParameters);
void hc_handler_initialise(
    UBaseType_t preset_data_receive_priority, UBaseType_t measurement_priority, UBaseType_t toggle_priority);
void hc_receive_preset_data_handler_task_run(void);
void hc_toggle_handler_task_run(void);
void hc_handler_task_run(uint8_t counter);