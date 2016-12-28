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

    /* Set registers to receive packet with nrf */

    /* Set RF channel */
	write_register(RF_CH, ... );

    // Set length of incoming payload 
	write_register(RX_PW_P0, ... );
	write_register(RX_PW_P1, ... );

    /* Power up RX, flush RX and set RX mode */
}