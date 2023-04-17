/*
 * LoRaWan_imp.c
 *
 * Created: 15/04/2023 03:14:37
 *  Author: wael
 */
#include <lora_driver.h>
#include <stddef.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>

#include <lora_driver.h>
#include <status_leds.h>

#define LORA_appEUI ""
#define LORA_appKEY ""

void lorawan_init(serial_comPort_t comPort, bool withDownlinkMessageBuffer)
{
    if (withDownlinkMessageBuffer)
    {
        MessageBufferHandle_t downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t) * 2);
        lora_driver_initialise(comPort, downLinkMessageBufferHandle);
        puts("LoRaWan init successfully");
    }
    else
    {
        lora_driver_initialise(comPort, NULL);
        puts("LoRaWan init successfully");
    }
}
// This must be done from a FreeRTOS task!!
void loran_prepare_LoRaWan_module()
{
    lora_driver_resetRn2483(1);  // Activate reset line
    vTaskDelay(2);
    lora_driver_resetRn2483(0);  // Release reset line
    vTaskDelay(150);             // Wait for tranceiver module to wake up after reset
    lora_driver_flushBuffers();  // get rid of first version string from module after reset!
    puts("LoRaWan prepared successfully");
}
void loran_setup_OTAA(void)
{
    char                     _out_buf[20];
    lora_driver_returnCode_t rc;
    status_leds_slowBlink(led_ST2);  // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

    // Factory reset the transceiver
    printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));

    // Configure to EU868 LoRaWAN standards
    printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

    // Get the transceivers HW EUI
    rc = lora_driver_getRn2483Hweui(_out_buf);
    printf("Get HWEUI >%s<: %s\n", lora_driver_mapReturnCodeToText(rc), _out_buf);

    // Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
    printf(
        "Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

    // Set Over The Air Activation parameters to be ready to join the LoRaWAN
    printf(
        "Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf,
        lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI, LORA_appKEY, _out_buf)));

    // Save all the MAC settings in the transceiver
    printf("Save mac >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

    // Enable Adaptive Data Rate
    printf(
        "Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

    // Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
    printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

    // Join the LoRaWAN
    uint8_t maxJoinTriesLeft = 10;

    do
    {
        rc = lora_driver_join(LORA_OTAA);
        printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

        if (rc != LORA_ACCEPTED)
        {
            // Make the red led pulse to tell something went wrong
            status_leds_longPuls(led_ST1);  // OPTIONAL
            // Wait 5 sec and lets try again
            vTaskDelay(pdMS_TO_TICKS(5000UL));
        }
        else
        {
            break;
        }
    } while (--maxJoinTriesLeft);

    if (rc == LORA_ACCEPTED)
    {
        // Connected to LoRaWAN :-)
        // Make the green led steady
        status_leds_ledOn(led_ST2);  // OPTIONAL
    }
    else
    {
        // Something went wrong
        // Turn off the green led
        status_leds_ledOff(led_ST2);  // OPTIONAL
        // Make the red led blink fast to tell something went wrong
        status_leds_fastBlink(led_ST1);  // OPTIONAL

        // Lets stay here
        while (1)
        {
            taskYIELD();
        }
    }
}
void loran_send_uplink(uint8_t *data, uint8_t length, uint8_t portNumber)
{
    lora_driver_payload_t uplinkPayload;
    uplinkPayload.len    = length;
    uplinkPayload.portNo = portNumber;
    for (uint8_t i = 0; i < 6; i++)
    {
        uplinkPayload.bytes[i] = data[i];  // data[i] == *(data + i)
    }

    lora_driver_returnCode_t rc;

    if ((rc = lora_driver_sendUploadMessage(false, &uplinkPayload)) == LORA_MAC_TX_OK)
    {
        // The uplink message is sent and there is no downlink message received
        puts("successful sent !!");
    }
    else if (rc == LORA_MAC_RX)
    {
        // The uplink message is sent and a downlink message is received
        puts("successful sent with downlink message  !!");
    }
    else
    {
        puts("failed to send");
    }
}
