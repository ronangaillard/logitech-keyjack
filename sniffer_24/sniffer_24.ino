#include "nrf24.h"
#include "logitech-device.h"

uint8_t base_mac_address[] = {0xBB, 0x0A, 0xDC, 0xA5, 0x75};
uint8_t base_mac_address_reversed[] = {0x75, 0xA5, 0xDC, 0x0A, 0xBB};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_nrf24();
  nrf24_set_bandwith(2); /* Logitech keyboards use 2MHz bw */

  uint8_t channel = read_register(REG_RF_CH);

  Serial.print("Channel is : ");
  Serial.println(channel + 2400, DEC);

  nrf24_set_rx_address_p0(base_mac_address, 5);
  nrf24_set_rx_address_p1(base_mac_address_reversed, 5);
  
  nrf24_power_rx();
}

void loop() {
  int channel, j=0;

  for(channel = LOGITECH_START_FREQUENCY; channel <= LOGITECH_END_FREQUENCY; channel+=3) {
      nrf24_set_channel(channel);

      nrf24_power_rx();
      
      
        delay(500);

        if(!nrf24_rx_fifo_empty()) {
          Serial.print("Received something on channel : ");
          Serial.println(channel, DEC);
        }
      
      Serial.print('.');
  }

  Serial.println("|");
  nrf24_toggle_activate();

  

}
