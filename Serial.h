/*
* Serial.h
*
* Created: 16.03.2017 13:43:05
* Author: a16007
*/


#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <uartHardware.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


class Serial
{
//variables
public:
protected:
private:

//functions
public:
	Serial(int UartNum);
	~Serial();

	void open(uint8_t baud, uint32_t frq);
	virtual void transmit( uint8_t data );
	void println(char *text);
	void println();
	void println(float Wert, int Stellen);
	virtual bool print(char *text);
	void print(unsigned char text,int type);
	void print_bin(uint8_t x, char ch0, char ch1);
	void print_bin(uint32_t x, char ch0='0', char ch1='1');
  void print10(unsigned long x);
  void print10(int32_t x);
	uint8_t pformat(const char *format, ...);
	bool getChar(char & got_byte);
	void input_flush( );
	char input_char();
	uint8_t input_line( char *input, uint8_t max_length, int16_t timeout );


	enum BAUDRATES{BAUD_9600=0,BAUD_19200,BAUD_28800,BAUD_38400,BAUD_57600,BAUD_76800,BAUD_115200};
	enum OUTSTYLE
	{
		DEC = -10,
		HEX = -11,
		OCT = -12
		};

protected:
private:
	int8_t PortNumber {-1} ;
	uint8_t ring_buffer_size;
	volatile uint8_t  *ring_received;
	volatile uint8_t  ring_interpreted;
	volatile char *ring_buffer; // #-#-#
	Serial( const Serial &c );
	Serial& operator=( const Serial &c );

}; //Serial


#endif //__SERIAL_H__
