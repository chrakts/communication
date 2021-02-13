/*
* Serial.cpp
*
* Created: 16.03.2017 13:43:04
* Author: a16007
*/


#include "Serial.h"
#include "uartHardware.h"

volatile uint8_t UART0_ring_received;
volatile  char UART0_ring_buffer[UART0_RING_BUFFER_SIZE];
volatile uint8_t UART1_ring_received;
volatile  char UART1_ring_buffer[UART1_RING_BUFFER_SIZE];


// enum-Liste der Baudraten in Serial.h in der Klassendefinition
uint8_t CTRL_A_32M[] = {254,229,71,197,55,133,23};
uint8_t CTRL_B_32M[] = {204,188,196,172,180,156,164};

uint8_t CTRL_A_2M[] = {3,193,107,33,75,5,11};
uint8_t CTRL_B_2M[] = {150,146,176,145,160,208,144};

uint8_t CTRL_A_18432K[] = {119,59,39,29,19,14,9};
uint8_t CTRL_B_18432K[] = {0,0,0,0,0,0,0};

uint8_t CTRL_A_22118400[] = {143,71,47,35,23,17,11};
uint8_t CTRL_B_22118400[] = {0,0,0,0,0,0,0};

uint8_t CTRL_A_29491200[] = {191,95,63,47,31,23,15};
uint8_t CTRL_B_29491200[] = {0,0,0,0,0,0,0};

uint8_t CTRL_A_36864K[] = {238,118,78,58,38,28,18};
uint8_t CTRL_B_36864K[] = {0,0,0,0,0,0,0};


// default constructor
Serial::Serial(int UartNum)
{
	PortNumber = UartNum;

} //Serial

void Serial::open(uint8_t baud, uint32_t frq)
{

	if (PortNumber==0)
	{
	    SERIAL_PORT_0.DIRSET = SERIAL_TX_PIN_0;
	    SERIAL_PORT_0.DIRCLR = SERIAL_RX_PIN_0;
#ifdef USE_RS485_0
      SERIAL_PORT_0.DIRSET = SERIAL_TE_PIN_0 | SERIAL_RE_PIN_0;
      SERIAL_PORT_0.OUTCLR = SERIAL_TE_PIN_0;
#pragma message "Verwende RS485 bei UART0"
#endif
#ifdef USE_RS485_FEEDBACK_0
      SERIAL_PORT_0.OUTCLR = SERIAL_RE_PIN_0;
#pragma message "Verwende Feedback bei UART0"
#else
      SERIAL_PORT_0.OUTSET = SERIAL_RE_PIN_0;
#endif // USE_RS485_FEEDBACK_0
		ring_received = &UART0_ring_received;
		ring_buffer   = UART0_ring_buffer; // #-#-#
		((USART_t *) &SERIAL_0)->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
		#ifdef USE_RS485_0
			((USART_t *) &SERIAL_0)->CTRLA = USART_RXCINTLVL_0 | USART_TXCINTLVL_0;
			TE_DISABLE_0;
			RE_ENABLE_0;
		#else // RS232 braucht keinen TX-Interrupt
			((USART_t *) &SERIAL_0)->CTRLA = USART_RXCINTLVL_0;
		#endif
		ring_buffer_size = UART0_RING_BUFFER_SIZE;
	}
	else
	{
	    SERIAL_PORT_1.DIRSET = SERIAL_TX_PIN_1;
	    SERIAL_PORT_1.DIRCLR = SERIAL_RX_PIN_1;
#ifdef USE_RS485_1
      SERIAL_PORT_1.DIRSET = SERIAL_TE_PIN_1 | SERIAL_RE_PIN_1;
      SERIAL_PORT_1.OUTCLR = SERIAL_TE_PIN_1;
#pragma message "Verwende RS485 bei UART1"
#endif
#ifdef USE_RS485_FEEDBACK_1
      SERIAL_PORT_1.OUTCLR = SERIAL_RE_PIN_1;
#pragma message "Verwende Feedback bei UART1"
#else
      SERIAL_PORT_1.OUTSET = SERIAL_RE_PIN_1;
#endif // USE_RS485_FEEDBACK_1
		ring_received = &UART1_ring_received;
		ring_buffer   = UART1_ring_buffer; // #-#-#
		((USART_t *) &SERIAL_1)->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
		#ifdef USE_RS485_1
			((USART_t *) &SERIAL_1)->CTRLA = USART_RXCINTLVL_1 | USART_TXCINTLVL_1;
			TE_DISABLE_1;
			RE_ENABLE_1;
		#else // RS232 braucht keinen TX-Interrupt
			((USART_t *) &SERIAL_1)->CTRLA = USART_RXCINTLVL_1;
		#endif
		ring_buffer_size = UART1_RING_BUFFER_SIZE;
	}
	ring_interpreted = 0;
	*ring_received = 0;


  uint8_t tempA,tempB;
	switch(frq)
	{
		case 32000000:
			tempA = CTRL_A_32M[baud];
			tempB = CTRL_B_32M[baud];
		break;
		case 18432000:
			tempA = CTRL_A_18432K[baud];
			tempB = CTRL_B_18432K[baud];
		break;
		case 2000000:
			tempA = CTRL_A_2M[baud];
			tempB = CTRL_B_2M[baud];
		break;
		case 22118400:
			tempA = CTRL_A_22118400[baud];
			tempB = CTRL_B_22118400[baud];
    break;
		case 29491200:
			tempA = CTRL_A_29491200[baud];
			tempB = CTRL_B_29491200[baud];
		break;
		case 36864000:
			tempA = CTRL_A_36864K[baud];
			tempB = CTRL_B_36864K[baud];
		break;
		default:
			tempA = CTRL_A_32M[baud];
			tempB = CTRL_B_32M[baud];
    break;
	}

	if (PortNumber==0)
	{
		((USART_t *) &SERIAL_0)->BAUDCTRLA = tempA;
		((USART_t *) &SERIAL_0)->BAUDCTRLB = tempB;
	}
	else
	{
		((USART_t *) &SERIAL_1)->BAUDCTRLA = tempA;
		((USART_t *) &SERIAL_1)->BAUDCTRLB = tempB;
	}
}



void Serial::transmit( uint8_t data )
{
	if (PortNumber==0) // Achtung: Implementierung entsprcht noch nicht dem USE_RS485_1 !!!!!!!!!!!!!!!!!!!!!!!!!!!
	{
		#ifdef USE_RS485_0
			while (  ( ((PORT_t *) &SERIAL_PORT_0)->IN & SERIAL_TE_PIN_0 ) != 0  )
			;
    #ifdef USE_RS485_FEEDBACK_0
      //((USART_t *) &SERIAL_0)->CTRLB = USART_TXEN_bm | USART_RXEN_bm; // ############################
      //_delay_us(50);// ############################
    #else
      ((USART_t *) &SERIAL_0)->CTRLB = USART_TXEN_bm;
      ((USART_t *) &SERIAL_0)->CTRLA = USART_TXCINTLVL_0;
      RE_DISABLE_0;
    #endif
      TE_ENABLE_0;
      _delay_us(10); // mindestens 8 beim atxmega256a3u, 5 bei stxmega32a4u
      ((USART_t *) &SERIAL_0)->DATA = data; //	UDR0 = data; 			        // Start transmittion
		#else
			while( (  ((USART_t *) &SERIAL_0)->STATUS & USART_DREIF_bm ) == 0 );
			((USART_t *) &SERIAL_0)->DATA = data;
			#pragma message "Reiner RS232-Betrieb fuer UART0 noch nicht getestet"
		#endif
	}
	else
	{
		#ifdef USE_RS485_1
			while (  ( ((PORT_t *) &SERIAL_PORT_1)->IN & SERIAL_TE_PIN_1 ) != 0  )
			;
    #ifdef USE_RS485_FEEDBACK_1
    #else
      ((USART_t *) &SERIAL_1)->CTRLB = USART_TXEN_bm;
      ((USART_t *) &SERIAL_1)->CTRLA = USART_TXCINTLVL_1;
      RE_DISABLE_1;
    #endif
      TE_ENABLE_1;
      _delay_us(10); // mindestens 8 beim atxmega256a3u, 5 bei stxmega32a4u
      ((USART_t *) &SERIAL_1)->DATA = data; //	UDR0 = data; 			        // Start transmittion
		#else
			while( (  ((USART_t *) &SERIAL_1)->STATUS & USART_DREIF_bm ) == 0 );
			((USART_t *) &SERIAL_1)->DATA = data;
		#endif
	}
}



// default destructor
Serial::~Serial()
{
} //~Serial

void Serial::println(const char *text)
{
	print(text);
	print((char *)EOL);
}
void Serial::println()
{
	print((char *)EOL);
}

void Serial::println(float Wert, int Stellen)
{
	pformat("%f",Wert);
}

bool Serial::print(const char *text)
{
	while(*text)
	{
		transmit(*text);
		text++;
	}
	return(true);
}

void Serial::print10(unsigned long x)
{
	unsigned  long int y;
	if (x<4294967295ul)
	{
		y=x/1000000000;transmit( y+0x30);x-=(y*1000000000);
		y=x/100000000;transmit( y+0x30);x-=(y*100000000);
		y=x/10000000;transmit( y+0x30);x-=(y*10000000);
		y=x/1000000;transmit( y+0x30);x-=(y*1000000);
		y=x/100000;transmit( y+0x30);x-=(y*100000);
		y=x/10000;transmit( y+0x30);x-=(y*10000);
		y=x/1000;transmit( y+0x30);x-=(y*1000);
		y=x/100;transmit( y+0x30);x-=(y*100);
		y=x/10;transmit( y+0x30);x-=(y*10);
		transmit( x+0x30);
	}
	else print((char *)"Err");

}

void Serial::print10(int32_t x)
{
	int32_t y;
  if( x&0x80000000 )
    transmit('-');
  else
    transmit('+');
  x = x & 0x7fffffff;
	if (x<2147483647L)
	{
		y=x/1000000000;transmit( y+0x30);x-=(y*1000000000);
		y=x/100000000;transmit( y+0x30);x-=(y*100000000);
		y=x/10000000;transmit( y+0x30);x-=(y*10000000);
		y=x/1000000;transmit( y+0x30);x-=(y*1000000);
		y=x/100000;transmit( y+0x30);x-=(y*100000);
		y=x/10000;transmit( y+0x30);x-=(y*10000);
		y=x/1000;transmit( y+0x30);x-=(y*1000);
		y=x/100;transmit( y+0x30);x-=(y*100);
		y=x/10;transmit( y+0x30);x-=(y*10);
		transmit( x+0x30);
	}
	else print((char *)"Err");

}

void Serial::print(unsigned char text,int type)
{

}

/*-----------------  19.03.2017  --------------------
 *  Display 8bit bin value
 *   print_bin(0xAA,		 -> 10101010
 *   print_bin(0xAA,'0','1') -> 10101010
 *   print_bin(0xAA,'_','*') -> *_*_*_*_
 *--------------------------------------------------*/
void Serial::print_bin(uint8_t x, char ch0, char ch1)
{
  uint8_t i;
  for (i=128;i>0;i>>=1)
  {
    if ((x&i)==0) transmit(ch0);
    else transmit(ch1);
  }
}

/*-----------------  19.03.2017  --------------------
 *  Display 8bit bin value
 *   print_bin(0xAA,		 -> 10101010
 *   print_bin(0xAA,'0','1') -> 10101010
 *   print_bin(0xAA,'_','*') -> *_*_*_*_
 *--------------------------------------------------*/
void Serial::print_bin(uint32_t x, char ch0, char ch1)
{
  uint32_t i;
  for (i=0x80000000;i>0;i>>=1)
  {
    if ((x&i)==0) transmit(ch0);
    else transmit(ch1);
  }
}

/*-----------------28.08.99 22:49-------------------
 *   Simple pformat function (no fp, and strings),
 *   but it works for signed numbers
 *--------------------------------------------------*/

uint8_t Serial::pformat(const char *format, ...)
{
  static const char hex[]= "0123456789ABCDEF";
  char format_flag;
  unsigned int u_val, div_val, base;
//  char *ptr;
  va_list ap;

  va_start (ap, format);
  for (;;) {
     while ((format_flag = *format++) != '%') {
       if (!format_flag) {
         va_end (ap);
         return (0);
       }
       transmit(format_flag);
     }

     switch (format_flag = *format++) {
       case 'c': format_flag = va_arg(ap,int);
       default:  transmit(format_flag); continue;

       case 'd': base = 10; div_val = 10000; goto CONVERSION_LOOP;
       case 'x': base = 16; div_val = 0x10;

       CONVERSION_LOOP:
       u_val = va_arg(ap,int);
       if (format_flag == 'd') {
         if (((int)u_val) < 0) {
           u_val = - u_val;
           transmit('-');
         }
         while (div_val > 1 && div_val > u_val) div_val /= 10;
       }
       do {
         transmit(hex[u_val / div_val]);
         u_val %= div_val;
         div_val /= base;
       } while (div_val);
    }
  }
}

bool Serial::getChar(char & gotByte) // gibt Zeichen zurueck, wartet aber nicht
{
	if(*ring_received!=ring_interpreted) //
	{
		(ring_interpreted)++;
		if(ring_interpreted == ring_buffer_size)  //RING_BUFFER_SIZE
			ring_interpreted = 0;
		gotByte = (ring_buffer)[ring_interpreted]; // #-#-#
		return( true );
	}
	else
		return( false );
}

void Serial::input_flush( )
{
	ring_interpreted = *ring_received;
}

char Serial::input_char()			// wartet bis Zeichen eingegeben wird
{
	bool temp_int=false;
	char temp_char;
	do
	{
		temp_int = getChar( temp_char );
	}
	while( temp_int==false );
	return( temp_char );
}
/*
uint8_t Serial::input_line( char *input, uint8_t max_length, int16_t timeout )
{
	uint8_t temp,zeiger;
	char gotchar;

	//	input_flush(file);

	zeiger = 0;
	temp = false;
	MyTimers[TIMER_TIMEOUT].value = timeout;
	MyTimers[TIMER_TIMEOUT].state = TM_START;
	do
	{
		if(getChar(gotchar)) // **********************************************************
		{
			input[zeiger]=gotchar;
			if(			(zeiger == max_length-2)
				| ( (input[zeiger]=='\\')&&(input[zeiger-1]=='<') )
				| ( (input[zeiger]=='>')&&(input[zeiger-1]=='.') )
				| ( (input[zeiger]=='>')&&(input[zeiger-1]=='!') )
				| ( input[zeiger]==13 )
			)
				temp = true;
			else
				zeiger++;
		}
	}
	while(  (temp==false) && (MyTimers[TIMER_TIMEOUT].state!=TM_STOP)  );
	input[zeiger+1] = 0;
	//	PMIC_CTRL = PMIC_LOLVLEX_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm;

	if (temp==true)
	{
		_delay_us(250);
		return( zeiger );
	}
	else
	{
		return( zeiger );
	}
}
*/

SIGNAL(R_COMPLETE_INT_0)
{
char c;

	c= ((USART_t *) &SERIAL_0)->DATA;

// das auskommentierte braucht man nur, wenn der RS485 durch das Umschalten falsche Nullen schickt
	if (c!=0)
	{
    UART0_ring_received++;
    if(UART0_ring_received == UART0_RING_BUFFER_SIZE)
      UART0_ring_received = 0;
    UART0_ring_buffer[UART0_ring_received]=c;
	}
}

SIGNAL(R_COMPLETE_INT_1)
{
char c;

	c= ((USART_t *) &SERIAL_1)->DATA;

// das auskommentierte braucht man nur, wenn der RS485 durch das Umschalten falsche Nullen schickt
//	if (c!=0)
//	{
		UART1_ring_received++;
		if(UART1_ring_received == UART1_RING_BUFFER_SIZE)
            UART1_ring_received = 0;
		UART1_ring_buffer[UART1_ring_received]=c;
//	}


}

SIGNAL(T_COMPLETE_INT_0)
{
	#ifdef USE_RS485_0
		TE_DISABLE_0;
		((USART_t *) &SERIAL_0)->STATUS = USART_RXCIF_bm;
		#ifdef USE_RS485_FEEDBACK_0
			//((USART_t *) &SERIAL_0)->CTRLB = USART_RXEN_bm; // #####################################################
		#else
			RE_ENABLE_0;
			((USART_t *) &SERIAL_0)->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
			((USART_t *) &SERIAL_0)->CTRLA = USART_RXCINTLVL_0 | USART_TXCINTLVL_0;
		#endif
	#else

	#endif
}

SIGNAL(T_COMPLETE_INT_1)
{
	#ifdef USE_RS485_1
		TE_DISABLE_1;
		((USART_t *) &SERIAL_1)->STATUS = USART_RXCIF_bm;
		#ifdef USE_RS485_FEEDBACK_1
		#else
			RE_ENABLE_1;
			((USART_t *) &SERIAL_1)->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
			((USART_t *) &SERIAL_1)->CTRLA = USART_RXCINTLVL_1 | USART_TXCINTLVL_1;
		#endif
	#else
	#endif
}


