#include "lorawan.h"

#include <event_groups.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <stddef.h>
#include <stdio.h>
#include <task.h>

#include "env.h"
#include "hardware_controller.h"
#include "logger.h"
#include "payload.h"
#include "scheduler.h"

extern MessageBufferHandle_t upLinkMessageBufferHandle;
extern MessageBufferHandle_t downLinkMessageBufferHandle;
extern MessageBufferHandle_t intervalDataMessageBufferHandle;
extern MessageBufferHandle_t presetDataMessageBufferHandle;
extern EventGroupHandle_t    xCreatedEventGroup;
extern action_t              manual_watering_action;
extern time_point_t          daily_time;
extern time_point_t          end_watering_time;


void lora_handler_initialise(UBaseType_t uplink_priority, UBaseType_t downlink_priority)
{
    xTaskCreate(uplink_handler_task, "LRUpLink", configMINIMAL_STACK_SIZE + 200, NULL, uplink_priority, NULL);
    xTaskCreate(downlink_handler_task, "LRDownLink", configMINIMAL_STACK_SIZE + 200, NULL, downlink_priority, NULL);
}

void uplink_handler_task_run(void)
{
    sensor_data_t data;
    xMessageBufferReceive(upLinkMessageBufferHandle, &data, sizeof(sensor_data_t), portMAX_DELAY);

    // TODO: Unhardcode 0xE0
    uint8_t flags = 0xE0;
    if (data.is_water_valve_open)
    {
        flags |= 1 << 0;
    }
    payload_uplink_t packed_payload = payload_pack_thc(flags, data.temp, data.hum, data.co2);

    lora_driver_payload_t _uplink_payload;
    _uplink_payload.len    = packed_payload.length;
    _uplink_payload.portNo = 2;

    for (uint8_t i = 0; i < packed_payload.length; i++)
    {
        _uplink_payload.bytes[i] = packed_payload.data[i];
    }

    status_leds_shortPuls(led_ST4);
    char *result = lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_uplink_payload));
    LOG("Upload Message >%s<\n", result);
}

void uplink_handler_task(void *pvParameters)
{
    lora_driver_resetRn2483(1);
    vTaskDelay(2);
    lora_driver_resetRn2483(0);
    vTaskDelay(150);

    lora_driver_flushBuffers();
    lora_setup();

    for (;;)
    {
        uplink_handler_task_run();
    }
}

void send_intervals_to_scheduler(lora_driver_payload_t *downlinkPayload)
{
    interval_t intervals[7] = {0};
    payload_unpack_intervals(downlinkPayload->bytes, downlinkPayload->len, intervals);
    for (uint8_t i = 0; i < (downlinkPayload->len + 1) / 3; i++)
    {
        xMessageBufferSend(intervalDataMessageBufferHandle, &(intervals[i]), sizeof(interval_t), portMAX_DELAY);
    }
}

void downlink_handler_task_run(void)
{
    lora_driver_payload_t downlinkPayload;

    xMessageBufferReceive(downLinkMessageBufferHandle, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY);

    LOG("DOWN LINK: from port: %d with %d bytes received!\n", downlinkPayload.portNo, downlinkPayload.len);

    payload_id_t payload_id = payload_get_id_u8_ptr(downlinkPayload.bytes);
    LOG("Payload name = %s\n", PAYLOAD_ID_TO_NAME(payload_id));

    if (payload_id == ACTIONS)
    {
        payload_unpack_actions_u8_ptr(downlinkPayload.bytes, &manual_watering_action);
        time_point_t new_end_watering_time =
            time_get_sum_of_time_with_minutes(daily_time, manual_watering_action.duration);

        if (time_is_before(&end_watering_time, &new_end_watering_time))
        {
            end_watering_time = new_end_watering_time;
            puts("Water toggling from scheduler.");
            xEventGroupSetBits(xCreatedEventGroup, BIT_0);
        }
        puts("Water toggling is skiped");
    }
    else if (payload_id == INTERVALS_CLS_APPEND)
    {
        // Send an interval with all zeros to reset the interval array in scheduler
        interval_t reset_interval = (interval_t){.start = {.hour = 0, .minute = 0}, .end = {.hour = 0, .minute = 0}};
        xMessageBufferSend(intervalDataMessageBufferHandle, &reset_interval, sizeof(interval_t), portMAX_DELAY);

        send_intervals_to_scheduler(&downlinkPayload);
    }
    else if (payload_id == INTERVALS_APPEND)
    {
        send_intervals_to_scheduler(&downlinkPayload);
    }
    else if (payload_id == THC_PRESETS)
    {
        puts("Received THC_PRESETS");

        range_t temp_range, hum_range, co2_range;
        payload_unpack_thc_presets_u8_ptr(downlinkPayload.bytes, &temp_range, &hum_range, &co2_range);

        preset_data_t data = {&temp_range, &hum_range, &co2_range};
        xMessageBufferSend(presetDataMessageBufferHandle, (void *) &data, sizeof(preset_data_t), portMAX_DELAY);
    }
}

void downlink_handler_task(void *pvParameters)
{
    for (;;)
    {
        downlink_handler_task_run();
    }
}

void lora_setup(void)
{
    char *result;
    char  _out_buf[20];

    lora_driver_returnCode_t rc;

    // Factory reset the transceiver
    result = lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset());
    LOG("FactoryReset >%s<\n", result);

    // Configure to EU868 LoRaWAN standards
    result = lora_driver_mapReturnCodeToText(lora_driver_configureToEu868());
    LOG("Configure to EU868 >%s<\n", result);

    // Get the transceivers HW EUI
    rc     = lora_driver_getRn2483Hweui(_out_buf);
    result = lora_driver_mapReturnCodeToText(rc);
    LOG("Get HWEUI >%s<: %s\n", result, _out_buf);

    // Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
    result = lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf));
    LOG("Set DevEUI: %s >%s<\n", _out_buf, result);

    // Set Over The Air Activation parameters to be ready to join the LoRaWAN
    result = lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI, LORA_appKEY, _out_buf));
    LOG("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, result);

    // Save all the MAC settings in the transceiver
    result = lora_driver_mapReturnCodeToText(lora_driver_saveMac());
    LOG("Save mac >%s<\n", result);

    // Enable Adaptive Data Rate
    result = lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON));
    LOG("Set Adaptive Data Rate: ON >%s<\n", result);

    // Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
    result = lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500));
    LOG("Set Receiver Delay: %d ms >%s<\n", 500, result);

    // Join the LoRaWAN
    uint8_t maxJoinTriesLeft = 10;

    do
    {
        rc = lora_driver_join(LORA_OTAA);
        LOG("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

        if (rc != LORA_ACCEPTED)
        {
            vTaskDelay(pdMS_TO_TICKS(5000UL));
        }
        else
        {
            break;
        }
    } while (--maxJoinTriesLeft);

    if (rc == LORA_ACCEPTED)
    {
        status_leds_ledOn(led_ST2);
    }
    else
    {
        while (1)
        {
            taskYIELD();
        }
    }
}
