#include "manager.h"

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
#include "lorawan.h"
#include "water_controller.h"

MessageBufferHandle_t upLinkMessageBufferHandle;
MessageBufferHandle_t downLinkMessageBufferHandle;
EventGroupHandle_t    xCreatedEventGroup;

void initialiseManager()
{
    DDRA |= _BV(DDA0) | _BV(DDA7);

    stdio_initialise(ser_USART0);
    status_leds_initialise(5);

    downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);
    upLinkMessageBufferHandle   = xMessageBufferCreate(20);
    xCreatedEventGroup          = xEventGroupCreate();

    if (hum_temp_init())
    {
        puts("HIH8120 driver initialized successfully");
    }
    else
    {
        puts("HIH8120 driver failed");
    }

    co2_init();

    water_controller_init();
    hc_handler_initialise(4, 4);

    lora_driver_initialise(1, downLinkMessageBufferHandle);
    lora_handler_initialise(3, 3);
}