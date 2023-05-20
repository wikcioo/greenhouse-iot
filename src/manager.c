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
#include "initialiser.h"
#include "lorawan.h"
#include "payload.h"
#include "scheduler.h"
#include "water_controller.h"

MessageBufferHandle_t intervalDataMessageBufferHandle;
MessageBufferHandle_t upLinkMessageBufferHandle;
MessageBufferHandle_t downLinkMessageBufferHandle;
MessageBufferHandle_t presetDataMessageBufferHandle;
EventGroupHandle_t    xCreatedEventGroup;

void initialiseSystem()
{
    DDRA |= _BV(DDA0) | _BV(DDA7);

    stdio_initialise(ser_USART0);
    status_leds_initialise(5);

    intervalDataMessageBufferHandle = xMessageBufferCreate(sizeof(interval_t) * 2);
    downLinkMessageBufferHandle     = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);
    upLinkMessageBufferHandle       = xMessageBufferCreate(sizeof(sensor_data_t) * 2);
    presetDataMessageBufferHandle   = xMessageBufferCreate(sizeof(preset_data_t) * 2);
    xCreatedEventGroup              = xEventGroupCreate();

    if (hum_temp_init())
    {
        puts("HIH8120 driver initialized successfully");
    }
    else
    {
        puts("HIH8120 driver failed");
    }

    if (co2_init())
    {
        puts("MH_Z19 driver initialized successfully");
    }
    else
    {
        puts("MH_Z19 driver failed");
    }

    water_controller_init();
    hc_handler_initialise(3, 3);
    scheduler_handler_initialise(3, 4, configMAX_PRIORITIES);

    lora_driver_initialise(1, downLinkMessageBufferHandle);
    lora_handler_initialise(2, 2);
}