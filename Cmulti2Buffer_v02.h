#ifndef CMULTI2BUFFER_V02_H
#define CMULTI2BUFFER_V02_H

#include <Communication.h>
#include "../LoraSensor/Operators.h"

#define WITH_CHECKSUM		4
#define BROADCAST "BR"

class Cmulti2Buffer_v02 : public Communication
{
  public:
    Cmulti2Buffer_v02(char *globBuffer, uint8_t lenGlobBuffer, char const *mySource):Communication(0, mySource,0,false)
    {
      buffer = lokBuffer;
      buffer = new char[lenGlobBuffer];
      length = lenGlobBuffer;
    }
    virtual ~Cmulti2Buffer_v02();
    bool print(char const *text);
    char *get();
    void set(char *text);


  protected:

  private:
  char *buffer;
  uint8_t length;
  char lokBuffer[40];
};

#endif // CMULTI2BUFFER_V02_H
