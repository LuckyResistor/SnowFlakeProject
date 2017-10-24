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
#include "Display.hpp"


#include "Configuration.hpp"
#include "Hardware.hpp"
#include "Helper.hpp"

#include "Chip.hpp"

#include <cstring>


namespace Display {


/// The number of LEDs
///
const uint8_t cLedCount = 19;

/// The masks for all LEDs in the order of the display.
///
const uint32_t cLedMask0 = (1UL<<0);
const uint32_t cLedMask1 = (1UL<<3);
const uint32_t cLedMask2 = (1UL<<6);
const uint32_t cLedMask3 = (1UL<<9);
const uint32_t cLedMask4 = (1UL<<16);
const uint32_t cLedMask5 = (1UL<<19);
const uint32_t cLedMask6 = (1UL<<1);
const uint32_t cLedMask7 = (1UL<<4);
const uint32_t cLedMask8 = (1UL<<7);
const uint32_t cLedMask9 = (1UL<<10);
const uint32_t cLedMask10 = (1UL<<17);
const uint32_t cLedMask11 = (1UL<<22);
const uint32_t cLedMask12 = (1UL<<2);
const uint32_t cLedMask13 = (1UL<<5);
const uint32_t cLedMask14 = (1UL<<8);
const uint32_t cLedMask15 = (1UL<<11);
const uint32_t cLedMask16 = (1UL<<18);
const uint32_t cLedMask17 = (1UL<<23);
const uint32_t cLedMask18 = (1UL<<24);

/// The LED masks in an array.
///
__aligned(4) const uint32_t cLedMasks[cLedCount] = {
	cLedMask0,
	cLedMask1,
	cLedMask2,
	cLedMask3,
	cLedMask4,
	cLedMask5,
	cLedMask6,
	cLedMask7,
	cLedMask8,
	cLedMask9,
	cLedMask10,
	cLedMask11,
	cLedMask12,
	cLedMask13,
	cLedMask14,
	cLedMask15,
	cLedMask16,
	cLedMask17,
	cLedMask18
};

/// The port mask for all pins used for LEDs
///
const uint32_t cPortMaskLed =
	cLedMask0|cLedMask1|cLedMask2|cLedMask3|cLedMask4|cLedMask5|cLedMask6|cLedMask7|
	cLedMask8|cLedMask9|cLedMask10|cLedMask11|cLedMask12|cLedMask13|cLedMask14|cLedMask15|
	cLedMask16|cLedMask17|cLedMask18;

/// The maximum level.
///
const uint8_t cMaximumLevel = 64;

/// The number of initial LED masks.
///
const uint8_t cInitialLedMaskCount = 12;

/// The logarithmic timing values
///
__aligned(4) const uint16_t cTimingValues[cMaximumLevel] {
	250, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 120, 132, 138, 141,
	145, 149, 154, 158, 163, 168, 174, 180,
	186, 193, 200, 207, 215, 223, 232, 242,
	252, 263, 274, 286, 299, 313, 327, 343,
	359, 376, 395, 414, 435, 457, 481, 506,
	532, 561, 591, 622, 656, 692, 730, 771,
	814, 859, 908, 959, 1014, 1072, 1134, 1200, // Compensate last value.
};

/// The timing for the initial values.
///
__aligned(4) const uint32_t cInitialTimingValues[cInitialLedMaskCount] {
	0, 0, 0, 0, 2, 8, 16, 18, 20, 32, 44, 50,
};


/// The counter for the PWM
///
__aligned(4) uint8_t gPwmCounter = 0;

/// The structure for a single display buffer.
///
struct Buffer
{
	/// The calculated LED masks to write into the port register.
	///
	uint32_t mask[cMaximumLevel];
	
	/// The LED level from 0 to cMaximumLevel, where cMaximumLevel is the brightest value.
	///
	uint8_t level[cLedCount];
};

/// The two buffers for the display.
///
__aligned(4) Buffer gBuffer[2];
	
/// The masks for the initial states of the LEDs.
///
__aligned(4) uint32_t gInitialMasks[cInitialLedMaskCount];
	
/// The currently displayed buffer
///
__aligned(4) volatile uint8_t gDisplayedBufferIndex = 0;
	
/// The frame counter for the display.
///
__aligned(4) volatile uint8_t gFrameCounter = 0;


/// Calculate the LED masks for the given buffer.
///
__attribute__((optimize(3))) // always optimize this function.
void calculateLedMasksForBuffer(Buffer *buffer)
{
	for (uint8_t maskIndex = 0; maskIndex < cMaximumLevel; ++maskIndex) {
		uint32_t mask = 0;
		for (uint8_t ledIndex = 0; ledIndex < cLedCount; ++ledIndex) {
			if (buffer->level[ledIndex] > maskIndex) {
				mask |= cLedMasks[ledIndex];
			}
		}
		buffer->mask[maskIndex] = ((~mask) & cPortMaskLed);
	}
}

	
void initialize()
{
	// Clear the buffers.
	std::memset(&gBuffer, 0, sizeof(Buffer)*2);
	// Prepare the masks for the first (currently displayed) buffer.
	calculateLedMasksForBuffer(gBuffer);

	// Configure all LED pins as outputs.
	PORT->Group[0].DIR.reg |= cPortMaskLed;
	// Set all LED pins to high to turn the LEDs off.
	PORT->Group[0].OUTSET.reg = cPortMaskLed;

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
		TC_CTRLA_PRESCALER_DIV8 |
		TC_CTRLA_WAVEGEN_MFRQ |
		TC_CTRLA_MODE_COUNT16;
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Configure the CC0 value
	TC0->COUNT16.CC[0].reg = 375; // Initial counter value.
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Enable interrupt on overflow.
	TC0->COUNT16.INTENSET.bit.OVF = true;
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};

	// Enable the interrupt for TC0 with priority 1 (one lower than the rest).
	NVIC_SetPriority(TC0_IRQn, 1);
	NVIC_EnableIRQ(TC0_IRQn);

	// Enable the counter.
	TC0->COUNT16.CTRLA.bit.ENABLE = true;
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};
}


void setLedLevel(uint8_t ledIndex, uint8_t level)
{
	if (level > cMaximumLevel) {
		level = cMaximumLevel;
	}
	gBuffer[gDisplayedBufferIndex^1].level[ledIndex] = level;
}


void synchronizeAndShow()
{
	// Prepare the masks for the new buffer.
	const uint8_t targetBuffer = (gDisplayedBufferIndex^1);
	calculateLedMasksForBuffer(gBuffer + targetBuffer);
	// If we already are at the frame boundary, skip a frame.
	if ((gFrameCounter&0b111) == 0) {
		while ((gFrameCounter&0b111) == 0) {}; // wait
	}
	// Wait for the frame boundary.
	while ((gFrameCounter&0b111)!=0) {}; // wait
	gDisplayedBufferIndex = targetBuffer; // flip the buffer.
	// Copy the current levels back to the new back buffer.
	std::memcpy(gBuffer[gDisplayedBufferIndex^1].level, gBuffer[gDisplayedBufferIndex].level, sizeof(uint8_t)*cLedCount);
	// Toggle trace output B if this is the first frame.
	if (cTraceOutputSource == TraceOutputSource::FrameClaculationTime) {
		Hardware::toggleTraceOutputB();
	}
}


void activateTestPattern()
{
	auto buffer = gBuffer;
	for (uint8_t maskIndex = 0; maskIndex < cMaximumLevel; ++maskIndex) {
		buffer->mask[maskIndex] = (((maskIndex & 1) == 0) ? cPortMaskLed : 0);
	}
	gDisplayedBufferIndex = 0;
}


/// The interrupt function called from the interrupt handler.
///
__attribute__((optimize(3))) // always optimize this function.
void onInterrupt()
{
	// Handle the trace outputs.
	if (cTraceOutputSource == TraceOutputSource::DisplayInterruptTime) {
		Hardware::setTraceOutputA();
		if (gPwmCounter == 0) {
			Hardware::setTraceOutputB();
		}
	}
	// Prepare a copy of the PWM counter value.
	const auto currentPwmValue = gPwmCounter;
	// The display buffer to show.
	const Buffer &buffer = gBuffer[gDisplayedBufferIndex];
	// The current state of the IO lines without the LEDs.
	const uint32_t portState = (PORT->Group[0].OUT.reg & (~cPortMaskLed));
	// Configure the next CC0 value
	TC0->COUNT16.CC[0].reg = cTimingValues[currentPwmValue];
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};	
	// Check for the special case of the first PWM values.
	if (currentPwmValue == 0) {
		// Display all initial masks.
		PORT->Group[0].OUT.reg = portState | buffer.mask[0];
		PORT->Group[0].OUT.reg = portState | buffer.mask[1];
		__NOP();
		__NOP();
		__NOP();
		PORT->Group[0].OUT.reg = portState | buffer.mask[2];
		for (uint8_t i = 3; i < cInitialLedMaskCount; ++i) {
			Helper::delayNop(cInitialTimingValues[i]);
			PORT->Group[0].OUT.reg = portState | buffer.mask[i];
		}
		// Update the PWM counter to the first regular step.
		gPwmCounter = cInitialLedMaskCount;
	} else {
		// Send the new mask to the port
		PORT->Group[0].OUT.reg = portState | buffer.mask[currentPwmValue];
		// Increase the PWM counter for the next call.
		++gPwmCounter;
		if (gPwmCounter>=cMaximumLevel) {
			gPwmCounter = 0;
			++gFrameCounter;
		}
	}
	// Handle the trace outputs.
	if (cTraceOutputSource == TraceOutputSource::DisplayInterruptTime) {
		Hardware::clearTraceOutputA();
		Hardware::clearTraceOutputB();
	}
	// Clear the interrupt (again).
	TC0->COUNT16.INTFLAG.bit.OVF = true;
}


}


/// Implement the interrupt handler for the counter TC0
///
__attribute__((optimize(3))) // always optimize this function.
void TC0_Handler()
{
	// Call our interrupt handler.
	Display::onInterrupt();
}
