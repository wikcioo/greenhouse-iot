#include "initialiser.h"

#include <avr/io.h>
#include <lora_driver.h>
#include <message_buffer.h>
#include <serial.h>
#include <status_leds.h>
#include <stdio.h>
#include <stdio_driver.h>

#include "co2.h"
#include "hardware_controller.h"
#include "humidity_temperature.h"
#include "logger.h"
#include "lorawan.h"
#include "payload.h"
#include "scheduler.h"
#include "water_controller.h"

MessageBufferHandle_t intervalDataMessageBufferHandle;
MessageBufferHandle_t upLinkMessageBufferHandle;
MessageBufferHandle_t downLinkMessageBufferHandle;
MessageBufferHandle_t presetDataMessageBufferHandle;
MessageBufferHandle_t actionDataMessageBufferHandle;

#ifdef ENABLE_DEBUG_PRINT
SemaphoreHandle_t loggerMutex;
#endif

void initialiseSystem()
{
    DDRA |= _BV(DDA0) | _BV(DDA7);

    stdio_initialise(ser_USART0);
    status_leds_initialise(5);

    intervalDataMessageBufferHandle = xMessageBufferCreate(sizeof(interval_t) * 2);
    downLinkMessageBufferHandle     = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);
    upLinkMessageBufferHandle       = xMessageBufferCreate(sizeof(sensor_data_t) * 2);
    presetDataMessageBufferHandle   = xMessageBufferCreate(sizeof(preset_data_t) * 2);
    actionDataMessageBufferHandle   = xMessageBufferCreate(sizeof(action_t) * 2);

#ifdef ENABLE_DEBUG_PRINT
    loggerMutex = xSemaphoreCreateMutex();
#endif

    if (hum_temp_init())
    {
        LOG("HIH8120 driver initialized successfully\n");
    }
    else
    {
        LOG("HIH8120 driver failed\n");
    }

    if (co2_init())
    {
        LOG("MH_Z19 driver initialized successfully\n");
    }
    else
    {
        LOG("MH_Z19 driver failed\n");
    }

    water_controller_init();
    hc_handler_initialise(3, 3, 3);
    scheduler_handler_initialise(4, 4);

    lora_driver_initialise(1, downLinkMessageBufferHandle);
    lora_handler_initialise(3, 3);
}