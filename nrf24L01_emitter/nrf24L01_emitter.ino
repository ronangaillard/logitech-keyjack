/*
  // March 2014 - TMRh20 - Updated along with High Speed RF24 Library fork
  // Parts derived from examples by J. Coliz <maniacbug@ymail.com>
*/
/**
 * Example for efficient call-response using ack-payloads 
 *
 * This example continues to make use of all the normal functionality of the radios including
 * the auto-ack and auto-retry features, but allows ack-payloads to be written optionally as well.
 * This allows very fast call-response communication, with the responding radio never having to 
 * switch out of Primary Receiver mode to send back a payload, but having the option to if wanting
 * to initiate communication instead of respond to a commmunication.
 */
 


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#define PAYLOAD_SIZE 22
// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 
RF24 radio(8,7);

// Topology
const uint64_t pipes[2] = { 0xBB0ADCA575LL, 0xBB0ADCA575LL };              // Radio pipe addresses for the 2 nodes to communicate.

// Role management: Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  

typedef enum { role_ping_out = 1, role_pong_back } role_e;                 // The various roles supported by this sketch
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};  // The debug-friendly names of those roles
role_e role = role_pong_back;                                              // The role of the current running sketch

// A single byte to keep track of the data being sent back and forth
byte counter = 1;
byte pairing_packet[] = {0xA1, 0x5F, 0x1, 0xFB, 0xD6, 0x95, 0xCF, 0xa, 0x8, 0x40, 0x24, 0x4, 0x2, 0x1, 0x4D, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2f, 0xD0};
//byte pairing_packet[] = {0x7F, 0x5F, 0x01, 0x31, 0x33, 0x73, 0x13, 0x37, 0x08, 0x10, 0x25, 0x04, 0x00, 0x02, 0x0C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x71, 0x40};
byte channel = 5;
byte kb_found = 0;
uint32_t kb_time = 0;

void setup(){

  Serial.begin(115200);
  printf_begin();
  Serial.print(F("\n\rRF24/examples/pingpair_ack/\n\rROLE: "));
  Serial.println(role_friendly_name[role]);
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Setup and configure rf radio
  byte init_status;
  init_status = radio.begin();
  if( init_status == 0 || init_status == 0xff) {
    Serial.println(" init failed");
    while(1){

    }
  }
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(PAYLOAD_SIZE);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.enableDynamicAck();
  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1,pipes[0]);
  radio.setChannel(channel);
  radio.setDataRate(RF24_2MBPS);
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

void loop(void) {

  if (role == role_ping_out){
    
    radio.stopListening();                                  // First, stop listening so we can talk.
        
    printf("Now sending %d as payload: ",pairing_packet);
    byte gotByte;  
    unsigned long time = micros();                          // Take the time, and send it.  This will block until complete   
                                                            //Called when STANDBY-I mode is engaged (User is finished sending)
    //radio.enableDynamicAck();
    if (!radio.write( &pairing_packet, PAYLOAD_SIZE, 0)){
      Serial.println(F("failed."));   
      counter++;   
    }else{
      if(!radio.available()){ 
        Serial.println(F("Blank Payload Received.")); 
        int read_count = 0;
        radio.startListening();
        while(read_count++ < 100) {
          if(radio.available()) {
            printf("ACK length : %d\n\r",radio.read( &gotByte, 1 ));
            printf("Received %d\n\r", gotByte);
          }
          delay(1);
        }
        radio.stopListening();
      }else{
        while(radio.available() ){
          unsigned long tim = micros();
          printf("ACK length : %d\n\r",radio.read( &gotByte, 1 ));
          printf("Got response %d, round-trip delay: %lu microseconds\n\r",gotByte,tim-time);
          counter++;
        }
      }
    }
    if(counter == 100) {
      channel = channel + 3 < 128 ? channel + 3 : 2;
      radio.setChannel(channel);
      counter = 0;
    }
    // Try again later
    delay(10);
  }

  // Pong back role.  Receive each packet, dump it out, and send it back

  if ( role == role_pong_back ) {
    byte pipeNo;
    byte ack_payload[] = {0x22, 0x23};
    byte gotByte[PAYLOAD_SIZE];                                       // Dump the payloads until we've gotten everything
    
    while( radio.available(&pipeNo)){
      //kb_found = 1;
      //kb_time = millis();
      int pack_length = radio.read( &gotByte, PAYLOAD_SIZE );
      printf("Packet on channel %d\n\r", channel);

      printf("Received packet of length %d\n\r",pack_length);
      radio.writeAckPayload(pipeNo,&ack_payload, 2 );   
      printf("Received : ");
      for(int i =0; i<PAYLOAD_SIZE; i++) {
        printf("%x ",gotByte[i]);
      }
      Serial.println();
      printf("Sending back %d as ackpayload\n\r",*ack_payload);
   }
   
   if (!kb_found) {
      kb_found = 0;
      channel = channel + 3 < 128 ? channel + 3 : 2;
      radio.setChannel(channel);
      delay(100);
      //delayMicroseconds(128);
   }
   
 }

  // Change roles

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      radio.printDetails();                   // Dump the configuration of the rf unit for debugging

      role = role_ping_out;                  // Become the primary transmitter (ping out)
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    }
    else if ( c == 'R' && role == role_ping_out )
    {
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      
       role = role_pong_back;                // Become the primary receiver (pong back)
       radio.openWritingPipe(pipes[1]);
       radio.openReadingPipe(1,pipes[0]);
       radio.startListening();
    }
  }
}
