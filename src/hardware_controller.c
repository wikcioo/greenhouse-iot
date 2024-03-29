#include "hardware_controller.h"

#include <message_buffer.h>
#include <status_leds.h>
#include <stdio.h>
#include <task.h>

#include "co2.h"
#include "humidity_temperature.h"
#include "logger.h"
#include "water_controller.h"

extern MessageBufferHandle_t upLinkMessageBufferHandle;
extern MessageBufferHandle_t presetDataMessageBufferHandle;
extern MessageBufferHandle_t actionDataMessageBufferHandle;

range_t temp_range;
range_t hum_range;
range_t co2_range;

extern time_point_t daily_time;
time_point_t        end_watering_time;

static void _print_preset_data()
{
    LOG("New preset data {\n\ttemperature: [%u, %u]\n\thumidity: [%u, %u]\n\tco2: [%u, %u]\n}\n", temp_range.low,
        temp_range.high, hum_range.low, hum_range.high, co2_range.low, co2_range.high);
}

void hc_handler_initialise(
    UBaseType_t preset_data_receive_priority, UBaseType_t measurement_priority, UBaseType_t toggle_priority)
{
    xTaskCreate(
        hc_receive_preset_data_handler_task, "Preset Data Receiver", configMINIMAL_STACK_SIZE, NULL,
        preset_data_receive_priority, NULL);
    xTaskCreate(hc_toggle_handler_task, "Water Toggler", configMINIMAL_STACK_SIZE, NULL, toggle_priority, NULL);
    xTaskCreate(
        hc_measurement_handler_task, "Hardware Controller", configMINIMAL_STACK_SIZE, NULL, measurement_priority, NULL);
}

void hc_receive_preset_data_handler_task_run(void)
{
    preset_data_t data;
    xMessageBufferReceive(presetDataMessageBufferHandle, &data, sizeof(preset_data_t), portMAX_DELAY);

    temp_range = *data.temp_range;
    hum_range  = (range_t){data.hum_range->low * 10, data.hum_range->high * 10};
    co2_range  = *data.co2_range;

    vTaskDelay(pdMS_TO_TICKS(2000));
    _print_preset_data();
}

void hc_receive_preset_data_handler_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1500));
    _print_preset_data();

    for (;;)
    {
        hc_receive_preset_data_handler_task_run();
    }
}

void hc_toggle_handler_task_run(void)
{
    action_t received_action;
    xMessageBufferReceive(actionDataMessageBufferHandle, &received_action, sizeof(action_t), portMAX_DELAY);

    LOG("Received action {\n\twater_on: %s\n\tduration: %u\n}\n", received_action.water_on ? "true" : "false",
        received_action.duration);

    if (received_action.water_on)
    {
        time_point_t new_end_time = time_add_minutes(daily_time, received_action.duration);
        if (!time_is_before(&new_end_time, &end_watering_time))
        {
            end_watering_time = new_end_time;
        }

        if (time_is_before(&daily_time, &end_watering_time))
        {
            water_controller_on();
        }
    }
    else
    {
        if (!time_is_before(&daily_time, &end_watering_time))
        {
            water_controller_off();
        }
    }
}

void hc_toggle_handler_task(void *pvParameters)
{
    for (;;)
    {
        hc_toggle_handler_task_run();
    }
}

static void _warn_if_measurement_outside_range(const char *name, uint16_t measurement, range_t range, status_leds_t led)
{
    if (range.low == 0 && range.high == 0)
        return;

    if (measurement < range.low || measurement > range.high)
    {
        LOG("[warning]: %s measurement (%u) is outside of range [%u, %u]\n", name, measurement, range.low, range.high);

        status_leds_fastBlink(led);
    }
    else
    {
        status_leds_ledOff(led);
    }
}

/*
 *  The following led configuration is used:
 *  ST4 (Blue)  : for temperature outside of preset range
 *  ST3 (Yellow): for humidity outside of preset range
 *  ST2 (Green) : for co2 outside of preset range
 */
static void _handle_measurements_outside_range(uint16_t temp, uint16_t hum, uint16_t co2)
{
    _warn_if_measurement_outside_range("temperature", temp, temp_range, led_ST4);
    _warn_if_measurement_outside_range("humidity", hum, hum_range, led_ST3);
    _warn_if_measurement_outside_range("co2", co2, co2_range, led_ST2);
}

void hc_measurement_handler_task_run(uint8_t counter)
{
    co2_measure();
    hum_temp_measure();

    uint16_t temp = get_last_temperature_measurement();
    uint16_t hum  = get_last_humidity_measurement();
    uint16_t co2  = co2_get_latest_measurement();

    LOG("Measurement [%d] {\n\tTemperature: %u\n\tHumidity: %d\n\tCO2: %u\n}\n", counter, temp, hum, co2);

    _handle_measurements_outside_range(temp, hum, co2);

    if (counter == 5)
    {
        sensor_data_t data = {water_controller_is_water_valve_open(), temp, hum, co2};

        // TODO: Add error handling
        xMessageBufferSend(upLinkMessageBufferHandle, (void *) &data, sizeof(sensor_data_t), portMAX_DELAY);

        counter = 0;
    }
}

void hc_measurement_handler_task(void *pvParameters)
{
    TickType_t       xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency    = pdMS_TO_TICKS(60000UL);
    uint8_t          counter       = 0;

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        counter++;
        hc_measurement_handler_task_run(counter);
        if (counter == 5)
        {
            counter = 0;
        }
    }
}