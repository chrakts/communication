#include "Cmulti2Lora.h"


Cmulti2Lora::~Cmulti2Lora()
{
    //dtor
}

bool Cmulti2Lora::print(char const *text)
{
  txIsReady = false;
  //LoRa_txMode();                        // set tx mode
  LoRa->beginPacket();                   // start packet
  LoRa->write((uint8_t*)text,strlen(text));                  // add payload
  LoRa->endPacket(false);                 // finish packet and send it !!!!!!!!!!!!! war true

}


