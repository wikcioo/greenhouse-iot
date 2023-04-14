#pragma once
#include <stdbool.h>
#include <stdint.h>


bool hum_temp_init();
bool hum_temp_measure(uint16_t *hume_buffer, uint16_t *temp_buffer);
void hum_temp_destroy();
