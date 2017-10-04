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
#include "TimerInterrupt.h"


#include "sam.h"


namespace TimerInterrupt {


/// The maximum number of callbacks.
///
const uint8_t cCallBackMaxCount = 2;

/// An array with callbacks.
///
CallBackFn gCallBacks[cCallBackMaxCount];

/// The number of defined call backs
///
uint8_t gCallBackCount = 0;

	
void initialize()
{
	// Enable power for counter TC0
	PM->APBCMASK.bit.TC0_ = true;
	// Send the main clock to the counter.
	GCLK->CLKCTRL.reg = 
		GCLK_CLKCTRL_ID_TC0_TC1 |
		GCLK_CLKCTRL_GEN_GCLK0 |
		GCLK_CLKCTRL_CLKEN;
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization
	// Configure the counter
	TC0->COUNT16.CTRLA.reg = 
		TC_CTRLA_PRESCALER_DIV64 |
		TC_CTRLA_WAVEGEN_MFRQ |
		TC_CTRLA_MODE_COUNT16;
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Configure the CC0 value
	TC0->COUNT16.CC[0].reg = 0x2000; // ???
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Enable interrupt on overflow.	
	TC0->COUNT16.INTENSET.bit.OVF = true;
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};

	// Enable the interrupt for TC0
	NVIC_EnableIRQ(TC0_IRQn);

	// Enable the counter.
	TC0->COUNT16.CTRLA.bit.ENABLE = true;
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};
}


void registerCallBack(CallBackFn callBackFn)
{
	gCallBacks[gCallBackCount] = callBackFn;
	++gCallBackCount;
}


}


/// Implement the interrupt handler for the counter TC0
///
void TC0_Handler()
{
	PORT->Group[0].OUTTGL.reg = 1; // Toggle PA00	
}



