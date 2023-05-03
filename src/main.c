#include <ATMEGA_FreeRTOS.h>
#include <avr/io.h>
#include <lora_driver.h>
#include <rc_servo.h>
#include <semphr.h>
#include <serial.h>
#include <status_leds.h>
#include <stdio.h>
#include <stdio_driver.h>
#include <task.h>

#include "co2.h"
#include "hardware_controller.h"
#include "humidity_temperature.h"
#include "lorawan.h"

MessageBufferHandle_t upLinkMessageBufferHandle;
MessageBufferHandle_t downLinkMessageBufferHandle;

void initialiseSystem()
{
    DDRA |= _BV(DDA0) | _BV(DDA7);

    stdio_initialise(ser_USART0);
    status_leds_initialise(5);

    downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t));
    upLinkMessageBufferHandle   = xMessageBufferCreate(20);

    if (hum_temp_init())
    {
        puts("HIH8120 driver initialized successfully");
    }
    else
    {
        puts("HIH8120 driver failed");
    }

    co2_init();

    rc_servo_initialise();
    hc_handler_initialise(4);

    lora_driver_initialise(1, downLinkMessageBufferHandle);
    lora_handler_initialise(3, 4);
}

int main(void)
{
    initialiseSystem();
    printf("Program Started!!\n");

    vTaskStartScheduler();

    while (1)
    {
    }
}