#include <SPI.h>
#include "nrf24.h"
#include "logitech_hid.h"


uint8_t base_mac_address[] = {0xBB, 0x0A, 0xDC, 0xA5, 0x75};

uint8_t pairing_packet[] = {0xA1, 0x5F, 0x1, 0xFB, 0xD6, 0x95, 0xCF, 0x8, 0x8, 0x40, 0x24, 0x4, 0x2, 0x1, 0x4D, 0x0, 0x0, 0x0, 0x0, 0x0, 0x32, 0xD0};


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
                                              
  init_nrf24();
  nrf24_set_bandwith(2); /* Logitech keyboards use 2MHz bw */

  nrf24_set_tx_address(base_mac_address, 5);
  nrf24_set_rx_address_p0(base_mac_address, 5);

  nrf24_set_channel(LOGITECH_START_FREQUENCY);

  print_register("channel", REG_RF_CH);
  print_register("tx addr", REG_TX_ADDR);
  print_register("tx addr +1 ", REG_TX_ADDR + 1);

  nrf24_power_rx();
}


void print_register(const char* reg_name, uint8_t reg){

  Serial.print("Reg : ");
  Serial.print(reg_name);
  Serial.print(" : ");
  Serial.print(read_register(reg), BIN);
  Serial.print(" % ");
  Serial.print(read_register(reg), DEC);
  Serial.println("");

}

void loop()
{
  int j, timeout = 0;

  Serial.println("Sending pairing packet...");

  nrf24_power_tx();

  nrf24_send_data(pairing_packet, PAYLOAD_SIZE);

  Serial.println("Waiting for ack...");

  //delay(400);

  nrf24_wait_for_ack(0);

  Serial.println("Received ACK!");

  /*nrf24_power_rx();

  
  while (nrf24_rx_fifo_empty() && timeout < 10)
  {
    timeout++;
    delay(10);
  }

  if (!nrf24_rx_fifo_empty)
  {
    Serial.println("Reading data");
    uint8_t rx_fifo[PAYLOAD_SIZE];

    nrf24_read_rx_data(rx_fifo);

    for (j = 0; j < PAYLOAD_SIZE; j++) {
      Serial.print("data[");
      Serial.print(j, DEC);
      Serial.print("] = 0x");
      Serial.println(rx_fifo[j], HEX);

    }*/

    //while(1) {}
  //}


}

