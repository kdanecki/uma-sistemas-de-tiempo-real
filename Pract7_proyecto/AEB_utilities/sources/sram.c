/*
	Library for managing the SRAM module of the UMA Arduino extension board.
	(c) Juan Antonio Fernandez Madrigal, 2015
*/

#include <stdlib.h>
#include <avr/io.h>
#include "sram.h"

// enable the SS pin. bit PB2 in the UMA shield v0.1.0; PD3 in v1.0.0
#define SRAMENABLE PORTD &= (~(1 << 3))

// disable the SS pin. The same.
#define SRAMDISABLE PORTD |= (1 << 3)

// send a byte
#define SRAMSEND(b) SPDR = b

// wait end of transmission of last written byte
// NOTE: A SRAMREAD or SRAMDISCARDREAD op must be performed after this macro
// for clearing SPIF
#define SRAMWAITENDSEND while ((SPSR & (1<<SPIF))==0)

// send a byte and wait its transmission
#define SRAMSENDANDWAIT(b) SRAMSEND(b); SRAMWAITENDSEND

// read last byte received
#define SRAMREAD(dest) dest = SPDR

// discard last byte received
#define SRAMDISCARDREAD 	__asm__ __volatile__ ( \
			"push r17"		" \n\t" \
			"in r17,0x2e"	" ; read SPDR and discard it - clears SPIF \n\t" \
			"pop r17"		" \n\t" \
		)



// instruction codes
#define SRAMINSTR_WR 2
#define SRAMINSTR_RD 3


void SRAMinitiate(void)
// configure initially the SRAM for being used with SPI
{
	// prepare SPI for connecting to the SRAM

	// MOSI(B3) as output; MISO (B4) as input;
	// SCK (B5) as output; SS output
	DDRB &= ~(1 << 4);
	// In version 0.1.0: DDRB |= (1 << 3) | (1 << 5); 
	// In version 1.0.0: DDRB |= (1 << 3) | (1 << 5); DDRD |= (1 << 3);
	DDRB |= (1 << 3) | (1 << 5); 
	DDRD |= (1 << 3);

	// We must set the SS pin of the ATmega as output in order to set SPI master
	// mode, even when we do not use the SS pin for chip select of the external
	// device (actually, that pin becomes a general output pin for any purpose).
	// This is explained in section 18.3.2 of the atmega328P datasheet.
	DDRB |= (1 << 2);

	SRAMDISABLE;
		// deselect SRAM; SRAM needs about 30 nanosecs to
	    // stabilize after a SS change (one cycle of the arduino is 62.5 nanos)
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (0 << SPR0);
		// Enable spi module; no interrupts; master operation; CPOL=1, CPHA=1
		// 16MHz/4 = 4MHz, since the SRAM
		// supports up to 20MHz; DORD=0 for sending/receiving first the most
		// significant bits, then the least.
	SPSR |= (1 << SPI2X);
		// Here we double communication speed because we are master, up to 8MHz
}

void SRAMsendinstr(const unsigned char *instr, unsigned char lengthinstr,
				   unsigned char *resp, unsigned char lengthresp)
// Send an instruction of LENGTHINSTR bytes to the SRAM, taken from INSTR,
// and after that waits for LENGTHRESP bytes to be received, storing them into
// RESP.
{
	unsigned char f;

	if ((instr==NULL)||(lengthinstr==0)) return;

	SRAMENABLE;

	for (f=0; f<lengthinstr; f++)
	{ // braces needed because the next macro consists of 2 sentences
		SRAMSENDANDWAIT(instr[f]); // writing to SPDR initiates transmission
			 					   //ignore data back (SRAM provides none)
		SRAMDISCARDREAD; // clears the SPIF
	}
	if ((resp!=NULL)&&(lengthresp!=0))
	{
		for (f=0; f<lengthresp; f++)
		{
			SRAMSENDANDWAIT(0xFF); // init transmission, i.e., to activate clk
					   // It is common practice to put the MOSI line high when
					   // no real data is being transmitted.
			SRAMREAD(resp[f]);
		}
	}
	SRAMDISABLE;
}

void SRAMwriteblock(unsigned address, const unsigned char *block,
					unsigned length)
// Write from ADDRESS up, LENGTH bytes taken from BLOCK
{
	#ifdef SRAMBLOCKINC

		SRAMENABLE;
		SRAMSENDANDWAIT(SRAMINSTR_WR);
		SRAMDISCARDREAD; // clears the SPIF
		SRAMSENDANDWAIT((address & 0xff00) >> 8);
		SRAMDISCARDREAD; // clears the SPIF
		SRAMSENDANDWAIT(address & 0xff);
		SRAMDISCARDREAD; // clears the SPIF
		while (length>0)
		{
			SRAMSENDANDWAIT(*block);
			SRAMDISCARDREAD; // clears the SPIF
			block++;
			length--;
		}
		SRAMDISABLE;

	#else

	// __asm__ and __volatile__ are standard macros
	// The syntax of the __asm__ block content, and the last output/input
	// operands is described entirely in
	// http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
	// and also in
	// https://gcc.gnu.org/onlinedocs/gcc-4.5.1/gcc/Machine-Constraints.html
	//
	// We use low registers (<r17) for holding C arguments, and high registers
	// (>r16) for this code; the latter are pushed-popped because clobbering
	// may cause the compiler to be unable to consider so many registers as we
	// use here for being clobbered (changed by this code).
	//
	// The __tmp_reg__ register can be mangled without saving it (it is 
	// typically r0)
	//
	// We assume a communication speed of 8MHz. That means that
	// transmitting a byte to the external sram takes 1microsec = 16 nops
	// The \n\t sequences are completely necessary; otherwise, the next
	// instruction will not be assembled.
	__asm__ __volatile__ (
		"push r18"		"; for accumulator \n\t"
		"push r30"		"; Z to point at SPSR at all times \n\t"
		"push r31"		" \n\t"
		"push r28"		"; Y to point to the next byte to send \n\t"
		"push r29"		" \n\t"
		"push r26"		"; X to hold the length (need X for SBI instruction) \n\t"
		"push r27"		" \n\t"

		"mov r26,%[_lengthlow]"		"; X <- length-low \n\t"
		"mov r27,%[_lengthhigh]"	"; X <- length-high \n\t"
		"mov __tmp_reg__,r27"	"\n\t"
		"or __tmp_reg__,r26"	"; __tmp_reg__ becomes 0 only if length is 0 \n\t"
		"brbs 1,SRAMwriteblock_endsending" " \n\t"

		"ldi r30,0x4d"	"; Z-low <- pointing to SPSR register \n\t"
		"clr r31"		"; Z-high <- pointing to SPSR register \n\t"

		"cbi 0x0B,3"	"; SRAMENABLE. cbi 0x05,2 in v0.1.0; cbi 0x0B,3 in v1.0.0 \n\t"

		"ldi r18,%[_wrinstr]"		" ; need to use r18 for ldi\n\t"
		"out 0x2e,r18"	"; SRAMSEND(SRAMINSTR_WR) \n\t"
		"mov __tmp_reg__,%[_addresshigh]"	"; get high byte of sram address\n\t"
	"SRAMwriteblock_wait1:"			" ; while ((SPSR & (1<<SPIF))==0)\n\t"
		"ld r18,Z" "; r18 <- (SPSR) \n\t"
		"bst r18,7" 	" \n\t"
		"brtc SRAMwriteblock_wait1"	" \n\t"
		"in r18,0x2e"	"; this clears the SPIF\n\t"

		"out 0x2e,__tmp_reg__"	" ; SRAMSEND(address_high) \n\t"
		"mov __tmp_reg__,%[_addresslow]"	"; get low byte of sram address\n\t"
	"SRAMwriteblock_wait2:"			"\n\t"
		"ld r18,Z" "; r18 <- (SPSR) \n\t"
		"bst r18,7" 	" \n\t"
		"brtc SRAMwriteblock_wait2"	" \n\t"
		"in r18,0x2e"	"; this clears the SPIF\n\t"

		"out 0x2e,__tmp_reg__"	" ; SRAMSEND(address_low) \n\t"
	"SRAMwriteblock_wait3:"			" \n\t"
		"ld r18,Z" "; r18 <- (SPSR) \n\t"
		"bst r18,7" 	" \n\t"
		"brtc SRAMwriteblock_wait3"	" \n\t"
		"in r18,0x2e"	"; this clears the SPIF\n\t"

		"mov r28,%[_blocklow]"	"; Y-low <- block-low \n\t"
		"mov r29,%[_blockhigh]"	"; Y-high <- block-high \n\t"
		"ld __tmp_reg__,Y+"		" ; __tmp_reg__ <- byte to send; Y(==block)++ \n\t"
	"SRAMwriteblock_sending: "			" ; while (length>0) \n\t"
		"out 0x2e,__tmp_reg__"	" ; SRAMSEND(byte) \n\t"
		"ld __tmp_reg__,Y+"		" ; get next byte to send already \n\t"
	"SRAMwriteblock_wait4:"			" ; while ((SPSR & (1<<SPIF))==0) \n\t"
		"ld r18,Z" "; r18 <- (SPSR) \n\t"
		"bst r18,7" 	" \n\t"
		"brtc SRAMwriteblock_wait4"	" \n\t"
		"in r18,0x2e"	"; this clears the SPIF\n\t"
		"sbiw r26,1"	" ; X(==length)-- \n\t"
		"brbc 1,SRAMwriteblock_sending" " \n\t"

		"sbi 0x0B,3"	" ; SRAMDISABLE. sbi 0x05,2 in v0.1.0; sbi 0x0B,3 in v1.0.0 \n\t"

	"SRAMwriteblock_endsending: "		" \n\t"
		"pop r27"		" \n\t"
		"pop r26"		" \n\t"
		"pop r29"		" \n\t"
		"pop r28"		" \n\t"
		"pop r31"		" \n\t"
		"pop r30"		" \n\t"
		"pop r18"		" \n\t"
		::[_wrinstr] "M" (SRAMINSTR_WR),
		  [_lengthlow] "l" ((unsigned char)(length & 0xff)),
		  [_lengthhigh] "l" ((unsigned char)(length >> 8)),
		  [_addresslow] "l" ((unsigned char)(address & 0xff)),
		  [_addresshigh] "l" ((unsigned char)(address >> 8)),
		  [_blocklow] "l" ((unsigned char)((unsigned)block & 0xff)),
		  [_blockhigh] "l" ((unsigned char)((unsigned)block >> 8))
	);

	#endif
}

void SRAMreadblock(unsigned address, unsigned char *block, unsigned length)
{

	#ifdef SRAMBLOCKINC

		SRAMENABLE;
		SRAMSENDANDWAIT(SRAMINSTR_RD);
		SRAMDISCARDREAD; // clears the SPIF
		SRAMSENDANDWAIT((address & 0xff00) >> 8);
		SRAMDISCARDREAD; // clears the SPIF
		SRAMSENDANDWAIT(address & 0xff);
		SRAMDISCARDREAD; // clears the SPIF
		while (length>0)
		{
			SRAMSENDANDWAIT(0xff); // we need to send something to generate the clk
			SRAMREAD(*block);
			block++;
			length--;
		}
		SRAMDISABLE;

	#else

		__asm__ __volatile__ (
		"push r18"		" \n\t"
		"push r30"		" \n\t"
		"push r31"		" \n\t"
		"push r28"		" \n\t"
		"push r29"		" \n\t"
		"push r26"		" \n\t"
		"push r27"		" \n\t"

		"mov r26,%[_lengthlow]"	" ; X <- length \n\t"
		"mov r27,%[_lengthhigh]"	" \n\t"
		"mov __tmp_reg__,r26"	" ; __tmp_reg__ <- length_low \n\t"
		"or __tmp_reg__,r27"	" ; __tmp_reg__ becomes 0 only if length is 0 \n\t"
		"brbs 1,SRAMreadblock_endreading" " \n\t"

		"ldi r30,0x4d"	" ; Z pointing to SPSR register \n\t"
		"clr r31"		" \n\t"

		"cbi 0x0B,3"	" ; SRAMENABLE. cbi 0x05,2 in v0.1.0; cbi 0x0B,3 in v1.0.0 \n\t"

		"ldi r18,%[_rdinstr]"		" ; SRAMSEND(SRAMINSTR_RD) \n\t"
		"out 0x2e,r18"	" \n\t"
		"mov __tmp_reg__,%[_addresshigh]"	"; prepare to write high address\n\t"
	"SRAMreadblock_wait1:"			"\n\t"
		"ld r18,Z" " \n\t"
		"bst r18,7" 	" \n\t"
		"brtc SRAMreadblock_wait1"	" \n\t"
		"in r18,0x2e"	"\n\t"

		"out 0x2e,__tmp_reg__"	" ; SRAMSEND(address_high) \n\t"
		"mov __tmp_reg__,%[_addresslow]"	"; prepare to write low address\n\t"
	"SRAMreadblock_wait2:"			"\n\t"
		"ld r18,Z" " \n\t"
		"bst r18,7" 	" \n\t"
		"brtc SRAMreadblock_wait2"	" \n\t"
		"in r18,0x2e"	"\n\t"

		"out 0x2e,__tmp_reg__"	" ; SRAMSEND(address_low) \n\t"
		"ser r18"		"; prepare next byte to write (=xff to activate clk) - need to be > r16\n\t"
		"mov __tmp_reg__,r18"	"; store byte to write into __tmp_reg__\n\t"
	"SRAMreadblock_wait3:"			"\n\t"
		"ld r18,Z" " \n\t"
		"bst r18,7" 	" \n\t"
		"brtc SRAMreadblock_wait3"	" \n\t"
		"in r18,0x2e"	"\n\t"

		"out 0x2e,__tmp_reg__"	"; send first 0xff\n\t"
		"mov r28,%[_blocklow]"	"; Y-low <- block-low\n\t"
		"mov r29,%[_blockhigh]"	"; Y-high <- block-high\n\t"
		"ser r18"		"\n\t"
	"SRAMreadblock_wait4:"			" \n\t"
		"ld __tmp_reg__,Z" 		" \n\t"
		"bst __tmp_reg__,7" 	" \n\t"
		"brtc SRAMreadblock_wait4"	" \n\t"
		"in __tmp_reg__,0x2e"	" ; this also clears the SPIF \n\t"
		"out 0x2e,r18"	" ; SRAMSEND(0xff) <- to activate clock again\n\t"
		"st Y+,__tmp_reg__"		" ; store value into block; Y++ \n\t"
		"sbiw r26,1"	" ; X(==length)-- \n\t"
		"brbc 1,SRAMreadblock_wait4" " \n\t"

	"SRAMreadblock_wait5:"			"; we need to wait answer of last 0xff \n\t"
		"ld __tmp_reg__,Z" 		" \n\t"
		"bst __tmp_reg__,7" 	" \n\t"
		"brtc SRAMreadblock_wait5"	" \n\t"

		"sbi 0x0B,3"	" ; SRAMDISABLE. sbi 0x05,2 in v0.1.0; sbi 0x0B,3 in v1.0.0 \n\t"

	"SRAMreadblock_endreading: "		" \n\t"
		"pop r27"		" \n\t"
		"pop r26"		" \n\t"
		"pop r29"		" \n\t"
		"pop r28"		" \n\t"
		"pop r31"		" \n\t"
		"pop r30"		" \n\t"
		"pop r18"		" \n\t"
	::[_rdinstr] "M" (SRAMINSTR_RD),
		  [_lengthlow] "l" ((unsigned char)(length & 0xff)),
		  [_lengthhigh] "l" ((unsigned char)(length >> 8)),
		  [_addresslow] "l" ((unsigned char)(address & 0xff)),
		  [_addresshigh] "l" ((unsigned char)(address >> 8)),
		  [_blocklow] "l" ((unsigned char)((unsigned)block & 0xff)),
		  [_blockhigh] "l" ((unsigned char)((unsigned)block >> 8))
	);

	#endif
}

