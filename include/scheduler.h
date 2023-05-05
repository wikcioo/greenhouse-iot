#pragma once

#include <ATMEGA_FreeRTOS.h>

#include "payload.h"

void scheduler_handler_initialise(UBaseType_t data_receive_priority, UBaseType_t scheduler_priority);