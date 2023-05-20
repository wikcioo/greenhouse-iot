#include "lorawan.h"

#include <event_groups.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <stddef.h>
#include <stdio.h>
#include <task.h>

#include "env.h"
#include "hardware_controller.h"
#include "scheduler.h"

extern MessageBufferHandle_t upLinkMessageBufferHandle;
extern MessageBufferHandle_t downLinkMessageBufferHandle;
extern MessageBufferHandle_t intervalDataMessageBufferHandle;
extern MessageBufferHandle_t presetDataMessageBufferHandle;
extern EventGroupHandle_t    xCreatedEventGroup;
extern action_t              manual_watering_action;

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
    printf(
        "Upload Message >%s<\n",
        lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_uplink_payload)));
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
void downlink_handler_task_run(void)
{
    lora_driver_payload_t downlinkPayload;

    xMessageBufferReceive(downLinkMessageBufferHandle, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY);

    payload_id_t payload_id = payload_get_id_u8_ptr(downlinkPayload.bytes);

    if (payload_id == ACTIONS)
    {
        payload_unpack_actions_u8_ptr(downlinkPayload.bytes, &manual_watering_action);
        puts("Water toggling form operator");
        xEventGroupSetBits(xCreatedEventGroup, BIT_0);
    }
    else if (payload_id == INTERVALS)
    {
        puts("Received intervals");
        interval_t intervals[7] = {0};
        payload_unpack_intervals(downlinkPayload.bytes, downlinkPayload.len, intervals);
        for (uint8_t i = 0; i < (downlinkPayload.len + 1) / 3; i++)
        {
            xMessageBufferSend(intervalDataMessageBufferHandle, &(intervals[i]), sizeof(interval_t), portMAX_DELAY);
        }
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
    char                     _out_buf[20];
    lora_driver_returnCode_t rc;

    lora_driver_rn2483FactoryReset();
    lora_driver_configureToEu868();
    rc = lora_driver_getRn2483Hweui(_out_buf);
    lora_driver_setDeviceIdentifier(_out_buf);
    lora_driver_setOtaaIdentity(LORA_appEUI, LORA_appKEY, _out_buf);
    lora_driver_saveMac();
    lora_driver_setAdaptiveDataRate(LORA_ON);
    lora_driver_setReceiveDelay(500);

    uint8_t maxJoinTriesLeft = 10;

    do
    {
        rc = lora_driver_join(LORA_OTAA);
        printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

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
