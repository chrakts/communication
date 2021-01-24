/*
* Communication.cpp
*
* Created: 10.05.2018 19:01:34
* Author: chrak_2
*/



#include "Communication.h"
#include "CRC_Calc.h"

volatile uint8_t sendFree_0;
volatile uint8_t sendAnswerFree_0;
volatile uint8_t sendFree_1;
volatile uint8_t sendAnswerFree_1;
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

void Communication::setEncryption(uint8_t *_random)
{
  header |= WITH_AES256;
  random = _random;
}

void Communication::clearEncryption()
{
  header &= ~WITH_AES256;
  random = nullptr;
}

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
int8_t l,i;
char extraInfo[5]="";
char parameterEndChar;
char crcTemp[5];
char locText[33];


  l=strlen(text);
  if(header & WITH_AES256)
  {
    if(random!=nullptr)
    {
      for (i=0;i<l;i++)
        locText[i] = random[i] ^ text[i];
      for (i=l;i<16;i++)
        locText[i] = random[i];
    }
    else
    {
      for (i=0;i<l;i++)
        locText[i] = text[i];
      for (i=l;i<16;i++)
        locText[i] = 0;
    }
    l=32;
    encryptDataDirect((uint8_t*)locText);
  }
  l+=10;
  if (header & WITH_CHECKSUM)
    l+=4;

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

  if(header & WITH_AES256)
  {
    encryptDataWait();
    getEncryptData((uint8_t*)locText);
    for(i=15;i>=0;i-=1)
      locText[2*i]=locText[i];
    char temp;
    for(i=0;i<32;i+=2)
    {
      temp=locText[i];
      locText[i] = ( (temp &0xf0)>>4 )+65;
      locText[i+1] = ( (temp &0x0f) )+65;
    }
    locText[32] = '\000';

    sprintf(sendBuffer,"#%02x%c%s%s%c%s%s%c",l,header,target,source,infoHeader,extraInfo,locText,parameterEndChar);
  }
  else
    sprintf(sendBuffer,"#%02x%c%s%s%c%s%s%c",l,header,target,source,infoHeader,extraInfo,text,parameterEndChar);
  crcGlobal.Reset();
  crcGlobal.String(sendBuffer);
  crcGlobal.Get_CRC(crcTemp);
  sprintf(sendBuffer,"%s%s\r\n",sendBuffer,crcTemp);

  clearEncryption();

  return(print(sendBuffer));
}

bool Communication::sendByteArray(uint8_t const *bytes,size_t length,char const *target,char infoHeader,char function, char address, char job)
{
int8_t j;
uint8_t i;
size_t l;
char extraInfo[5]="";
char crcTemp[5];
char locText[33];

  if(header & WITH_AES256)
  {
    if(random!=nullptr)
    {
      for (i=0;i<length;i++)
        locText[i] = random[i] ^ bytes[i];
      for (i=length;i<16;i++)
        locText[i] = random[i];
    }
    else
    {
      for (i=0;i<length;i++)
        locText[i] = bytes[i];
      for (i=length;i<16;i++)
        locText[i] = 0;
    }
    l=32;
    encryptDataDirect((uint8_t*)locText);
  }
  else
  {
    for(i=0;i<length;i++)
      locText[i] = (char) bytes[i];
    l=length*2;
  }
  l+=10;
  if (header & WITH_CHECKSUM)
    l+=4;

  if ( (infoHeader=='S') | (infoHeader=='R') | (infoHeader=='r'))
  {
    l+=3;
    extraInfo[0]=function;
    extraInfo[1]=address;
    extraInfo[2]=job;
    extraInfo[3]='T';
    extraInfo[4]=0;
  }

  if(header & WITH_AES256)
  {
    encryptDataWait();
    getEncryptData((uint8_t*)locText);
  }

  for(j=length-1;j>=0;j-=1)   // war 15
    locText[2*j]=locText[j];
  char temp;
  for(i=0;i<length*2;i+=2)
  {
    temp=locText[i];
    locText[i] = ( (temp &0xf0)>>4 )+65;
    locText[i+1] = ( (temp &0x0f) )+65;
  }
  locText[length*2] = '\000';

  sprintf(sendBuffer,"#%02x%c%s%s%c%s%s<",l,header,target,source,infoHeader,extraInfo,locText);

  crcGlobal.Reset();
  crcGlobal.String(sendBuffer);
  crcGlobal.Get_CRC(crcTemp);
  sprintf(sendBuffer,"%s%s\r\n",sendBuffer,crcTemp);

  clearEncryption();

  return(print(sendBuffer));
}

void Communication::encryptSetKey(uint8_t *newkey)
{
uint8_t dummy[16];

  key = newkey;
	encryptStatus = ENCRYPT_KEYDEFINED;
	encryptAes = &AES;
	encryptData(dummy);

}

uint8_t Communication::encryptDataDirect(uint8_t *data)
{
int i;

/*	if ( (encryptStatus&ENCRYPT_KEYDEFINED) )
	{*/
		encryptAes->CTRL = AES_RESET_bm ; // setzt das AES-Modul zurück

		for (i=0;i<16;i++)
			encryptAes->KEY=key[i];
		for (i=0;i<16;i++)
			encryptAes->STATE=data[i];
		encryptAes->CTRL = AES_START_bm ;
		return(0);
/*	}
	else
		return(-1);*/
}

uint8_t Communication::encryptDataWait()
{
int i;

	while(!(encryptAes->STATUS & AES_SRIF_bm))
		;
	for (i=0;i<16;i++)
		subkey[i] = encryptAes->KEY;
	encryptStatus |= ENCRYPT_SUBKEYDEFINED;
	return(0);
}

uint8_t Communication::encryptData(uint8_t *data)
{
	if ( encryptDataDirect(data)==0 )
	{
		encryptDataWait();
		return(0);
	}
	else
	{
		return(-1);
	}
}

uint8_t Communication::decryptData(uint8_t *data)
{
	int i;

	if (encryptStatus&ENCRYPT_SUBKEYDEFINED)
	{
		encryptAes->CTRL = AES_RESET_bm  | AES_DECRYPT_bm; // setzt das AES-Modul zurück
		_delay_us(100);
		encryptAes->CTRL = AES_DECRYPT_bm; // setzt das AES-Modul zurück

		for (i=0;i<16;i++)
			encryptAes->KEY=subkey[i];
		for (i=0;i<16;i++)
			encryptAes->STATE=data[i];
		encryptAes->CTRL = AES_START_bm  | AES_DECRYPT_bm;

		while(!(encryptAes->STATUS & AES_SRIF_bm))
			;
		//		encryptStatus = Encrypt_SubKeyDefined;
		return(0);
	}
	else
		return(-1);
}


void Communication::getEncryptData(uint8_t *data)
{
int i;
	for (i=0;i<16;i++)
		data[i] = encryptAes->STATE;
}

void Communication::getEncryptKey(uint8_t *keysave)
{
	int i;
	for (i=0;i<16;i++)
	keysave[i] = encryptAes->KEY;
}


bool Communication::sendStandard(char const *text,char const *target,char function, char address, char job, char dataType)
{
	return(send(text,target,'S',function,address,job,dataType));
}

bool Communication::sendStandardByteArray(uint8_t const *text,size_t length,char const *target,char function, char address, char job, char dataType)
{
  char *buffer;
  uint8_t i;
  buffer = (char *) malloc(length*2+1);
  for(i=0;i<length;i++)
  {
    buffer[2*i] = (text[i]>>4) + 65;
    buffer[2*i+1] = (text[i]&0x0f) + 65;
  }
  buffer[2*length] = '\000';
  i = send(buffer,target,'S',function,address,job,dataType);
	free(buffer);
	return((bool)i);
}

bool Communication::sendCommand(char const *target,char function, char address, char job)
{
	return(send("",target,'S',function,address,job,'?'));
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

bool Communication::broadcastFloat(float wert,char function,char address,char job)
{
char text[10];
  sprintf(text,"%.4f",(double)wert);
  return(sendStandard(text,BROADCAST,function,address,job,'F'));
}

bool Communication::broadcastDouble(double wert,char function,char address,char job)
{
char text[10];
  sprintf(text,"%.4f",wert);
  return(sendStandard(text,BROADCAST,function,address,job,'F'));
}

bool Communication::broadcastUInt8(uint8_t wert,char function,char address,char job)
{
char text[10];
  sprintf(text,"%hu",wert);
  return(sendStandard(text,BROADCAST,function,address,job,'T'));
}
bool Communication::broadcastUInt16(uint16_t wert,char function,char address,char job)
{
char text[10];
  sprintf(text,"%u",wert);
  return(sendStandard(text,BROADCAST,function,address,job,'T'));
}
bool Communication::broadcastInt16(int16_t wert,char function,char address,char job)
{
char text[10];
  sprintf(text,"%d",wert);
  return(sendStandard(text,BROADCAST,function,address,job,'T'));
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

void Communication::sendStandardDouble(char const *target, char function,char address,char job,double wert)
{
char str[25];
	sprintf(str,"%f",wert);
	sendStandard(str,target,function,address,job,'T');
}

void Communication::sendStandardInt64(char const *target, char function,char address,char job,int64_t wert)
{
char str[25];
	sprintf(str,"%lld",wert);
	sendStandard(str,target,function,address,job,'T');
}

void Communication::sendAnswerDouble(char *answerTo, char function,char address,char job,double wert,uint8_t noerror)
{
char temp[20];
	sprintf(temp,"%f",wert);
	sendAnswer(temp,answerTo,function,address,job,noerror);
}


bool Communication::print(char const *text)
{
  if( sendFree==nullptr )
  {
    uint8_t i;
    for(i=0;i<strlen(text);i++)
    {
      transmit(text[i]);
    }
    return(true);
    }
  else
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
      while(*sendFree==false) ;
      input_flush();
      while(j<len && (error!=true))
      {
        transmit(text[j]);
        _delay_us(50);
        *sendFree=false;     // notwendig, weil sendFree durch den Interrupt zu spaet gesetzt wird.
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
        if (*sendFree==true)		// dann ist so lange nichts mehr gesendet worden, dass ein Fehler vorliegen muss
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
        if (*sendFree==true)		// dann ist so lange nichts mehr gesendet worden, dass ein Fehler vorliegen muss
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
}

void Communication::setBeSilent(uint8_t sil)
{
  this->beSilent=sil;
}

uint8_t Communication::getBeSilent()
{
  return(this->beSilent);
}

void Communication::initReadMonitor(uint8_t num)
{
  if(num==0)
  {
    Busy_Control_Port_0.INTCTRL  |= PORT_INT0LVL_HI_gc; // High-Level interrupt 0 for PORTD
    Busy_Control_Port_0.INT0MASK = Busy_Control_Pin_0;
    Busy_Control_Port_0.Busy_Control_PinCtrl_0 = PORT_ISC_BOTHEDGES_gc | PORT_OPC_PULLUP_gc ;
  }
  else
  {
    Busy_Control_Port_1.INTCTRL  |= PORT_INT1LVL_HI_gc; // High-Level interrupt 0 for PORTD
    Busy_Control_Port_1.INT1MASK = Busy_Control_Pin_1;
    Busy_Control_Port_1.Busy_Control_PinCtrl_1 = PORT_ISC_BOTHEDGES_gc | PORT_OPC_PULLUP_gc ;
  }
}

void Communication::deInitReadMonitor(uint8_t num)
{
  if(num==0)
  {
    Busy_Control_Port_0.INTCTRL  &= ~PORT_INT0LVL_HI_gc;
    Busy_Control_Port_0.INT0MASK = 0;
    Busy_Control_Port_0.Busy_Control_PinCtrl_0 = PORT_ISC_INPUT_DISABLE_gc | PORT_OPC_PULLUP_gc ;
  }
  else
  {
    Busy_Control_Port_1.INTCTRL  &= ~PORT_INT1LVL_HI_gc;
    Busy_Control_Port_1.INT0MASK = Busy_Control_Pin_1;
    Busy_Control_Port_1.Busy_Control_PinCtrl_1 = PORT_ISC_INPUT_DISABLE_gc | PORT_OPC_PULLUP_gc ;
  }
}

void Communication::initBusyCounter(uint8_t num)
{
  if(num==0)
  {
    BUSY_TIMER.CTRLE = TC2_BYTEM_SPLITMODE_gc;
    BUSY_TIMER.CTRLA = TC2_CLKSEL_DIV256_gc;
    BUSY_TIMER.CTRLB = 0;
    BUSY_TIMER.INTCTRLA = TC2_LUNFINTLVL_HI_gc;
    BUSY_TIMER.LCNT = 128; // 128
    BUSY_TIMER.LPER = 42;
  }
  else
  {
    BUSY_TIMER.CTRLE = TC2_BYTEM_SPLITMODE_gc;
    BUSY_TIMER.CTRLA = TC2_CLKSEL_DIV256_gc;
    BUSY_TIMER.CTRLB = 0;
    BUSY_TIMER.INTCTRLA = TC2_LUNFINTLVL_HI_gc;
    BUSY_TIMER.HCNT = 128; // 128
    BUSY_TIMER.HPER = 42;
  }
}

#if USE_BUSY_0 == true
ISR( Busy_Control_IntVec_0 )
{
  sendFree_0 = false;
  sendAnswerFree_0 = false;
  BUSY_TIMER.LCNT = 20;
//  LED_ROT_ON;
}
#endif // USE_BUSY_0

#if USE_BUSY_1 == true
ISR( Busy_Control_IntVec_1 )
{
  sendFree_1 = false;
  sendAnswerFree_1 = false;
  BUSY_TIMER.LCNT = 20;
  //LED_ROT_ON;
}
#endif // USE_BUSY_1

#if USE_BUSY_0 == true
ISR ( Busy_Control_TimVec_0 )
{
  if(sendAnswerFree_0 == true )
  {
      sendFree_0 = true;
      //LED_ROT_OFF;
  }
  else
      sendAnswerFree_0 = true;
  BUSY_TIMER.LCNT = 20;
}
#endif // USE_BUSY_0

#if USE_BUSY_1==true
ISR ( Busy_Control_TimVec_1 )
{
  if(sendAnswerFree_1 == true )
  {
      sendFree_1 = true;
      //LED_ROT_OFF;
  }
  else
      sendAnswerFree_1 = true;
  BUSY_TIMER.HCNT = 20;
}
#endif // USE_BUSY_1
