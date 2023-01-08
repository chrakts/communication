#include "Cmulti2Buffer.h"



Cmulti2Buffer::~Cmulti2Buffer()
{
  //dtor
}

bool Cmulti2Buffer::print(char const *text)
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

char * Cmulti2Buffer::get()
{
  return buffer;
}

void Cmulti2Buffer::set(char *text)
{
  strncpy(buffer,text,length-1);
}


bool Cmulti2Buffer::send(char const *text,char const *target,char infoHeader,char function, char address, char job, char dataType)
{
uint8_t l;
char extraInfo[5]="";
char parameterEndChar;
char crcTemp[5];


	if (header&WITH_CHECKSUM)
	{
		l=strlen(text)+9;
	}
	else
	{
		l=strlen(text)+5;
	}
	if ( (infoHeader=='S') | (infoHeader=='R') | (infoHeader=='r'))
	{
		l+=3;
		extraInfo[0]=function;
		extraInfo[1]=address;
		extraInfo[2]=job;
		extraInfo[3]=dataType;
		extraInfo[4]=0;
	}
	if( strlen(text)>0 )
    parameterEndChar='<';
  else
    parameterEndChar=0;
	//sprintf(sendBuffer,"#%02x%c%s%s%c%s%s<",l,header,target,source,infoHeader,extraInfo,text);
	sprintf(sendBuffer,"%c%s%s%c%s%s%c",header,target,source,infoHeader,extraInfo,text,parameterEndChar);
  if (header & WITH_CHECKSUM)
  {
    crcGlobal.Reset();
    crcGlobal.String(sendBuffer);
    crcGlobal.Get_CRC(crcTemp);
    sprintf(sendBuffer,"%s%s",sendBuffer,crcTemp);
  }
	return(print(sendBuffer));
}
