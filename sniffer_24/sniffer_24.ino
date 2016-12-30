#include "nrf24.h"
#include "logitech-device.h"

uint8_t base_mac_address[] = {0xBB, 0x0A, 0xDC, 0xA5, 0x75};

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

  nrf24_set_rx_address(base_mac_address, 5);
}

void loop() {
  int channel = 0;

  for(channel = LOGITECH_START_FREQUENCY; channel < LOGITECH_END_FREQUENCY; channel++) {
      Serial.print("Listening to channel (");
      Serial.print(channel, DEC);
      Serial.println(")");

      nrf24_set_channel(channel);

      delay(30000);

      if(!nrf24_rx_fifo_empty())
        Serial.println("Heard something !!");
  }

  Serial.println("DONE ! RESET TO RESTART !");
  while(1) {}

}
