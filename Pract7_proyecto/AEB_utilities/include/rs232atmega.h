/* ****************************************************************************
*
*	Simple RS232 library for ATmega328P
*
*	(c) Juan-Antonio Fernandez-Madrigal, 2015-2016
*
*		-This library only works correctly with ATmega328P @ 16MHz, and sets
*		RS232 to asynchronous, 38400 bauds, no parity, 1 stop bit, 8 data bits.
*
*		-This code uses ISR-guided processing for receptions and no ISRs for
*		transmissions, particularly USART_RX_vect, that has higher priority than
*		ADC_vect and lower than	external interrupts or timers. Our ISR will
*		delay the latter when executing -the non-interruptible code in this
*		library has been kept to a minimum, though-.
*
*		-Transmissions from the ATmega to the outside consist of commands of
*		unlimited length ended in a given particular character.
*
*		-Receptions consist of commands of a given maximum length, also ended in
*		a given ending character.
*
*		-Both transmissions and receptions are full-duplex. Receptions can be
*		configured to receive and drop characters while using only transmissions
*		(for instance, for tracing programs).
*
*		-USAGE: First, call RS232_Init; then enable interrupts and use
*		RS232_Read and RS232_Send at will; finalize your use of the rs232 system
*		with RS232_Finish (it can be re-activated with a new RS232_Init).
*		In windows and linux, you can use putty, for example, in order to
*		connect to the arduino and test this library.
*
*******************************************************************************/

#ifndef RS232ATMEGA328P
#define RS232ATMEGA328P

#include <stdlib.h>

/* ------------------ RS232 constants, types, variables and macros ---------- */

// RS232 data for reporting results of receptions. Normal value: (0,0,endchar)
typedef struct {
				// Input buffer overflow count (number of chars dropped by OF)
				unsigned bufov;
				// Input buffer last error (copy of FE,DOR,UPE; 0 if none)
				// (p. 189 of datasheet)
				unsigned char lasterr;
				// Rs232 last received char, either within the command or in OF
				char lastinputchar;
				} RS232InputReport;


/* ------------------ RS232 routines --------------- */

void RS232_Init(char *buf, unsigned char bufsize, char endch);
// Initiates the rs232 hw/sw system to send commands and receive commands into
// BUF, which must have enough size for BUFSIZE characters. All received
// commands must end in ENDCH from now on.
// Interrupts are disabled while the configuration is done
// and restored to its previous status after that.
// Ownership of the buffer is on the caller, not on this function.
// The buffer cannot be changed until new RS232_Finish() and RS232_Init() calls
// are issued after this one.
// If BUF==NULL, receptions are configured in hardware but they drop any
// character received from the outside, with the same ISR load as they use when
// are active (in terms of delays induced in other ISRs).

void RS232_Finish(void);
// Disable the hw/sw system for rs232 communications. The current reception
// buffer is detached but not deleted or managed in any other way. Interrupt
// enabling status is not affected by this function, and the interrupts are not
// disabled while it works.

void RS232_Read(RS232InputReport *inpcp);
// Wait until the current command is completely received into the buffer
// provided in RS232_Init(), either with or without errors, then copy the
// resulting status and data into INPCP (as long as INPCP!=NULL), and clear the
// status of the input receiver buffer in order to get further commands on the
// same buffer.
// Interrupts must be enabled for this to work.
// If the input buffer was set to NULL in RS232_Init, this function does
// nothing.

void RS232_Send(const char *c, unsigned char endchar);
// Send command in C and return when everything up to (and including) the first
// ENDCHAR within C has been transmitted. If C==NULL, does nothing.
// Interrupts are not used by this function. Concurrent reads can be active at
// the same time.
// NOTE: maximum length of C must be < 65536

void RS232_SendNoEnd(const char *c, unsigned char endchar);
// The same as RS232_Send but without sending the endchar at the end.


#endif
