/* 
* CRC_Calc.cpp
*
* Created: 17.02.2016 21:15:29
* Author: Christof
*/


#include "CRC_Calc.h"

// default constructor
CRC_Calc::CRC_Calc()
{
	crc_base = &CRC;
	crc_base->CTRL = CRC_RESET_RESET0_gc;
	crc_base->CTRL = CRC_SOURCE_IO_gc;
} //CRC_Calc

// default destructor
CRC_Calc::~CRC_Calc()
{
} //~CRC_Calc

void CRC_Calc::Reset()
{
	crc_base->CTRL = CRC_RESET_RESET0_gc;
	crc_base->CTRL = CRC_SOURCE_IO_gc;
}



void CRC_Calc::Data(uint8_t byte)
{
	crc_base->DATAIN = byte;
}

void CRC_Calc::Data(char byte)
{
	crc_base->DATAIN = byte;
}

void CRC_Calc::String(const char *string)
{
int i=0;
	while(string[i]!=0)
	{
		crc_base->DATAIN = string[i];
		i++;
	}
}

uint16_t CRC_Calc::Get_CRC()
{
	return( (((uint16_t)(crc_base->CHECKSUM1))<<8 )  | ((uint16_t)(crc_base->CHECKSUM0)));
}

void CRC_Calc::Get_CRC(char *result)
{
	sprintf(result,"%04x",Get_CRC());
}


uint8_t CRC_Calc::compare(char *data)
{
char str[5];
uint8_t res = true,i;
	sprintf(str,"%04x",Get_CRC());
	for (i=0;i<4;i++)
	{
		if(str[i]!=data[i])
		{	
			res = false;
		}
	}
	return res;
}

uint8_t CRC_Calc::Test_CRC(char *data, char *crc, uint8_t num)
{
uint8_t i,ret;
uint16_t res;
	for (i=0;i<num;i++)
	{
		Data(data[i]);
	}
	ret = true;
	res = Get_CRC();
	for (i=0;i<4;i++)
	{
		if ( crc[3-i] - 65 != (uint8_t) (res & 0x000f) )
		{
			ret = false;
		}
		res = res >> 4;
	}
	return(ret);
}