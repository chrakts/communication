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
#include "CRC_Calc.h"

#define SEND_BUFFER_LENGTH 60
#define WITH_CHECKSUM		4
#define WITH_AES256 2
#define BROADCAST "BR"

extern volatile uint8_t sendFree_0;
extern volatile uint8_t sendAnswerFree_0;
extern volatile uint8_t sendFree_1;
extern volatile uint8_t sendAnswerFree_1;

enum EncryptMode {ENCRYPT_UNDEFINED=0, ENCRYPT_KEYDEFINED=1, ENCRYPT_SUBKEYDEFINED=2};

class Communication: public Serial
{
//variables
public:
protected:
  volatile uint8_t *sendFree=nullptr;
  volatile uint8_t *sendAnswerFree=nullptr;
	char sendBuffer[SEND_BUFFER_LENGTH];
	char source[3];
	char saveSource[3];
	uint8_t header = 64+WITH_CHECKSUM;
	CRC_Calc crcGlobal;

	uint8_t *random=nullptr;
	uint8_t *key;
	uint8_t subkey[16];
	AES_t   *encryptAes;   // Encrypt_aes
	uint8_t encryptStatus;
	uint8_t beSilent=false;

private:
	uint8_t retryNum;
//functions
public:
	Communication(int UartNum, char const *mySource,int retryNumber, bool busyControl):Serial(UartNum)
	{
		retryNum = retryNumber;
		strncpy(source,mySource,2);
		if(busyControl)
		{
      this->initReadMonitor(UartNum);
      this->initBusyCounter(UartNum);
		}
		if(UartNum==0)
		{
#if USE_BUSY_0==true
      sendFree = &sendFree_0;
      sendAnswerFree = &sendAnswerFree_0;
#endif // USE_BUSY_0
		}
		else
		{
#if USE_BUSY_1==true
      sendFree = &sendFree_1;
      sendAnswerFree = &sendAnswerFree_1;
#endif // USE_BUSY_1
		}
	};
	~Communication();
	void setAlternativeNode(char *altNode);
	void setEncryption(uint8_t *_random=nullptr);
  void clearEncryption();
  void encryptSetKey(uint8_t *newkey);
  uint8_t encryptDataDirect(uint8_t *data);
  uint8_t encryptDataWait();
  uint8_t encryptData(uint8_t *data);
  uint8_t decryptData(uint8_t *data);
  void getEncryptData(uint8_t *data);
  void getEncryptKey(uint8_t *keysave);

	void resetNode();
	void transmit(uint8_t data);
	virtual bool print(char const *text);
	virtual bool send(char const *text,char const *target,char infoHeader,char function, char address, char job, char dataType);
	bool sendStandard(char const *text,char const *target,char function, char address, char job, char dataType);
	bool sendByteArray(uint8_t const *bytes,size_t length,char const *target,char infoHeader,char function, char address, char job);

	void sendStandardInt(char const *target, char function,char address,char job,int32_t wert);
  void sendStandardInt64(char const *target, char function,char address,char job,int64_t wert);
	bool sendInfo(char const *text,char const *target);
	bool sendCommand(char const *target,char function, char address, char job);
	bool sendAlarm(char const *text,char const *target);
  bool sendWarning(char const *text,char const *target);
  bool sendStandardByteArray(uint8_t const *text,size_t length,char const *target,char function, char address, char job, char dataType);

  void sendAnswer(char const *answer,char *answerTo, char function,char address,char job,uint8_t noerror);
  void sendPureAnswer(char *answerTo, char function,char address,char job,uint8_t noerror);
  void sendAnswerInt(char *answerTo, char function,char address,char job,uint32_t wert,uint8_t noerror);
  void sendAnswerDouble(char *answerTo, char function,char address,char job,double wert,uint8_t noerror);
  bool broadcastFloat(float wert,char function,char address,char job);
  bool broadcastUInt8(uint8_t wert,char function,char address,char job);
  bool broadcastUInt16(uint16_t wert,char function,char address,char job);
  bool broadcastInt16(int16_t wert,char function,char address,char job);
  void initReadMonitor(uint8_t num);
  void deInitReadMonitor(uint8_t num);
  void initBusyCounter(uint8_t num);
  void setBeSilent(uint8_t sil);
  uint8_t getBeSilent();

protected:

private:
	Communication( const Communication &c );
	Communication& operator=( const Communication &c );

}; //Communication

#endif //__COMMUNICATION_H__
