/*
 * main.c
 * Author : IHA
 *
 * Example main file including LoRaWAN setup
 * Just for inspiration :)
 */

#include <ATMEGA_FreeRTOS.h>
#include <avr/io.h>
#include <semphr.h>
#include <serial.h>
#include <stddef.h>
#include <stdio.h>
#include <stdio_driver.h>
#include <task.h>

#include <ATMEGA_FreeRTOS.h>

#include <lora_driver.h>
#include <status_leds.h>
#include "co2.h"
#include "humidity_temperature.h"
#include "lorawan.h"
#include "payload.h"

// Needed for LoRaWAN
#include <lora_driver.h>
#include <status_leds.h>

// define two Tasks
void task1(void *pvParameters);
void task2(void *pvParameters);

// define semaphore handle
SemaphoreHandle_t xTestSemaphore;
// static lora_driver_payload_t _uplink_payload;
// Prototype for LoRaWAN handler
void lora_handler_initialise(UBaseType_t lora_handler_task_priority);

/*-----------------------------------------------------------*/
void create_tasks_and_semaphores(void)
{
    // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
    // because it is sharing a resource, such as the Serial port.
    // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
    if (xTestSemaphore == NULL)  // Check to confirm that the Semaphore has not already been created.
    {
        xTestSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore.
        if ((xTestSemaphore) != NULL)
        {
            xSemaphoreGive((xTestSemaphore));  // Make the mutex available for use, by initially "Giving" the Semaphore.
        }
    }

    xTaskCreate(
        task1, "Task1"  // A name just for humans
        ,
        configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL);

    xTaskCreate(
        task2, "Task2"  // A name just for humans
        ,
        configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL);
}

/*-----------------------------------------------------------*/
void task1(void *pvParameters)
{
    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = 5000 / portTICK_PERIOD_MS;  // 500 ms

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        if (hum_temp_measure())
        {
            puts("measure is successfully read");
            printf("hum: %u , temp: %u \n", humidity_get_last_measurement(), temperature_get_last_measurement());
        }
        else
        {
            puts("Can't read humidity and temperature measurements");
        }
        if (co2_measure())
        {
            printf("Co2 read = %u\n", co2_get_latest_measurement());
        }
        else
        {
            printf("Error reading co2\n");
        }

        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        PORTA ^= _BV(PA0);
    }
}

/*-----------------------------------------------------------*/
void task2(void *pvParameters)
{
    loran_prepare_LoRaWan_module();
    loran_setup_OTAA();
    puts("Join OTAA successfully !!");

    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = 10000 / portTICK_PERIOD_MS;  // 1000 ms
                                                               // loran_setup_OTAA();
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        uint16_t         hum     = humidity_get_last_measurement();
        int16_t          temp    = temperature_get_last_measurement();
        uint16_t         co2_ppm = co2_get_latest_measurement();
        uint8_t          flags   = 0xE0;
        payload_uplink_t p       = payload_pack_thc(flags, temp, hum, co2_ppm);
        loran_send_uplink(p.data, p.length, 6);
        puts("Task2 done.");
        PORTA ^= _BV(PA7);
    }
}

/*-----------------------------------------------------------*/
void initialiseSystem()
{
    // Set output ports for leds used in the example
    DDRA |= _BV(DDA0) | _BV(DDA7);

    // Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
    stdio_initialise(ser_USART0);
    // Let's create some tasks
    create_tasks_and_semaphores();

    // vvvvvvvvvvvvvvvvv BELOW IS LoRaWAN initialisation vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // Status Leds driver
    status_leds_initialise(5);  // Priority 5 for internal task

    lorawan_init(ser_USART1, NULL);
}

/*-----------------------------------------------------------*/
int main(void)
{
    initialiseSystem();  // Must be done as the very first thing!!

    printf("Program Started!!\n");
    if (hum_temp_init())
    {
        puts("HIH8120 driver initialized successfully");
    }
    else
    {
        puts("HIH8120 driver failed");
    }

    co2_init();

    vTaskStartScheduler();  // Initialise and run the freeRTOS scheduler. Execution should never return from here.

    /* Replace with your application code */
    while (1)
    {
    }
}
