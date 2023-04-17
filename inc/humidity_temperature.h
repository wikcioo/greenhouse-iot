#pragma once
#include <stdbool.h>
#include <stdint.h>


bool     hum_temp_init();
bool     hum_temp_measure();
void     hum_temp_destroy();
uint16_t humidity_get_last_measurement();
uint16_t temperature_get_last_measurement();
