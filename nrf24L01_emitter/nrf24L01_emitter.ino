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
// Hardware configuration, Set up nRF24L01 radio on SPI bus plus pins 7 & 8 
RF24 radio(8,7);

// Topology
const uint64_t pipes[2] = { 0xBB0ADCA575LL, 0xBB0ADCA575LL };              // Radio pipe addresses for the 2 nodes to communicate.

// Role management, Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  

typedef enum { role_ping_out = 1, role_pong_back } role_e;                 // The various roles supported by this sketch
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};  // The debug-friendly names of those roles
role_e role = role_ping_out;                                              // The role of the current running sketch

// A single byte to keep track of the data being sent back and forth
byte counter = 1;
//byte pairing_packet[] = {0xa2, 0x5f, 0x01, 0x4f, 0xcc, 0x96, 0xf0, 0x0a, 0x08, 0x40, 0x24, 0x04, 0x02, 0x01, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2d, 0x66};
//byte pairing_packet_1_bis[] = {0xa2, 0x40, 0x01, 0x4f, 0xce};
//byte pairing_packet_1_bis[] = {0xff, 0xff, 0xff, 0xff, 0xff};
byte payload_pairing_2[] =  {0x00, 0x5F, 0x02, 0x01, 0x02, 0x03, 0x04, 0x58, 0x8A, 0x51, 0xEA, 0x1E, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19};

// PDF packets
byte pairing_packet[]  = {0x15,0x5F,0x01,0x84,0x5E,0x3A,0xA2,0x57,0x08,0x10,0x25,0x04,0x00,0x01,0x47,0x00,0x00,0x00,0x00,0x00,0x01,0xEC};
byte pairing_packet_1_bis[] = {0x15, 0x40, 0x01, 0x84, 0x26};
byte payload_pairing_2_bis[] =  {0x00, 0x40, 0x02, 0x01, 0xbd};

//End
//byte pairing_packet[] = {0xA1, 0x5F, 0x1, 0xFB, 0xD6, 0x95, 0xCF, 0xa, 0x8, 0x40, 0x24, 0x4, 0x2, 0x1, 0x4D, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2f, 0xD0};
//byte pairing_packet[] = {0x7F, 0x5F, 0x01, 0x31, 0x33, 0x73, 0x13, 0x37, 0x08, 0x10, 0x25, 0x04, 0x00, 0x02, 0x0C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x71, 0x40};

//byte pairing_packet[] = {0xf2, 0x5f, 0x01, 0x4f, 0xcf, 0x9f, 0xf0, 0x0a, 0x08, 0x40, 0x24, 0x04, 0x02, 0x01, 0x4d, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x2d, 0x6f};
byte channel = 5;
byte kb_found = 0;
uint32_t kb_time = 0;

void setup(){

  Serial.begin(115200);
  printf_begin();
  Serial.println("Ready !");

  // Setup and configure rf radio
  byte init_status;
  init_status = radio.begin();
  if( init_status == 0 || init_status == 0xff) {
    Serial.println(" init failed");
    while(1){

    }
  }
  
  radio.stopListening();  
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.setRetries(2,3);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(PAYLOAD_SIZE);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.enableDynamicAck();
  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1,pipes[0]);
  radio.setChannel(channel);
  radio.setDataRate(RF24_2MBPS);
  //radio.startListening();                 // Start listening
 // radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

void loop(void) {
delay(50);
  if (role == role_ping_out){
    byte gotByte[PAYLOAD_SIZE]; 
    
    radio.stopListening();                                  // First, stop listening so we can talk.

    while(1)
    {
      delay(100);

      radio.write( pairing_packet, 22, 1);
      if(radio.write( pairing_packet_1_bis, 5, 0))
        break;      
    }
    if(!radio.available())
    {
      Serial.println("FATAL : empty ack !");
      delay(100);
      return;
    }
    radio.read( gotByte, 22 );
    pairing_set_1(gotByte);

          
      
    radio.flush_tx();
    if(counter == 10) {
      //channel = channel + 3 < 128 ? channel + 3 , 2;
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
      int pack_length = radio.read( gotByte, PAYLOAD_SIZE );
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
   
   /*if (!kb_found) {
      kb_found = 0;
      channel = channel + 3 < 128 ? channel + 3 , 2;
      radio.setChannel(channel);
      delay(100);
      //delayMicroseconds(128);
   }*/
   
 }

  // Change roles

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      //radio.printDetails();                   // Dump the configuration of the rf unit for debugging

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

void pairing_set_1(byte* ack_payload)
{
   for(int i =0; i<PAYLOAD_SIZE; i++) {
        printf("%02x ",ack_payload[i]);
      }

  printf("\nStep : %d\n", ack_payload[2]);
  printf("New address : ");

  for(int i = 0;i < 4; i++)
    printf("%02x:", ack_payload[i+3]);

  printf("%02x", ack_payload[3+4]);

    Serial.println("");

/*byte buf[22];
 radio.read( buf, 22 );
 for(int i =0; i<PAYLOAD_SIZE; i++)
    printf("%02x,", buf[i]);*/

    #define REV
 
#ifdef REV
Serial.println("Using reversed addresses");
  byte new_add[5];
  byte new_add_dongle[5];

  for(int i=0; i< 5;i++){

      new_add[i] = ack_payload[3+(4-i)];
new_add_dongle[i] = ack_payload[3+(4-i)];
  }
     new_add_dongle[0] = 0;

    #else 
    Serial.println("NOT using reversed addresses");

  byte new_add[5];
  byte new_add_dongle[5];

  for(int i=0; i< 5;i++)
   { new_add[i] = ack_payload[3+i];
   new_add_dongle[i] = ack_payload[3+i];}

     new_add_dongle[4] = 0;


    #endif

 

   
   

    


  //end tmp

radio.stopListening();
radio.openReadingPipe(1,new_add_dongle);
radio.openReadingPipe(2,new_add);
  radio.openWritingPipe(new_add_dongle);        // Both radios listen on the same pipes by default, and switch when writing



    radio.printDetails();
  
  //delay(150);   
 //  radio.openWritingPipe(&ack_payload[3]);        // Both radios listen on the same pipes by default, and switch when writing
  //radio.openReadingPipe(1,&ack_payload[3]);
//radio.startListening();
//radio.stopListening();


 // radio.printDetails();

radio.flush_tx();

if(radio.available()){
  Serial.println("Oh ! There is data in RX FIFO ! :o");
}

Serial.println("Waiting for dongle to switch address");
/*while(!radio.write( payload_pairing_2, 22, 0))
{
  delay(50);
}
radio.write( payload_pairing_2, 22, 1);
Serial.println("!");
//delay(50);
while(!radio.available()){
radio.write( payload_pairing_2, 22, 0);}*/

while(1)
    {

      if(radio.available())
          break;  

      while(!radio.write( payload_pairing_2, 22, 0)){}
      //delay(50);
      while(1)
        { 
          if(radio.write( payload_pairing_2_bis, 5, 0))
          //if(radio.write( payload_pairing_2, 22, 0))
          {  
            Serial.print('.');
            if(radio.available())
              break; 
          }               
        }
    }
Serial.println("ACK received !");

      byte ack_pay[22];
          radio.read( ack_pay, 22 );
         
           for(int i =0; i<PAYLOAD_SIZE; i++) {
        printf("%x ",ack_pay[i]);
      }
          while(1){}

  
}
