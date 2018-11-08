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


#define SEND_BUFFER_LENGTH 36
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
	Communication(int UartNum, char *mySource,int retryNumber):Serial(UartNum)
	{
		retryNum = retryNumber;
		strncpy(source,mySource,2);
	};
	~Communication();
	void transmit(uint8_t data);
	bool print(char *text);
	bool send(char *text,char *target,char infoHeader,char function, char job, char dataType);
	bool sendStandard(char *text,char *target,char function, char job, char dataType);
	bool sendInfo(char *text,char *target);
	bool sendCommand(char *target,char function, char job);

protected:

private:
	Communication( const Communication &c );
	Communication& operator=( const Communication &c );

}; //Communication

#endif //__COMMUNICATION_H__
