#include "hardware_controller.h"

#include <message_buffer.h>
#include <stdio.h>

#include "co2.h"
#include "humidity_temperature.h"
#include "water_controller.h"

extern MessageBufferHandle_t upLinkMessageBufferHandle;
extern MessageBufferHandle_t presetDataMessageBufferHandle;
extern EventGroupHandle_t    xCreatedEventGroup;

static range_t temp_range;
static range_t hum_range;
static range_t co2_range;

void hc_receive_preset_data_handler_task(void *pvParameters);
void hc_toggle_handler_task(void *pvParameters);
void hc_handler_task(void *pvParameters);

static void _print_preset_data()
{
    printf(
        "New preset data {\n\ttemperature: [%u, %u]\n\thumidity: [%u, %u]\n\tco2: [%u, %u]\n}\n", temp_range.low,
        temp_range.high, hum_range.low, hum_range.high, co2_range.low, co2_range.high);
}

void hc_handler_initialise(
    UBaseType_t preset_data_receive_priority, UBaseType_t measurement_priority, UBaseType_t toggle_priority)
{
    /* ======= For testing purposes only ======= */
    temp_range.low  = 700;
    temp_range.high = 820;

    hum_range.low  = 200;
    hum_range.high = 700;

    co2_range.low  = 200;
    co2_range.high = 1500;
    /* ========================================= */

    xTaskCreate(
        hc_receive_preset_data_handler_task, "Preset Data Receiver", configMINIMAL_STACK_SIZE, NULL,
        preset_data_receive_priority, NULL);
    xTaskCreate(hc_toggle_handler_task, "Water Toggler", configMINIMAL_STACK_SIZE, NULL, toggle_priority, NULL);
    xTaskCreate(hc_handler_task, "Hardware Controller", configMINIMAL_STACK_SIZE, NULL, measurement_priority, NULL);
}

void hc_receive_preset_data_handler_task(void *pvParameters)
{
    // TODO: Replace delay with printf with mutexes
    vTaskDelay(pdMS_TO_TICKS(1500));
    _print_preset_data();

    for (;;)
    {
        preset_data_t data;
        xMessageBufferReceive(presetDataMessageBufferHandle, &data, sizeof(preset_data_t), portMAX_DELAY);

        temp_range = *data.temp_range;
        hum_range  = (range_t){data.hum_range->low * 10, data.hum_range->high * 10};
        co2_range  = *data.co2_range;

        vTaskDelay(pdMS_TO_TICKS(2000));
        _print_preset_data();
    }
}

void hc_toggle_handler_task(void *pvParameters)
{
    for (;;)
    {
        xEventGroupWaitBits(xCreatedEventGroup, BIT_0, pdTRUE, pdFALSE, portMAX_DELAY);
        puts("Toggling water with event groups\n");
        if (water_controller_get_state())
        {
            water_controller_off();
        }
        else
        {
            water_controller_on();
        }
    }
}

static void _warn_if_measurement_outside_range(const char *name, uint16_t measurement, range_t range)
{
    if (measurement < range.low || measurement > range.high)
    {
        printf(
            "[warning]: %s measurement (%u) is outside of range [%u, %u]\n", name, measurement, range.low, range.high);
    }
}

static void _handle_measurements_outside_range(uint16_t temp, uint16_t hum, uint16_t co2)
{
    _warn_if_measurement_outside_range("temperature", temp, temp_range);
    _warn_if_measurement_outside_range("humidity", hum, hum_range);
    _warn_if_measurement_outside_range("co2", co2, co2_range);
}

void hc_handler_task(void *pvParameters)
{
    TickType_t       xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency    = pdMS_TO_TICKS(60000UL);
    uint8_t          counter       = 0;

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        co2_measure();
        hum_temp_measure();

        uint16_t temp = get_last_temperature_measurement();
        uint16_t hum  = get_last_humidity_measurement();
        uint16_t co2  = co2_get_latest_measurement();

        counter++;
        printf("Measurement [%d] {\n\tTemperature: %u\n\tHumidity: %d\n\tCO2: %u\n}\n", counter, temp, hum, co2);

        _handle_measurements_outside_range(temp, hum, co2);

        if (counter == 5)
        {
            sensor_data_t data = {water_controller_get_state(), temp, hum, co2};

            // TODO: Add error handling
            xMessageBufferSend(upLinkMessageBufferHandle, (void *) &data, sizeof(sensor_data_t), portMAX_DELAY);

            counter = 0;
        }
    }
}