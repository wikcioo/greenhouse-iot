#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>

#define BIT_0 (1 << 0)

typedef struct
{
    uint16_t temp;
    uint16_t hum;
    uint16_t co2;
} sensor_data_t;

void hc_handler_initialise(UBaseType_t measurement_priority, UBaseType_t toggle_priority);