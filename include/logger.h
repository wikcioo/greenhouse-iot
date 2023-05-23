#pragma once

#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <stdio.h>

#ifdef ENABLE_DEBUG_PRINT
extern SemaphoreHandle_t loggerMutex;
#define LOG(format, ...)               \
    do                                 \
    {                                  \
        printf(format, ##__VA_ARGS__); \
    } while (0)
#else
#define LOG(format, ...)
#endif