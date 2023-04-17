/*
 * lorawan.h
 *
 * Created: 15/04/2023 03:03:01
 *  Author: drwae
 */
#include <lora_driver.h>

void lorawan_init(serial_comPort_t comPort, bool withDownlinkMessageBuffer);
void loran_setup_OTAA(void);
void loran_receive_downlink();
void loran_prepare_LoRaWan_module();
void loran_send_uplink(uint8_t *data, uint8_t length, uint8_t portNumber);
