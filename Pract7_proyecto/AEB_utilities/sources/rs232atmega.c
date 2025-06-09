/* ****************************************************************************
*
*	Simple RS232 library for ATmega328P
*
*	(c) Juan-Antonio Fernandez-Madrigal, 2015-2016
*
*******************************************************************************/

#include "rs232atmega.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

/* ------------------ RS232 constants, types, variables and macros ---------- */

// RS232 receiving status
typedef enum {
				RS232_RS_WAITING_DATA=0, // still receiving data
				RS232_RS_OVERFLOW, // too long command received (excess dropped)
				RS232_RS_END, // end of command received
				RS232_RS_POSTEND, // end received and then more data received
				RS232_RS_PAUSE	// external program has paused ISR work
			 } RS232RecStatus;

// RS232 input buffer (NULL if not used)
static volatile char *rs232buffer;
// RS232 input buffer size
static volatile unsigned char rs232buffersize;
// RS232 current position in the input buffer
static volatile unsigned char rs232bufferpos;
// RS232 input buffer state
static volatile unsigned char rs232bufferstate=RS232_RS_PAUSE;
// RS232 input buffer end char
static volatile char rs232endchar;
// RS232 input data
static volatile RS232InputReport rs232inputreport;



/* ------------------ RS232 ISRs --------------- */

ISR(USART_RX_vect)
// USART rx complete; a byte is waiting to be read from the usart
// This interrupt has higher priority than ADC, blocking the ADC until finished
// It has higher priority than the other usart isrs.
{
	// read hardware
	rs232inputreport.lasterr=UCSR0A & ((1 << FE0)|(1 << DOR0)|(1 << UPE0));
	rs232inputreport.lastinputchar=UDR0;

	// enable other interrupts while we do the less strict part of the process;
	// do not enable this very interrupt for avoiding recursion
	UCSR0B &= (~(1 << RXCIE0));
	sei();

	// process data and status
	switch ((RS232RecStatus)rs232bufferstate)
	{
		case RS232_RS_WAITING_DATA: // a new char is available while waiting
		{
			rs232buffer[rs232bufferpos++]=rs232inputreport.lastinputchar;
			if (rs232inputreport.lastinputchar==rs232endchar)
				rs232bufferstate=(unsigned char)RS232_RS_END;
			else if (rs232bufferpos==rs232buffersize)
				rs232bufferstate=(unsigned char)RS232_RS_OVERFLOW;
			break;
		}
		case RS232_RS_OVERFLOW: // receiving too long command
		{
			rs232inputreport.bufov++;
			if (rs232inputreport.lastinputchar==rs232endchar)
				rs232bufferstate=(unsigned char)RS232_RS_END;
			break;
		}
		case RS232_RS_END: // received new data after the end of previous
		{
			rs232bufferstate=RS232_RS_POSTEND; // data lost
			break;
		}
		default: break; // all other cases do nothing and lose data
	}

	// restore interrupts
	cli();
	UCSR0B |= (1 << RXCIE0);
}

/* ------------------ RS232 routines --------------- */

static void RS232_PrepareToRead(void)
// Prepare the input rs232 system to read the next command, initializing the
// status and losing any data already read.
{
	rs232bufferstate=RS232_RS_PAUSE; // disable ISR work
	if (rs232buffer!=NULL)
	{
		// prepare variables
		rs232bufferpos=0;
		rs232inputreport.bufov=0;
		rs232inputreport.lasterr=0;
		rs232inputreport.lastinputchar=0;
		// start receiving
		rs232bufferstate=RS232_RS_WAITING_DATA; // must be at the end
	}
}

void RS232_Init(char *buf, unsigned char bufsize, char endch)
// Initiates the rs232 hw/sw system. Interrupts must be disabled previously
{
	// backup interrupt status and disable interrupts
	unsigned char prevints=(SREG & (1 << 7));
	if (prevints) cli();

	// 38400 bauds in order to reduce probability of error (p. 203, datasheet)
 	UBRR0H = (25 >> 8);
	UBRR0L = (25 & 0xff); // this updates the baudrate generator (p. 199)

	// no x2 speed, no multicomms (p. 196)
	UCSR0A &= (~(1 << U2X0)) & (~(1 << MPCM0));

	// enable isr vects, enable tx and rx hardware, 8 bits
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

	// 8 bits, asynchronous, no parity, 1 stop
	UCSR0C = (3 << UCSZ00);

	// flush hardware buffered data
//	unsigned char aux;
	while (UCSR0A & (1 << RXC0))
	{ // in assembler for avoiding the "unused after set" warning on AUX
		__asm__ __volatile__ (
			"push r17"		" \n\t"
			"push r30"		" \n\t"
			"push r31"		" \n\t"
			"ldi r30,0xc6"	" ; Z pointing to UDR0 register \n\t"
			"clr r31"		" \n\t"
			"ld r17,Z"		" ; read UDR0 and discard it \n\t"
			"pop r31"		" \n\t"
			"pop r30"		" \n\t"
			"pop r17"		" \n\t"
		);
//		aux=UDR0;
	}

	// prepare software reading buffer
	rs232buffer=buf;
	rs232buffersize=bufsize;
	rs232endchar=endch;
	RS232_PrepareToRead();

	// resume interrupt status
	if (prevints) sei();
}

void RS232_Finish(void)
// Disable the hw/sw system for rs232 communications.
// Interrupts should be disabled previously.
{
	// disable further isr work
	rs232bufferstate=RS232_RS_PAUSE;
	// disable isr, tx and rx hardware
	UCSR0B = 0;
}

void RS232_Read(RS232InputReport *inpcp)
// Wait until the current command is completed, either with or
// without errors, then copy the resulting report into INPCP, clearing the
// status of the input receiver in order to receive further commands.
// (NOTE: Infinite waiting is possible, if no command is received)
{
	if (rs232buffer!=NULL)
	{
		// wait pending receptions
		while ((RS232RecStatus)rs232bufferstate<RS232_RS_END);
		// disable ISR work
		rs232bufferstate=RS232_RS_PAUSE;
		if (inpcp!=NULL) memcpy(inpcp,(const void *)&rs232inputreport,sizeof(RS232InputReport));
		// enable again ISR work
		RS232_PrepareToRead();
	}
}

void RS232_Send(const char *c, unsigned char endchar)
// Send command in C and return after the whole transmission
{
	if (c==NULL) return;
	// send command
	unsigned f=0;
	while (c[f]!=endchar)
	{
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0=c[f++];
	}
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0=endchar;
}

void RS232_SendNoEnd(const char *c, unsigned char endchar)
{
	if (c==NULL) return;
	// send command
	unsigned f=0;
	while (c[f]!=endchar)
	{
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0=c[f++];
	}
	while ( !( UCSR0A & (1<<UDRE0)) );
}
