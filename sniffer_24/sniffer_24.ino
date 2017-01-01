#include "nrf24.h"
#include "logitech-device.h"

uint8_t base_mac_address[] = {0xBB, 0x0A, 0xDC, 0xA5, 0x75};
uint8_t base_mac_address_reversed[] = {0x75, 0xA5, 0xDC, 0x0A, 0xBB};
uint8_t dongle_mac_address[] = {0x04, 0x6D, 0xC5, 0x2B, 0x00};
uint8_t dongle_mac_address_reversed[] = {0x00, 0x2B, 0xC5, 0x6D, 0x04};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_nrf24();
  nrf24_set_bandwith(2); /* Logitech keyboards use 2MHz bw */

  uint8_t channel = read_register(REG_RF_CH);

  Serial.print("Channel should be : ");
  Serial.println(INIT_CHANNEL, DEC);
  Serial.print("Channel is : ");
  Serial.println(channel + 2400, DEC);

  nrf24_set_rx_address_p0(base_mac_address, 5);
  nrf24_set_rx_address_p1(base_mac_address_reversed, 5);
}

void loop() {
  int channel,j  = 0;

  for(channel = LOGITECH_START_FREQUENCY; channel <= LOGITECH_END_FREQUENCY; channel++) {
      //Serial.print("Listening to channel (");
      //Serial.print(channel, DEC);
      //Serial.println(")");

      nrf24_set_channel(channel);
      
      for(j =0; j < 10; j++) {
        delay(1000);

        if(!nrf24_rx_fifo_empty()) {
          Serial.print("Received something on channel : ");
          Serial.println(channel, DEC);
        }
      }
  }

  Serial.println("Looped ! ");
  nrf24_toggle_activate();

  

}
