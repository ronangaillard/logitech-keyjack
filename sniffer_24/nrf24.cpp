#include "nrf24.h"
#include <SPI.h>

void init_nrf24(void)
{
    /**
    * Inits and config the nrf24 module
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

    /* Power down */
    nrf24_set_config(CONFIG_VALUE_PW_DW);

    /* Set length of incoming payload  */
    write_register(REG_RX_PW_P0, PAYLOAD_SIZE);
    write_register(REG_RX_PW_P1, PAYLOAD_SIZE);

    /* Enable RX addresses */
    write_register(REG_EN_RXADDR, ( (1 << BIT_ERX_P0) | (1 << BIT_ERX_P1) ));

    /* Sets address width */
    write_register(REG_SETUP_AW, 0x3);    

    nrf24_disable_dpl();

    /* Set RF channel */
    nrf24_set_channel(INIT_CHANNEL);
}

void nrf24_set_bandwith(uint8_t bw)
{
    /**
    * Sets the bandwith in MHz of the communication
    * Allowed bandwith are 1MHz and 2MHz
    * 
    * @param bw the bandwith value in MHz
    * @return none
    */

    if(bw == 1)
        write_register(REG_RF_SETUP, (DEFAULT_RF_SETUP | 0 << BIT_RF_DR));
    else
        write_register(REG_RF_SETUP, (DEFAULT_RF_SETUP | 1 << BIT_RF_DR));
}

void write_register(uint8_t reg_number, uint8_t value)
{
    /**
    * Updates the designated NRF24 register with the byte valu
    * 
    * @param reg_number (uint8_t) Register number to be modified
    * @param value (uint8_t) The value to be updated in the register
    * @return none
    */

    digitalWrite(PIN_CE, LOW);
    nrf24_unselect();
    nrf24_select();
    SPI.transfer(W_REGISTER | (REGISTER_MASK & reg_number));
    SPI.transfer(value);
    nrf24_unselect();
}

uint8_t read_register(uint8_t reg_number)
{
    /**
    * Reads the designated register value in the nrf24 and returns it
    * 
    * @param reg_number (uint8_t) the registered to be read from
    * @return (uint8_t) the value of the register
    */
    nrf24_unselect();
    nrf24_select();
    SPI.transfer(R_REGISTER | (REGISTER_MASK & reg_number));
    return SPI.transfer(0xFF);
    nrf24_unselect();
}

void nrf24_select(void)
{
    /**
    * Sets Slave Select pin LOW to select NRF24 chip to send SPI commands to it
    * 
    * @param none
    * @return none
    */
    digitalWrite(PIN_CSN, LOW);
}

void nrf24_unselect(void)
{
    /**
    * Sets Slave Select pin to HIGH to send command to other SPI devices
    * 
    * @param none
    * @return none
    */
    digitalWrite(PIN_CSN, HIGH);
}

bool nrf24_rx_fifo_empty(void)
{
    /**
    * Return state of RX FIFO
    * 
    * @param none
    * @return boolean corresponding to FIFO state (true -> FIFO is empty)
    */

    uint8_t status = read_register(REG_STATUS);
    uint8_t fifo_status = read_register(REG_FIFO_STATUS);

    return ( !(status & (1 << BIT_RX_DR)) ) | (fifo_status & (1 << BIT_RX_EMPTY) );
}

void nrf24_set_channel(int channel)
{
    /**
    * Set RX/TX channel
    * 
    * @param channel (uint8_t) the channel to listen to
    * @return none
    */
    write_register(REG_RF_CH, channel - 2400);
}

void nrf24_power_rx(void)
{
    /**
    * Re-powers the RX part (needed to take payload and channel changes into account)
    * Also flushes RX
    * 
    * @param none
    * @return none
    */

    /* Set RX mode and CRC to 2 bytes (and poweron on) */
    nrf24_set_config(CONFIG_VALUE);

    /* Power up */
    write_register(REG_STATUS, (1 << BIT_RX_DR) | (1 << BIT_MAX_RT));

    /* Flush RX */
    nrf24_select();
    SPI.transfer( FLUSH_RX );
    nrf24_unselect();

    /* Test */
    digitalWrite(PIN_CE, LOW);
    delay(50);
    digitalWrite(PIN_CE, HIGH);
}

void nrf24_set_config(uint8_t value)
{
    /**
    * Sets the config register value
    * 
    * @param value (uint8_t) the value of the register to be set
    * @return none
    */

    digitalWrite(PIN_CE, LOW);
    write_register(REG_CONFIG, value);
    digitalWrite(PIN_CE, HIGH);
}
void nrf24_set_rx_address_p0(uint8_t *address, uint8_t length)
{
    /**
    * Set RX address of NRF 24 (pipe 0)
    * This is usually known as the max address (5 bytes)
    * The address can vary between 3 and 5 bytes
    * 
    * @param address an array containing the bytes of the address
    * @param length the number of bytes of the address
    * @return none
    */
    
    int i = 0;
    digitalWrite(PIN_CE, LOW);
    nrf24_select();

    SPI.transfer(W_REGISTER | (REGISTER_MASK & REG_RX_ADDR_P0));

    for (i = 0; i < length; i++)
        SPI.transfer(address[i]);

    nrf24_unselect();
    digitalWrite(PIN_CE, HIGH);
}

void nrf24_set_rx_address_p1(uint8_t *address, uint8_t length)
{
    /**
    * Set RX address of NRF 24 (pipe 1)
    * This is usually known as the max address (5 bytes)
    * The address can vary between 3 and 5 bytes
    * 
    * @param address an array containing the bytes of the address
    * @param length the number of bytes of the address
    * @return none
    */
    
    int i = 0;
    digitalWrite(PIN_CE, LOW);
    nrf24_select();

    SPI.transfer(W_REGISTER | (REGISTER_MASK & REG_RX_ADDR_P1));

    for (i = 0; i < length; i++)
        SPI.transfer(address[i]);

    nrf24_unselect();
    digitalWrite(PIN_CE, HIGH);
}

void nrf24_toggle_activate(void) 
{
    /**
    * Toogle dynamic/static payload length
    *
    * @param none
    * @return none
    */

    /* Power off */
    digitalWrite(PIN_CE, LOW);
    nrf24_set_config(CONFIG_VALUE_PW_DW);

    nrf24_select();
    SPI.transfer(ACTIVATE);
    SPI.transfer(0x73);
    nrf24_unselect();

    /* Power on */
    nrf24_set_config(CONFIG_VALUE);
    digitalWrite(PIN_CE, HIGH);
    
    nrf24_power_rx();
}

void nrf24_enable_dpl(void)
{
    /**
    * Enable dynamic payload
    *
    * @param none
    * @return none
    */

    write_register(REG_FEATURE, (1 << BIT_EN_DPL));
    write_register(REG_EN_AA, ((1 << BIT_ENAA_P0) | (1 << BIT_ENAA_P1)) );
    write_register(REG_DYNPD, ((1 << BIT_DPL0) | (1 << BIT_DPL1)) );

    nrf24_toggle_activate();
}

void nrf24_disable_dpl(void)
{
    /**
    * Disable dynamic payload (-> static payload)
    *
    * @param none
    * @return none
    */

    write_register(REG_FEATURE, 0);
    write_register(REG_EN_AA, ((1 << BIT_ENAA_P0) | (1 << BIT_ENAA_P1)) );
    write_register(REG_DYNPD, 0);

    nrf24_toggle_activate();
}

void nrf24_read_rx_data(uint8_t * data) 
{
    int i;

    nrf24_unselect();
    nrf24_select();
    SPI.transfer(R_RX_PAYLOAD);

    for(i = 0;i < PAYLOAD_SIZE;i++){
		data[i] = SPI.transfer(0xFF);
	}

    nrf24_unselect();                          
    
    /* Set bit to indicate that data was read */
    write_register(REG_STATUS, (1 << BIT_RX_DR));   
}
