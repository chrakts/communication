/*
* Communication.cpp
*
* Created: 10.05.2018 19:01:34
* Author: chrak_2
*/



#include "Communication.h"
#include "CRC_Calc.h"

CRC_Calc crcGlobal;
uint8_t sendFree;

// default constructor
/*Communication(int UartNum):Serial(UartNum)
{
} //Communication
*/
// default destructor
Communication::~Communication()
{
} //~Communication

void Communication::transmit(uint8_t data)
{
	Serial::transmit(data);
}

bool Communication::send(char *text,char *target,char infoHeader,char function, char job, char dataType)
{
uint8_t l;
char extraInfo[4]="";
char crcTemp[5];


	if (header&WITH_CHECKSUM)
	{
		l=strlen(text)+13;
	}
	else
	{
		l=strlen(text)+9;
	}
	if (infoHeader=='S')
	{
		l+=3;
		extraInfo[0]=function;
		extraInfo[1]=job;
		extraInfo[2]=dataType;
		extraInfo[3]=0;
	}
	sprintf(sendBuffer,"#%02x%c%s%s%c%s%s<",l,header,target,source,infoHeader,extraInfo,text);
	crcGlobal.Reset();
	crcGlobal.String(sendBuffer);
	crcGlobal.Get_CRC(crcTemp);
	sprintf(sendBuffer,"%s%s\r\n",sendBuffer,crcTemp);
	return(print(sendBuffer));
}

bool Communication::sendStandard(char *text,char *target,char function, char job, char dataType)
{
	send(text,target,'S',function,job,dataType);
}

bool Communication::sendCommand(char *target,char function, char job)
{
	send("",target,'S',function,job,'0');
}

bool Communication::sendInfo(char *text,char *target)
{
	send(text,target,'I','-','-','-');
}


bool Communication::print(char *text)
{
	int len = strlen(text);
	char c='+';
	int retries=0;
	bool ret = false;
	while((ret==false) && (retries<retryNum))
	{
		bool error = false;
		int i = 0;			// Antwort-Zeichen empfangen
		int j = 0;			// Zeichen zur Uebertragung
		while(sendFree==false) ;
		input_flush();
		while(j<len && (error!=true))
		{
			transmit(text[j]);
			sendFree=false;     // notwendig, weil sendFree durch den Interrupt zu spaet gesetzt wird.
			j++;
			if (getChar(c))
			{
                if (text[i]!=c)
                {
                    error = true;
                    break;
                }
                i++;
			}
			if (sendFree==true)		// dann ist so lange nichts mehr gesendet worden, dass ein Fehler vorliegen muss
			{
				error=true;
			}
		}
		while( (i<len) && (error!=true) )
		{
			if (getChar(c))
			{
				if (text[i]!=c)
				{
					error = true;
					break;
				}
				i++;
			}
			if (sendFree==true)		// dann ist so lange nichts mehr gesendet worden, dass ein Fehler vorliegen muss
			{
				error=true;
			}
		}
		retries++;
		ret = (i==j);
	}
	if (ret==false)
	{
/*		debug.println("!!!!!!!!!!!! Mega-Fault !!!!!!!!!!!");
		debug.print(":");
		debug.print_bin(c);
		debug.print(":");
		debug.print(text);*/
	}
	return(ret);
}
