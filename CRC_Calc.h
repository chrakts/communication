/*
* CRC_Calc.h
*
* Created: 17.02.2016 21:15:29
* Author: Christof
*/


#ifndef __CRC_CALC_H__
#define __CRC_CALC_H__

#include <avr/io.h>
#include<stdio.h>

class CRC_Calc
{
//variables
public:
protected:
private:
	CRC_t	*crc_base;

//functions

protected:
private:
	CRC_Calc( const CRC_Calc &c );
	CRC_Calc& operator=( const CRC_Calc &c );

public:
	CRC_Calc();
	~CRC_Calc();
	void Reset();
	void Data(uint8_t byte);
	void Data(char byte);
	void String(const char *string);
	uint8_t compare(char *data);
	uint16_t Get_CRC();
	void Get_CRC(char *result);

	uint8_t Test_CRC(char *data, char *crc, uint8_t num);


}; //CRC_Calc

#endif //__CRC_CALC_H__
