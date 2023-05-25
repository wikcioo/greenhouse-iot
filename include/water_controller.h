#pragma once

#include <stdbool.h>

void water_controller_init();
void water_controller_on();
void water_controller_off();
bool water_controller_is_water_valve_open();