#include "Cmulti2Buffer_v02.h"


Cmulti2Buffer_v02::~Cmulti2Buffer_v02()
{
  //dtor
}

bool Cmulti2Buffer_v02::print(char const *text)
{
uint8_t l;
  l = strlen(text);
  if(l<length-1)
  {
    strncpy(buffer,text,l);
    buffer[l] = '\0';
    return(true);
  }
  else
    return false;
}

char * Cmulti2Buffer_v02::get()
{
  return buffer;
}

void Cmulti2Buffer_v02::set(char *text)
{
  strncpy(buffer,text,length-1);
}

