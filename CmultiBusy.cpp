/*
 * CmultiBusy.cpp
 *
 * Created: 24.05.2018 13:30:48
 *  Author: chrak_2
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <uartHardware.h>

extern uint8_t sendFree;

/* Hier wird der Pin konfiguriert, der die Interruptueberwachung bekommt, ob der Bus belegt ist */

void initReadMonitor()
{
	Busy_Control_Port.INTCTRL  = PORT_INT1LVL0_bm ; // Low-Level interrupt 0 for PORTD
	Busy_Control_Port.INT1MASK = Busy_Control_Pin;
	Busy_Control_Port.Busy_Control_PinCtrl = PORT_ISC_BOTHEDGES_gc | PORT_OPC_PULLUP_gc ;
}

ISR( Busy_Control_IntVec )
{
	sendFree = false;
	TCC2.LPER = 42;
}

void initBusyCounter()
{
	TCC2.CTRLE = TC2_BYTEM_SPLITMODE_gc;
	TCC2.CTRLA = TC2_CLKSEL_DIV256_gc;
	TCC2.CTRLB = 0;
	TCC2.INTCTRLA = TC2_LUNFINTLVL_LO_gc;
	TCC2.LCNT = 128; // 128
	TCC2.LPER = 42;
}

ISR ( TCC2_LUNF_vect )
{
	sendFree = true;
}
