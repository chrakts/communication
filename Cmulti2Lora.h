#ifndef CMULTI2LORA_H
#define CMULTI2LORA_H


#include <Communication.h>
#include "../LoraSensor/Operators.h"
#include "LoRa.h"

#define WITH_CHECKSUM		4
#define BROADCAST "BR"

#define LORAMESSAGELENGTH 20

class Cmulti2Lora : public Communication
{
    public:
        Cmulti2Lora(LoRaClass *lora, char const *mySource):Communication(0, mySource,0,false)
        {
            LoRa = lora;
        }
        bool print(char const *text);
        virtual ~Cmulti2Lora();

    protected:

        struct relaisInfo
        {
          char   target[3];
          char   source[3];
          char   function;
          char   address;
          char   job;
          char   endChar;
          char   content[LORAMESSAGELENGTH];
          uint8_t medium;
        };
        typedef struct relaisInfo RelaisInfo;

    private:
        RelaisInfo relinfo[5];
        LoRaClass *LoRa;
};

#endif // CMULTI2LORA_H
