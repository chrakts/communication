/*
* Communication.cpp
*
* Created: 10.05.2018 19:01:34
* Author: chrak_2
*/



#include "Communication.h"
#include "CRC_Calc.h"

//#include "ledHardware.h"


volatile uint8_t sendFree;
volatile uint8_t sendAnswerFree;
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

// default constructor
/*Communication(int UartNum):Serial(UartNum)
{
} //Communication
*/
// default destructor
Communication::~Communication()
{
} //~Communication

void Communication::setAlternativeNode(char *altNode)
{
  strcpy(saveSource,source);
  strcpy(source,altNode);
}

void Communication::resetNode()
{
  strcpy(source,saveSource);
}

void Communication::transmit(uint8_t data)
{
	Serial::transmit(data);
}

bool Communication::send(char const *text,char const *target,char infoHeader,char function, char address, char job, char dataType)
{
uint8_t l;
char extraInfo[5]="";
char parameterEndChar;
char crcTemp[5];


	if (header&WITH_CHECKSUM)
	{
		l=strlen(text)+14;
	}
	else
	{
		l=strlen(text)+10;
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
	sprintf(sendBuffer,"#%02x%c%s%s%c%s%s%c",l,header,target,source,infoHeader,extraInfo,text,parameterEndChar);
	crcGlobal.Reset();
	crcGlobal.String(sendBuffer);
	crcGlobal.Get_CRC(crcTemp);
	sprintf(sendBuffer,"%s%s\r\n",sendBuffer,crcTemp);
	return(print(sendBuffer));
}

bool Communication::sendStandard(char const *text,char const *target,char function, char address, char job, char dataType)
{
	return(send(text,target,'S',function,address,job,dataType));
}

bool Communication::sendCommand(char const *target,char function, char address, char job)
{
	return(send("",target,'S',function,address,job,'0'));
}

bool Communication::sendInfo(char const *text,char const *target)
{
	return(send(text,target,'I','-','-','-','-'));
}

bool Communication::sendAlarm(char const *text,char const *target)
{
	return(send(text,target,'A','-','-','-','-'));
}

bool Communication::sendWarning(char const *text,char const *target)
{
	return(send(text,target,'W','-','-','-','-'));
}

// alt: void Communication::sendAnswer(char *answerTo, char function,char address,char job,char const *answer,uint8_t noerror)
void Communication::sendAnswer(char const *answer,char *answerTo, char function,char address,char job,uint8_t noerror)
{
//char tempString[36];
//	LED_ROT_ON;

	char iHeader='R';
	if(!noerror)
		iHeader = 'r';
  char dataType = '?';
  if(strlen(answer)!=0)
      dataType = 'T';
//	sprintf(tempString,"%c%c%c%s%c",function,address_KNET,job,answer,sign);
	send(answer,answerTo,iHeader,function,address,job,dataType);
//	LED_ROT_OFF; #17DMeH1.36.260311<07d4
}

void Communication::sendPureAnswer(char *answerTo, char function,char address,char job,uint8_t noerror)
{
	sendAnswer("",answerTo,function,address,job,noerror);
}

void Communication::sendAnswerInt(char *answerTo, char function,char address,char job,uint32_t wert,uint8_t noerror)
{
char str[25];
	sprintf(str,"%lu",wert);
	sendAnswer(str,answerTo,function,address,job,noerror);
}

void Communication::sendStandardInt(char const *target, char function,char address,char job,int32_t wert)
{
char str[25];
	sprintf(str,"%ld",wert);
	sendStandard(str,target,function,address,job,'T');
}

void Communication::sendAnswerDouble(char *answerTo, char function,char address,char job,double wert,uint8_t noerror)
{
char temp[20];
	sprintf(temp,"%f",wert);
	sendAnswer(temp,answerTo,function,address,job,noerror);
}

#ifdef NO_BUSY_CONTROL
bool Communication::print(char const *text)
{
	uint8_t i;
  for(i=0;i<strlen(text);i++)
  {
    transmit(text[i]);
  }
  return(true);
}

#else
bool Communication::print(char const *text)
{
	int len = strlen(text);
	char c;
	int retries=0;
	bool ret = false;
	while((ret==false) && (retries<retryNum))
	{
		bool error = false;
		int i = 0;			// Zeichen empfangen
		int j = 0;			// Zeichen zur Uebertragung
		while(sendFree==false) ;
		input_flush();
		while(j<len && (error!=true))
		{
			transmit(text[j]);
			_delay_us(50);
			sendFree=false;     // notwendig, weil sendFree durch den Interrupt zu spaet gesetzt wird.
			j++;
			if (getChar(c))
			{
				if (text[i]!=c)
				{
/*					debug.print("fault1");
					debug.print(":");
					debug.print(text);*/
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
/*					debug.print("fault2");
					debug.print(":");
					debug.print(text);*/
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
		debug.print(text);*/
	}
	return(ret);
}

#endif // NO_BUSY_CONTROL


