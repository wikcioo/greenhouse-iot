extern "C"
{
#include "lorawan.h"

#include <lora_driver.h>
#include <status_leds.h>

#include "hardware_controller.h"
#include "payload.h"
}

#include <gtest/gtest.h>

#include "FreeRTOS_FFF_MocksDeclaration.h"
#include "fff.h"

MessageBufferHandle_t upLinkMessageBufferHandle;
MessageBufferHandle_t downLinkMessageBufferHandle;
MessageBufferHandle_t intervalDataMessageBufferHandle;
MessageBufferHandle_t presetDataMessageBufferHandle;
EventGroupHandle_t    xCreatedEventGroup;

FAKE_VOID_FUNC(lora_driver_resetRn2483, uint8_t);
FAKE_VOID_FUNC(lora_driver_flushBuffers);
FAKE_VOID_FUNC(status_leds_shortPuls, status_leds_t);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_sendUploadMessage, bool, lora_driver_payload_t *);
FAKE_VOID_FUNC(status_leds_slowBlink, status_leds_t);
FAKE_VALUE_FUNC(char *, lora_driver_mapReturnCodeToText, lora_driver_returnCode_t);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_rn2483FactoryReset);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_configureToEu868);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_getRn2483Hweui, char *);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setDeviceIdentifier, const char *);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setOtaaIdentity, char *, char *, char *);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_saveMac);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setAdaptiveDataRate, lora_driver_adaptiveDataRate_t);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_setReceiveDelay, uint16_t);
FAKE_VALUE_FUNC(lora_driver_returnCode_t, lora_driver_join, lora_driver_joinMode_t);
FAKE_VOID_FUNC(status_leds_longPuls, status_leds_t);
FAKE_VOID_FUNC(status_leds_ledOn, status_leds_t);
FAKE_VOID_FUNC(status_leds_ledOff, status_leds_t);
FAKE_VOID_FUNC(status_leds_fastBlink, status_leds_t);

class LorawanTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        RESET_FAKE(lora_driver_resetRn2483);
        RESET_FAKE(lora_driver_flushBuffers);
        RESET_FAKE(status_leds_shortPuls);
        RESET_FAKE(lora_driver_sendUploadMessage);
        RESET_FAKE(status_leds_slowBlink);
        RESET_FAKE(lora_driver_mapReturnCodeToText);
        RESET_FAKE(lora_driver_rn2483FactoryReset);
        RESET_FAKE(lora_driver_configureToEu868);
        RESET_FAKE(lora_driver_getRn2483Hweui);
        RESET_FAKE(lora_driver_setDeviceIdentifier);
        RESET_FAKE(lora_driver_setOtaaIdentity);
        RESET_FAKE(lora_driver_saveMac);
        RESET_FAKE(lora_driver_setAdaptiveDataRate);
        RESET_FAKE(lora_driver_setReceiveDelay);
        RESET_FAKE(lora_driver_join);
        RESET_FAKE(status_leds_longPuls);
        RESET_FAKE(status_leds_ledOn);
        RESET_FAKE(status_leds_ledOff);
        RESET_FAKE(status_leds_fastBlink);
        RESET_FAKE(xTaskCreate);
        RESET_FAKE(vTaskDelay);
        RESET_FAKE(xMessageBufferReceive);
        RESET_FAKE(xEventGroupSetBits);
        RESET_FAKE(xMessageBufferSend);
        RESET_FAKE(xEventGroupSetBits);

        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
};

TEST_F(LorawanTest, lora_handler_initialise)
{
    lora_handler_initialise(3, 4);

    ASSERT_EQ(xTaskCreate_fake.call_count, 2);
}

TEST_F(LorawanTest, lora_setup)
{
    lora_driver_join_fake.return_val = LORA_ACCEPTED;

    lora_setup();

    ASSERT_EQ(status_leds_slowBlink_fake.call_count, 1);
    ASSERT_EQ(lora_driver_rn2483FactoryReset_fake.call_count, 1);
    ASSERT_EQ(lora_driver_configureToEu868_fake.call_count, 1);
    ASSERT_EQ(lora_driver_getRn2483Hweui_fake.call_count, 1);
    ASSERT_EQ(lora_driver_setDeviceIdentifier_fake.call_count, 1);
    ASSERT_EQ(lora_driver_setOtaaIdentity_fake.call_count, 1);
    ASSERT_EQ(lora_driver_saveMac_fake.call_count, 1);
    ASSERT_EQ(lora_driver_setAdaptiveDataRate_fake.call_count, 1);
    ASSERT_EQ(lora_driver_setReceiveDelay_fake.call_count, 1);
    ASSERT_EQ(status_leds_longPuls_fake.call_count, 0);
    ASSERT_EQ(status_leds_ledOn_fake.call_count, 1);
}

// *** DOWNLINK TESTS ***

lora_driver_payload_t example_downlinkPayload;

size_t lorawan_downlink_xMessageBufferReceiveCustomFake(
    MessageBufferHandle_t buffer, void *data, size_t size, TickType_t delay)
{
    lora_driver_payload_t *donwlinkPayload_ptr = (lora_driver_payload_t *) data;

    *donwlinkPayload_ptr = example_downlinkPayload;

    return 0;
}

TEST_F(LorawanTest, downlink_handler_task_run_actions)
{
    example_downlinkPayload.portNo   = 20U;
    example_downlinkPayload.len      = 1;
    example_downlinkPayload.bytes[0] = {0x14};

    xMessageBufferReceive_fake.custom_fake = lorawan_downlink_xMessageBufferReceiveCustomFake;

    downlink_handler_task_run();

    // First xMessageBufferReceive
    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg0_val, downLinkMessageBufferHandle);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(lora_driver_payload_t));
    ASSERT_EQ(xMessageBufferReceive_fake.arg3_val, portMAX_DELAY);

    ASSERT_EQ(xEventGroupSetBits_fake.call_count, 1);
    ASSERT_EQ(xEventGroupSetBits_fake.arg0_val, xCreatedEventGroup);
    ASSERT_EQ(xEventGroupSetBits_fake.arg1_val, BIT_0);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 0);
}

TEST_F(LorawanTest, downlink_handler_task_run_intervals1)
{
    example_downlinkPayload.portNo   = 20U;
    example_downlinkPayload.len      = 2;
    example_downlinkPayload.bytes[0] = {0xA};

    xMessageBufferReceive_fake.custom_fake = lorawan_downlink_xMessageBufferReceiveCustomFake;

    downlink_handler_task_run();

    // First xMessageBufferReceive
    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg0_val, downLinkMessageBufferHandle);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(lora_driver_payload_t));
    ASSERT_EQ(xMessageBufferReceive_fake.arg3_val, portMAX_DELAY);

    ASSERT_EQ(xEventGroupSetBits_fake.call_count, 0);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferSend_fake.arg0_val, intervalDataMessageBufferHandle);
    ASSERT_EQ(xMessageBufferSend_fake.arg2_val, sizeof(interval_t));
    ASSERT_EQ(xMessageBufferSend_fake.arg3_val, portMAX_DELAY);
}

TEST_F(LorawanTest, downlink_handler_task_run_intervals2)
{
    example_downlinkPayload.portNo   = 20U;
    example_downlinkPayload.len      = 5;
    example_downlinkPayload.bytes[0] = {0xA};

    xMessageBufferReceive_fake.custom_fake = lorawan_downlink_xMessageBufferReceiveCustomFake;

    downlink_handler_task_run();

    // First xMessageBufferReceive
    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg0_val, downLinkMessageBufferHandle);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(lora_driver_payload_t));
    ASSERT_EQ(xMessageBufferReceive_fake.arg3_val, portMAX_DELAY);

    ASSERT_EQ(xEventGroupSetBits_fake.call_count, 0);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 2);
    ASSERT_EQ(xMessageBufferSend_fake.arg0_val, intervalDataMessageBufferHandle);
    ASSERT_EQ(xMessageBufferSend_fake.arg2_val, sizeof(interval_t));
    ASSERT_EQ(xMessageBufferSend_fake.arg3_val, portMAX_DELAY);
}

TEST_F(LorawanTest, downlink_handler_task_run_thc_presets)
{
    example_downlinkPayload.portNo   = 20U;
    example_downlinkPayload.len      = 1;
    example_downlinkPayload.bytes[0] = {0x10};

    xMessageBufferReceive_fake.custom_fake = lorawan_downlink_xMessageBufferReceiveCustomFake;

    downlink_handler_task_run();

    // First xMessageBufferReceive
    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg0_val, downLinkMessageBufferHandle);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(lora_driver_payload_t));
    ASSERT_EQ(xMessageBufferReceive_fake.arg3_val, portMAX_DELAY);

    ASSERT_EQ(xEventGroupSetBits_fake.call_count, 0);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferSend_fake.arg0_val, presetDataMessageBufferHandle);
    ASSERT_EQ(xMessageBufferSend_fake.arg2_val, sizeof(preset_data_t));
    ASSERT_EQ(xMessageBufferSend_fake.arg3_val, portMAX_DELAY);
}

// =========

// *** UPLINK TESTS ***

sensor_data_t example_uplinkPayload;

size_t lorawan_uplink_xMessageBufferReceiveCustomFake(
    MessageBufferHandle_t buffer, void *data, size_t size, TickType_t delay)
{
    sensor_data_t *uplinkPayload_ptr = (sensor_data_t *) data;

    *uplinkPayload_ptr = example_uplinkPayload;

    return 0;
}

TEST_F(LorawanTest, uplink_handler_task_run)
{
    example_uplinkPayload.is_water_valve_open = true;
    example_uplinkPayload.temp                = 700U;
    example_uplinkPayload.hum                 = 2555U;
    example_uplinkPayload.co2                 = 1000U;

    xMessageBufferReceive_fake.custom_fake = lorawan_uplink_xMessageBufferReceiveCustomFake;

    uplink_handler_task_run();

    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg0_val, upLinkMessageBufferHandle);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(sensor_data_t));
    ASSERT_EQ(xMessageBufferReceive_fake.arg3_val, portMAX_DELAY);

    ASSERT_EQ(lora_driver_mapReturnCodeToText_fake.call_count, 1);

    ASSERT_EQ(lora_driver_sendUploadMessage_fake.call_count, 1);
    ASSERT_EQ(lora_driver_sendUploadMessage_fake.arg0_val, false);

    ASSERT_EQ(status_leds_shortPuls_fake.call_count, 1);
    ASSERT_EQ(status_leds_shortPuls_fake.arg0_val, led_ST4);
}