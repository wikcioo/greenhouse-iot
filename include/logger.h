#pragma once

#include <stdio.h>

#ifdef ENABLE_DEBUG_PRINT
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG(format, ...)
#endif