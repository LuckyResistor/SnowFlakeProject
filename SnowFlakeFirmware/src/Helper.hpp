//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2017 by Lucky Resistor. See LICENSE for details.
// https://luckyresistor.me
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#pragma once


#include <cstdint>


/// Simple helper methods.
///
namespace Helper {
	
	
/// Initialize the helper module.
///
void initialize();	

/// Simple delay using a loop around a NOP command.
///
void delayNop(uint32_t cycles);
		
/// Delay using the SysTick for longer delays.
///
void delayMs(uint32_t delay);

/// Get the current system time.
///
uint32_t getSystemTimeMs();

/// Wait until the system time reaches the given value.
///
void waitForSystemTime(uint32_t systemTime);

/// Get a pseudo random number in the selected range.
///
/// @param minimum The minimum value for the pseudo random number, including this number.
/// @param maximum The maximum value for the pseudo random number, including this number.
/// @return A pseudo random number in the selected range.
///
uint32_t getRandom32(uint32_t minimum, uint32_t maximum);

/// Get a pseudo random number in the selected range.
///
/// @param minimum The minimum value for the pseudo random number, including this number.
/// @param maximum The maximum value for the pseudo random number, including this number.
/// @return A pseudo random number in the selected range.
///
uint16_t getRandom16(uint16_t minimum, uint16_t maximum);


}

