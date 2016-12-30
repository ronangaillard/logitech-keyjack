#include "nrf24.h"

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
  // put your main code here, to run repeatedly:

}
