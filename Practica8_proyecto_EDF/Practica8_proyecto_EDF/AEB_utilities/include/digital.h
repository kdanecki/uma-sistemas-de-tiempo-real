/*  +-----------------------------------+-----------------------------------+
    |                                                                       |
    |            C library for SRAM 23LC512-I/SN in ATmega328P              |
    |           http://babel.isa.uma.es/jafma/index.php/software            |
    |                                                                       |
    | Copyright (c) 2015 - 2016, Individual contributors, see AUTHORS file. |
    | Machine Perception and Intelligent Robotics (MAPIR),                  |
    | University of Malaga. <http://mapir.isa.uma.es>                       |
    |                                                                       |
    | This program is free software: you can redistribute it and/or modify  |
    | it under the terms of the GNU General Public License as published by  |
    | the Free Software Foundation, either version 3 of the License, or     |
    | (at your option) any later version.                                   |
    |                                                                       |
    | This program is distributed in the hope that it will be useful,       |
    | but WITHOUT ANY WARRANTY; without even the implied warranty of        |
    | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
    | GNU General Public License for more details.                          |
    |                                                                       |
    | You should have received a copy of the GNU General Public License     |
    | along with this program. If not, see <http://www.gnu.org/licenses/>.  |
    |                                                                       |
    +-----------------------------------------------------------------------+ */


/***************************************************************************//**
 *
 * @file
 * @brief       Library for managing the digital interfaces on the UMA-MS 
 * @author      Andres Gongora <andresgongora@uma.es>
 * @date        2016
 * @copyright   GPLv3. Contact authors for closed-source version.
 *
 ******************************************************************************/

#ifndef __UMA_MS_DIGITAL_INCLUDE__
#define __UMA_MS_DIGITAL_INCLUDE__

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>



/// Initialize all LEDs, DAC and buttons.
/// Configure each pin as output and initializes it to 0 (LED OFF).
void DigitalInit(void);


/// Hold DAC in latch mode (output voltage does not follow digital input)
void DACWRdisable(void);


/// Set DAC in transparent mode (output voltage follows digital input)
void DACWRenable(void);


/// Writes to digital interface (LEDs and DAC).
/// Sets on bit at a time starting with the LSB.
/// Disables DAC during write to avoid jitter.
void DigitalWrite(unsigned char byte);


/// Check whether ButtonA is pressed
bool DigitalButtonA(void);


/// Check whether ButtonB is pressed
bool DigitalButtonB(void);


/// Check whether ButtonC is pressed
bool DigitalButtonC(void);


#endif // __UMA_MS_DIGITAL_INCLUDE__
