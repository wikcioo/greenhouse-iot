#pragma once

#include <stdbool.h>
#include <stdint.h>

bool     co2_init();
bool     co2_measure();
uint16_t co2_get_latest_measurement();