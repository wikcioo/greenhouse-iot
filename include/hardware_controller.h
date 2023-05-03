#pragma once

#include <ATMEGA_FreeRTOS.h>

typedef struct
{
    uint16_t temp;
    uint16_t hum;
    uint16_t co2;
} sensor_data_t;

void hc_handler_initialise(UBaseType_t measurement_priority);