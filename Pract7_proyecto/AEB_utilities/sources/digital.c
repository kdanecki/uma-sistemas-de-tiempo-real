#include "digital.h"


/*===============================================================================================*\
 *	CONFIGURE GPIO PINS AND PORTS
\*===============================================================================================*/
 
#define MS_DIGITAL_OUT_BIT_0_PORT	PORTB
#define MS_DIGITAL_OUT_BIT_0_DIR	DDRB
#define MS_DIGITAL_OUT_BIT_0_PIN	2

#define MS_DIGITAL_OUT_BIT_1_PORT	PORTB
#define MS_DIGITAL_OUT_BIT_1_DIR	DDRB
#define MS_DIGITAL_OUT_BIT_1_PIN	1

#define MS_DIGITAL_OUT_BIT_2_PORT	PORTB
#define MS_DIGITAL_OUT_BIT_2_DIR	DDRB
#define MS_DIGITAL_OUT_BIT_2_PIN	0

#define MS_DIGITAL_OUT_BIT_3_PORT	PORTD
#define MS_DIGITAL_OUT_BIT_3_DIR	DDRD
#define MS_DIGITAL_OUT_BIT_3_PIN	7

#define MS_DIGITAL_OUT_BIT_4_PORT	PORTD
#define MS_DIGITAL_OUT_BIT_4_DIR	DDRD
#define MS_DIGITAL_OUT_BIT_4_PIN	5

#define MS_DIGITAL_OUT_BIT_5_PORT	PORTD
#define MS_DIGITAL_OUT_BIT_5_DIR	DDRD
#define MS_DIGITAL_OUT_BIT_5_PIN	4

#define MS_DIGITAL_OUT_BIT_6_PORT	PORTC
#define MS_DIGITAL_OUT_BIT_6_DIR	DDRC
#define MS_DIGITAL_OUT_BIT_6_PIN	4

#define MS_DIGITAL_OUT_BIT_7_PORT	PORTC
#define MS_DIGITAL_OUT_BIT_7_DIR	DDRC
#define MS_DIGITAL_OUT_BIT_7_PIN	3

#define MS_DIGITAL_OUT_BIT_WR_PORT	PORTC
#define MS_DIGITAL_OUT_BIT_WR_DIR	DDRC
#define MS_DIGITAL_OUT_BIT_WR_PIN	0

#define MS_BUTTON_A_PORT		PINB	
#define MS_BUTTON_A_DIR			DDRB
#define MS_BUTTON_A_PIN			4

#define MS_BUTTON_B_PORT		PINB
#define MS_BUTTON_B_DIR			DDRB
#define MS_BUTTON_B_PIN			3

#define MS_BUTTON_C_PORT		PIND
#define MS_BUTTON_C_DIR			DDRD
#define MS_BUTTON_C_PIN			2


/*===============================================================================================*\
 *	DIGITAL I/O
\*===============================================================================================*/

void DigitalInit(void)
{
	// CONFIGURE LEDS AS OUTPUT
	MS_DIGITAL_OUT_BIT_0_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_0_PIN);
	MS_DIGITAL_OUT_BIT_1_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_1_PIN);
	MS_DIGITAL_OUT_BIT_2_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_2_PIN);
	MS_DIGITAL_OUT_BIT_3_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_3_PIN);
	MS_DIGITAL_OUT_BIT_4_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_4_PIN);
	MS_DIGITAL_OUT_BIT_5_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_5_PIN);
	MS_DIGITAL_OUT_BIT_6_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_6_PIN);
	MS_DIGITAL_OUT_BIT_7_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_7_PIN);
	
	// CONFIGURE DAC WR
	MS_DIGITAL_OUT_BIT_WR_DIR |= (0x01 << MS_DIGITAL_OUT_BIT_WR_PIN);
	
	// CONFIGURE BUTTONS
	//MS_BUTTON_A_DIR &= ~(0x01 << MS_BUTTON_A_PIN);
	//MS_BUTTON_B_DIR &= ~(0x01 << MS_BUTTON_B_PIN);
	//MS_BUTTON_C_DIR &= ~(0x01 << MS_BUTTON_C_PIN);
}


void DACWRdisable(void)
{
	MS_DIGITAL_OUT_BIT_WR_PORT |= (0x01 << MS_DIGITAL_OUT_BIT_WR_PIN);
}


void DACWRenable(void)
{
	MS_DIGITAL_OUT_BIT_WR_PORT &=  ~(0x01 << MS_DIGITAL_OUT_BIT_WR_PIN);
}


void DigitalWrite(unsigned char byte)
{
	unsigned char bit;
	unsigned char mask;
	
	DACWRdisable();
	
	bit = (byte >> 0) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_0_PIN);
	MS_DIGITAL_OUT_BIT_0_PORT = (MS_DIGITAL_OUT_BIT_0_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_0_PIN);
	
	bit = (byte >> 1) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_1_PIN);
	MS_DIGITAL_OUT_BIT_1_PORT = (MS_DIGITAL_OUT_BIT_1_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_1_PIN);

	bit = (byte >> 2) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_2_PIN);
	MS_DIGITAL_OUT_BIT_2_PORT = (MS_DIGITAL_OUT_BIT_2_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_2_PIN);

	bit = (byte >> 3) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_3_PIN);
	MS_DIGITAL_OUT_BIT_3_PORT = (MS_DIGITAL_OUT_BIT_3_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_3_PIN);

	bit = (byte >> 4) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_4_PIN);
	MS_DIGITAL_OUT_BIT_4_PORT = (MS_DIGITAL_OUT_BIT_4_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_4_PIN);

	bit = (byte >> 5) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_5_PIN);
	MS_DIGITAL_OUT_BIT_5_PORT = (MS_DIGITAL_OUT_BIT_5_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_5_PIN);

	bit = (byte >> 6) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_6_PIN);
	MS_DIGITAL_OUT_BIT_6_PORT = (MS_DIGITAL_OUT_BIT_6_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_6_PIN);
	
	bit = (byte >> 7) & 0x01;
	mask = ~(0x01 << MS_DIGITAL_OUT_BIT_7_PIN);
	MS_DIGITAL_OUT_BIT_7_PORT = (MS_DIGITAL_OUT_BIT_7_PORT & mask) | (bit << MS_DIGITAL_OUT_BIT_7_PIN);
	
	DACWRenable();
}


bool DigitalButtonA(void)
{
	return MS_BUTTON_A_PORT & (0x01 << MS_BUTTON_A_PIN);
}


bool DigitalButtonB(void)
{
	return MS_BUTTON_B_PORT & (0x01 << MS_BUTTON_B_PIN);
}


bool DigitalButtonC(void)
{
	return MS_BUTTON_C_PORT & (0x01 << MS_BUTTON_C_PIN);
}



