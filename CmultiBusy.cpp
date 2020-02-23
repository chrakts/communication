/*
 * CmultiBusy.cpp
 *
 * Created: 24.05.2018 13:30:48
 *  Author: chrak_2
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <uartHardware.h>
//#include "../loraSupervisor/myConstants.h"

extern volatile uint8_t sendFree;
extern volatile uint8_t sendAnswerFree;

/* Hier wird der Pin konfiguriert, der die Interruptueberwachung bekommt, ob der Bus belegt ist */

void initReadMonitor()
{
  Busy_Control_Port.INTCTRL  = PORT_INT0LVL0_bm | PORT_INT0LVL1_bm; // Low-Level interrupt 0 for PORTD
  Busy_Control_Port.INT0MASK = Busy_Control_Pin;
  Busy_Control_Port.Busy_Control_PinCtrl = PORT_ISC_BOTHEDGES_gc | PORT_OPC_PULLUP_gc ;
}

void deInitReadMonitor()
{
  Busy_Control_Port.INTCTRL  = 0; // Low-Level interrupt 0 for PORTD
  Busy_Control_Port.INT0MASK = Busy_Control_Pin;
  Busy_Control_Port.Busy_Control_PinCtrl = PORT_ISC_INPUT_DISABLE_gc | PORT_OPC_PULLUP_gc ;
}

ISR( Busy_Control_IntVec )
{
  sendFree = false;
  sendAnswerFree = false;
  TCC2.LCNT = 20;
  //LED_ROT_ON;
}

void initBusyCounter()
{
  TCC2.CTRLE = TC2_BYTEM_SPLITMODE_gc;
  TCC2.CTRLA = TC2_CLKSEL_DIV256_gc;
  TCC2.CTRLB = 0;
  TCC2.INTCTRLA = TC2_LUNFINTLVL_HI_gc;
  TCC2.LCNT = 128; // 128
  TCC2.LPER = 42;
}

ISR ( TCC2_LUNF_vect )
{
  if(sendAnswerFree == true )
  {
      sendFree = true;
      //LED_ROT_OFF;
  }
  else
      sendAnswerFree = true;
  TCC2.LCNT = 20;
}
