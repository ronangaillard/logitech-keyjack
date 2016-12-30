#include "nrf24.h"
#include <SPI.h>

void init_nrf24(void) {
    /**
    * Inits and config the nrf24 moduel
    * 
    * @param none
    * @return none
    */

    pinMode(PIN_CE, OUTPUT);
    pinMode(PIN_CSN, OUTPUT);

    digitalWrite(PIN_CE, LOW);
    digitalWrite(PIN_CSN, HIGH);

    SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_2XCLOCK_MASK);

    /* Set registers to receive packet with nrf */

    /* Set RF channel */
	write_register(REG_RF_CH, INIT_CHANNEL_MAPPED);

    /* Set length of incoming payload TODO : change these values */
	write_register(REG_RX_PW_P0, 16 );
	write_register(REG_RX_PW_P1, 16 );

    /* Set RX mode */
	write_register(REG_CONFIG,  (1<<BIT_PWR_UP) | (1<<BIT_PRIM_RX)); 
	digitalWrite(PIN_CE, HIGH);
}

void write_register (uint8_t reg_number, uint8_t value) {
    nrf24_select();
    SPI.transfer(W_REGISTER | (REGISTER_MASK & reg_number));
    SPI.transfer(value);
    nrf24_unselect();
}

uint8_t read_register (uint8_t reg_number) {
    nrf24_select();
    return SPI.transfer(R_REGISTER | (REGISTER_MASK & reg_number));
    nrf24_unselect();
}

void nrf24_select(void) {
    digitalWrite(PIN_CSN, LOW);
}

void nrf24_unselect(void) {
    digitalWrite(PIN_CSN, HIGH);
}