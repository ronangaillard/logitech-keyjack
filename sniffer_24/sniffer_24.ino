#include "nrf24.h"
#include "logitech-device.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_nrf24();

  uint8_t channel = read_register(REG_RF_CH);

  Serial.print("Channel should be : ");
  Serial.println(INIT_CHANNEL_MAPPED, DEC);
  Serial.print("Channel is : ");
  Serial.println(channel, DEC);
}

void loop() {
  int channel = 0;

  for(channel = LOGITECH_START_FREQUENCY; channel < LOGITECH_END_FREQUENCY; channel++) {
      Serial.print("Listening to channel (");
      Serial.print(channel, DEC);
      Serial.println(")");

      nrf24_set_channel(channel);

      delay(1000);

      if(!nrf24_rx_fifo_empty())
        Serial.println("Heard something !!");
  }

}
