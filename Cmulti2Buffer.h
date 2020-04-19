#ifndef CMULTI2BUFFER_H
#define CMULTI2BUFFER_H

#include <Communication.h>
#include "../loraSupervisor/Operators.h"

#define WITH_CHECKSUM		4
#define BROADCAST "BR"

class Cmulti2Buffer : public Communication
{
  public:
    Cmulti2Buffer(char *globBuffer, uint8_t lenGlobBuffer, char const *mySource):Communication(0, mySource,0,false)
    {
      buffer = lokBuffer;
      buffer = new char[lenGlobBuffer];
      length = lenGlobBuffer;
    }
    virtual ~Cmulti2Buffer();
    bool print(char const *text);
    char *get();
    void set(char *text);
    bool send(char const *text,char const *target,char infoHeader,char function, char address, char job, char dataType);


  protected:

  private:
  char *buffer;
  uint8_t length;
  char lokBuffer[40];
};

#endif // CMULTI2BUFFER_H
