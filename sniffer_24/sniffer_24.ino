#include "nrf24.h"
#include "logitech-device.h"

uint8_t base_mac_address[] = {0xBB, 0x0A, 0xDC, 0xA5, 0x75};
uint8_t base_mac_address_reversed[] = {0x75, 0xA5, 0xDC, 0x0A, 0xBB};

bool dpl_mode = false;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");

  init_nrf24();
  nrf24_set_bandwith(2); /* Logitech keyboards use 2MHz bw */

  uint8_t channel = read_register(REG_RF_CH);

  Serial.print("Channel is : ");
  Serial.println(channel + 2400, DEC);

  nrf24_set_rx_address_p1(base_mac_address, 5);
  nrf24_set_rx_address_p0(base_mac_address_reversed, 5);

  nrf24_power_rx();
}

void loop()
{
  int channel, j = 0;
  bool kb_found = false;
  Serial.println("Looking for keyboard :o");

  while (!kb_found)
  {
    //for (channel = LOGITECH_START_FREQUENCY; channel <= LOGITECH_END_FREQUENCY; channel += 3)
    //{
      //nrf24_set_channel(channel);

      nrf24_power_rx();

      delay(100);

      if (!nrf24_rx_fifo_empty())
      {
        Serial.print("\r\nKeyboard is on channel : ");
        Serial.println(channel, DEC);
        kb_found = true;
        break;
      }

      Serial.print(".");
      if(channel == 125)
        Serial.println();
    //}
  }

  Serial.println("Reading data");
  uint8_t rx_fifo[PAYLOAD_SIZE];

  nrf24_read_rx_data(rx_fifo);

  for(j = 0; j < PAYLOAD_SIZE; j++) {
    Serial.print("data[");
    Serial.print(j, DEC);
    Serial.print("] = 0x");
    Serial.println(rx_fifo[j], HEX);
  }

  while(1) {}
}
