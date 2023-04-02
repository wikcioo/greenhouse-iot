#pragma once
#include <stdint.h>
#include <stdbool.h>


bool hum_temp_init();
void hum_measure(uint16_t * buffer);
void temp_measure(uint16_t * buffer);
void hum_temp_destroy();