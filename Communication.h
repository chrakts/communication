/*
* Communication.h
*
* Created: 10.05.2018 19:01:34
* Author: chrak_2
*/


#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <avr/io.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Serial.h"


#define SEND_BUFFER_LENGTH 50
#define WITH_CHECKSUM		4
#define BROADCAST "BR"


class Communication: public Serial
{
//variables
public:
protected:
private:
	uint8_t retryNum;
	char sendBuffer[SEND_BUFFER_LENGTH];
	uint8_t header = 64+WITH_CHECKSUM;
	char source[3];
//functions
public:
	Communication(int UartNum, char const *mySource,int retryNumber):Serial(UartNum)
	{
		retryNum = retryNumber;
		strncpy(source,mySource,2);
	};
	~Communication();
	void transmit(uint8_t data);
	bool print(char const *text);
	bool send(char const *text,char const *target,char infoHeader,char function, char address, char job, char dataType);
	bool sendStandard(char const *text,char const *target,char function, char address, char job, char dataType);
	bool sendInfo(char const *text,char const *target);
	bool sendCommand(char const *target,char function, char address, char job);
	bool sendAlarm(char const *text,char const *target);
  bool sendWarning(char const *text,char const *target);

  void sendAnswer(char const *answer,char *answerTo, char function,char address,char job,uint8_t noerror);
  void sendPureAnswer(char *answerTo, char function,char address,char job,uint8_t noerror);
  void sendAnswerInt(char *answerTo, char function,char address,char job,uint32_t wert,uint8_t noerror);
  void sendAnswerDouble(char *answerTo, char function,char address,char job,double wert,uint8_t noerror);


protected:

private:
	Communication( const Communication &c );
	Communication& operator=( const Communication &c );

}; //Communication

#endif //__COMMUNICATION_H__
