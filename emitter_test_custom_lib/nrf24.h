#ifndef NRF24_H
#define NRF24_H

#include <SPI.h>

#define CONFIG_VALUE_PW_DW  (1 << BIT_CRCO)
#define CONFIG_VALUE_TX     ( CONFIG_VALUE_PW_DW | (1 << BIT_PWR_UP) )
#define CONFIG_VALUE        ( CONFIG_VALUE_TX | (1 << BIT_PRIM_RX) ) 

#define DEFAULT_RF_SETUP    ( (1 << BIT_LNA_HCURR ) | (0x3 << BIT_RF_PWR) )

#define PAYLOAD_SIZE        22

/* PINS */
#define PIN_CE  8
#define PIN_CSN 7

/* COMMANDS */
#define R_REGISTER      0x00    
#define W_REGISTER      0x20
#define REGISTER_MASK   0x1F
#define ACTIVATE        0x50
#define R_RX_PAYLOAD    0x61
#define W_TX_PAYLOAD    0xA0
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define REUSE_TX_PL     0xE3
#define NOP             0xFF

/* REGISTER MAP TABLE */
#define REG_CONFIG      0x00
#define REG_EN_AA       0x01
#define REG_EN_RXADDR   0x02
#define REG_SETUP_AW    0x03
#define REG_SETUP_RETR  0x04
#define REG_RF_CH       0x05
#define REG_RF_SETUP    0x06
#define REG_STATUS      0x07
#define REG_OBSERVE_TX  0x08
#define REG_CD          0x09
#define REG_RX_ADDR_P0  0x0A
#define REG_RX_ADDR_P1  0x0B
#define REG_RX_ADDR_P2  0x0C
#define REG_RX_ADDR_P3  0x0D
#define REG_RX_ADDR_P4  0x0E
#define REG_RX_ADDR_P5  0x0F
#define REG_TX_ADDR     0x10
#define REG_RX_PW_P0    0x11
#define REG_RX_PW_P1    0x12
#define REG_RX_PW_P2    0x13
#define REG_RX_PW_P3    0x14
#define REG_RX_PW_P4    0x15
#define REG_FIFO_STATUS 0x17
#define REG_DYNPD       0x1C
#define REG_FEATURE     0x1D

/* BIT MNEMONIC */
#define BIT_MASK_RX_DR  6
#define BIT_MASK_TX_DS  5
#define BIT_MASK_MAX_RT 4
#define BIT_EN_CRC      3
#define BIT_CRCO        2
#define BIT_PWR_UP      1
#define BIT_PRIM_RX     0
#define BIT_ENAA_P5     5
#define BIT_ENAA_P4     4
#define BIT_ENAA_P3     3
#define BIT_ENAA_P2     2
#define BIT_ENAA_P1     1
#define BIT_ENAA_P0     0
#define BIT_ERX_P5      5
#define BIT_ERX_P4      4
#define BIT_ERX_P3      3
#define BIT_ERX_P2      2
#define BIT_ERX_P1      1
#define BIT_ERX_P0      0
#define BIT_AW          0
#define BIT_ARD         4
#define BIT_ARC         0
#define BIT_PLL_LOCK    4
#define BIT_RF_DR       3
#define BIT_RF_PWR      1
#define BIT_LNA_HCURR   0        
#define BIT_RX_DR       6
#define BIT_TX_DS       5
#define BIT_MAX_RT      4
#define BIT_RX_P_NO     1
#define BIT_TX_FULL     0
#define BIT_PLOS_CNT    4
#define BIT_ARC_CNT     0
#define BIT_TX_REUSE    6
#define BIT_FIFO_FULL   5
#define BIT_TX_EMPTY    4
#define BIT_RX_FULL     1
#define BIT_RX_EMPTY    0
#define BIT_EN_DPL      2
#define BIT_DPL0        0
#define BIT_DPL1        1

/* CONSTANTS */
#define INIT_CHANNEL        2402

void init_nrf24(void);
void write_register (uint8_t reg_number, uint8_t value);
void nrf24_set_bandwith(uint8_t bw);
uint8_t read_register (uint8_t reg_number);
void nrf24_select(void);
void nrf24_unselect(void);
bool nrf24_rx_fifo_empty(void);
void nrf24_set_channel(int channel);
void nrf24_set_rx_address_p0(uint8_t *address, uint8_t length);
void nrf24_set_rx_address_p1(uint8_t *address, uint8_t length);
void nrf24_set_tx_address(uint8_t *address, uint8_t length);
void nrf24_toggle_activate(void);
void nrf24_power_rx(void);
void nrf24_power_tx(void);
void nrf24_set_config(uint8_t value);
void nrf24_enable_dpl(void);
void nrf24_disable_dpl(void);
void nrf24_read_rx_data(uint8_t * data);
void nrf24_send_data(uint8_t* data, uint8_t len);
void nrf24_wait_for_ack(uint8_t *payload);

#endif
