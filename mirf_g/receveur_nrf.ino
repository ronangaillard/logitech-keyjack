/*
    réception d’une valeur integer via module nRF24L01
*/

#include <SPI.h> // gestion du bus SPI
#include <Mirf.h> // gestion de la communication
#include <nRF24L01.h> // définition des registres du nRF24L01
#include <MirfHardwareSpiDriver.h> // communication SPI nRF24L01

byte valeur[22];  // contient la valeur à recevoir
byte valeur_octet[2]; // contient la valeur découpée en octet pour la réception
byte pairing_packet[] = {0xA1, 0x5F, 0x1, 0xFB, 0xD6, 0x95, 0xCF, 0x8, 0x8, 0x40, 0x24, 0x4, 0x2, 0x1, 0x4D, 0x0, 0x0, 0x0, 0x0, 0x0, 0x32, 0xD0};

void setup()
{
  uint8_t base_mac_address[] = {0xBB, 0x0A, 0xDC, 0xA5, 0x75};
  Mirf.cePin = 8; // CE sur broche 8
  Mirf.csnPin = 7; // CSN sur broche 7
  Mirf.spi = &MirfHardwareSpi; // utilisation du port SPI hardware
  Mirf.init(); // initialise le module SPI
  Mirf.channel = 2; // utilisation canal 0 pour communiquer (128 canaux disponible, de 0 à 127)
  Mirf.payload = 22; // = 2, déclaration taille du message à transmettre, max 32 octets
  // RF_SETUP=0000abcd : a=1–>2Mb/s, a=0–>1Mb/s; puissance émission bc=00–>-18 dBm, bc=01–>-12dBm, bc=10–>-6dBm, bc=11–>0dBm;
  // d=0 pas de gain sur le bruit en réception
  Mirf.configRegister(RF_SETUP, 0x0E); // 1 Mb/s et 0 dBm (puissance maximum)
  Mirf.config(); // configure le canal et la taille du message
  Mirf.setTADDR(base_mac_address); // définition adresse sur 5 octets de la 2ème carte Arduino
  Mirf.setRADDR(base_mac_address); // définition adresse sur 5 octets de la 1ere carte Arduino
  Serial.begin(230400);
}

void loop()
{
  while(Mirf.isSending())
  {
    // en cours d’émission
  }
  while(!Mirf.dataReady())
  {
    // pas prêt à recevoir
    //delay(5);
  }
  Mirf.getData(valeur); // récupére 2 octets
  //valeur = valeur_octet[0] + (valeur_octet[1] << 8); // transforme en int
  for(int i =0;i<22;i++) {
    Serial.println(valeur[i], HEX);
  }
    Serial.println();
    while(Mirf.isSending())
    {
      // en cours d’émission
    }
    Mirf.send(pairing_packet);
  delay(1000);
}
