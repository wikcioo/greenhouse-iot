#pragma once
#include <stdbool.h>
#include <stdint.h>


bool     hum_temp_init();
bool     hum_temp_measure();
void     hum_temp_destroy();
uint16_t get_last_humidity_measurement();
uint16_t get_last_temperature_measurement();
