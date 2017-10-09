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
#include "Helper.hpp"


#include "sam.h"


namespace Helper {


/// The tick counter for the delay function.
/// Align the counter to make sure all operations on this 32bit value are atomic.
///
volatile uint32_t gTickCounter __attribute__((aligned (4))) = 0;


void initialize()
{
	// Configure SysTick to generate an interrupt every ms.
	SysTick_Config(48000000/1000);
	// Enable the interrupt with a low priority.
	NVIC_SetPriority(SysTick_IRQn, 2);
	NVIC_EnableIRQ(SysTick_IRQn);
}


void delayNop(uint32_t cycles)
{
	for (uint32_t i = 0; i < cycles; ++i) {
		__NOP();
	}
}
	

void delayMs(uint32_t delay)
{
	if (delay > 0) {
		const uint32_t endValue = gTickCounter + delay;
		while (gTickCounter != endValue) { __NOP(); }
	}
}


}


void SysTick_Handler()
{
	++Helper::gTickCounter;
}
