extern "C"
{
#include "lorawan.h"

#include <lora_driver.h>
#include <status_leds.h>
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
        RESET_FAKE(xTaskCreateStatic);
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