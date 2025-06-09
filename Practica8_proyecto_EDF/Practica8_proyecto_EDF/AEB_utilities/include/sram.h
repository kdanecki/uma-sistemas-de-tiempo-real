/*
	Library for managing the SRAM module of the UMA Arduino extension board.
	(c) Juan Antonio Fernandez Madrigal, 2015

	This module does not use any interrupts.

	The block operations can be compiled in C version or in Assembler version
	(optimized, by default). To switch to C version, pass the symbol definition
	"-D SRAMBLOCKINC" to the compiler.

	IMPORTANT NOTE: In this version of the library, you must set optimizations
					to 0 when compiling its sources.

*/

#ifndef SRAM
#define SRAM

#include <avr/io.h>

void SRAMinitiate(void);
// configure initially the SRAM for being used with SPI

void SRAMwriteblock(unsigned address, const unsigned char *block,
					unsigned length);
// Write from ADDRESS up, LENGTH bytes taken from BLOCK
// Including the call to this routine and without interrupts enabled, the
// transfer speed is around 5 microsecs per byte (200KHz)

void SRAMreadblock(unsigned address, unsigned char *block, unsigned length);
// Read from ADDRESS up, LENGTH bytes and store them into BLOCK
// Including the call to this routine and without interrupts enabled, the
// transfer speed is around 5.3 microsecs per byte (190KHz)

void SRAMsendinstr(const unsigned char *instr, unsigned char lengthinstr,
				   unsigned char *resp, unsigned char lengthresp);
// Send an instruction of LENGTHINSTR bytes to the SRAM chip, taken from INSTR,
// and after that waits for LENGTHRESP bytes to be received, storing them into
// RESP.

#endif
