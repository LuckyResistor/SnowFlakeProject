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
#include "Display.h"


#include "Hardware.h"

#include "sam.h"

#include <cstring>


namespace Display {

/// The display buffer address
///
enum BufferId : uint8_t {
	BufferA = 0,
	BufferB = 1,	
};

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

/// The port mask for all pins used for LEDs
///
const uint32_t cPortMaskLed =
	cLedMask0|cLedMask1|cLedMask2|cLedMask3|cLedMask4|cLedMask5|cLedMask6|cLedMask7|
	cLedMask8|cLedMask9|cLedMask10|cLedMask11|cLedMask12|cLedMask13|cLedMask14|cLedMask15|
	cLedMask16|cLedMask17|cLedMask18;

/// The maximum level.
///
const uint8_t cMaximumLevel = 64;

/// The logarithmic timing values
///
const uint16_t cTimingValues[cMaximumLevel] {
	105, 106, 108, 109, 111, 113, 115, 118,
	120, 123, 125, 128, 131, 134, 138, 141,
	145, 149, 154, 158, 163, 168, 174, 180,
	186, 193, 200, 207, 215, 223, 232, 242,
	252, 263, 274, 286, 299, 313, 327, 343,
	359, 376, 395, 414, 435, 457, 481, 506,
	532, 561, 591, 622, 656, 692, 730, 771,
	814, 859, 908, 959, 1014, 1072, 1134, 1200,
};


/// The counter for the PWM
///
uint8_t gPwmCounter = 0;
	
/// Display buffer A
///
uint8_t gDisplayBufferA[cLedCount];

/// Display buffer B
///
uint8_t gDisplayBufferB[cLedCount];
	
/// The currently displayed buffer
///
volatile BufferId gDisplayedBuffer = BufferA;
	
	
void initialize()
{
	// Clear the buffers.
	std::memset(&gDisplayBufferA, cMaximumLevel, sizeof(uint8_t)*cLedCount);
	std::memset(&gDisplayBufferB, cMaximumLevel, sizeof(uint8_t)*cLedCount);

	// Configure all LED pins as outputs.
	PORT->Group[0].DIR.reg |= cPortMaskLed;
	// Set all LED pins to high to turn the LEDs off.
	PORT->Group[0].OUT.reg |= cPortMaskLed;

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
	TC0->COUNT16.CC[0].reg = 375; // 48MHz / 2 / 64kHz = 375
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


void setLedPinLevels(uint32_t outputMask)
{
}


void setLedLevel(uint8_t ledIndex, uint8_t level)
{
	if (level > cMaximumLevel) {
		level = cMaximumLevel;
	}
	if (gDisplayedBuffer == BufferA) {
		gDisplayBufferB[ledIndex] = (cMaximumLevel-level);
	} else {
		gDisplayBufferA[ledIndex] = (cMaximumLevel-level);
	}
}


void show()
{
	if (gDisplayedBuffer == BufferA) {
		gDisplayedBuffer = BufferB;
		std::memcpy(gDisplayBufferA, gDisplayBufferB, sizeof(uint8_t)*cLedCount);
	} else {
		gDisplayedBuffer = BufferA;
		std::memcpy(gDisplayBufferB, gDisplayBufferA, sizeof(uint8_t)*cLedCount);
	}
}


constexpr uint8_t getMaximumLevel()
{
	return cMaximumLevel;
}


void onInterrupt()
{
	// Clear the interrupt.
	TC0->COUNT16.INTFLAG.bit.OVF = true;
	// Prepare a copy of the PWM counter value.
	const auto currentPwmValue = gPwmCounter;
	// Configure the new CC0 value
	TC0->COUNT16.CC[0].reg = cTimingValues[cMaximumLevel-1-currentPwmValue];
	while (TC0->COUNT16.STATUS.bit.SYNCBUSY) {};
	// The resulting LED mask.
	uint32_t ledMask = 0;
	// Check which buffer to display.
	if (gDisplayedBuffer == BufferA) {
		if (gDisplayBufferA[0] > currentPwmValue) ledMask |= cLedMask0;	
		if (gDisplayBufferA[1] > currentPwmValue) ledMask |= cLedMask1;
		if (gDisplayBufferA[2] > currentPwmValue) ledMask |= cLedMask2;
		if (gDisplayBufferA[3] > currentPwmValue) ledMask |= cLedMask3;
		if (gDisplayBufferA[4] > currentPwmValue) ledMask |= cLedMask4;
		if (gDisplayBufferA[5] > currentPwmValue) ledMask |= cLedMask5;
		if (gDisplayBufferA[6] > currentPwmValue) ledMask |= cLedMask6;
		if (gDisplayBufferA[7] > currentPwmValue) ledMask |= cLedMask7;
		if (gDisplayBufferA[8] > currentPwmValue) ledMask |= cLedMask8;
		if (gDisplayBufferA[9] > currentPwmValue) ledMask |= cLedMask9;
		if (gDisplayBufferA[10] > currentPwmValue) ledMask |= cLedMask10;
		if (gDisplayBufferA[11] > currentPwmValue) ledMask |= cLedMask11;
		if (gDisplayBufferA[12] > currentPwmValue) ledMask |= cLedMask12;
		if (gDisplayBufferA[13] > currentPwmValue) ledMask |= cLedMask13;
		if (gDisplayBufferA[14] > currentPwmValue) ledMask |= cLedMask14;
		if (gDisplayBufferA[15] > currentPwmValue) ledMask |= cLedMask15;
		if (gDisplayBufferA[16] > currentPwmValue) ledMask |= cLedMask16;
		if (gDisplayBufferA[17] > currentPwmValue) ledMask |= cLedMask17;
		if (gDisplayBufferA[18] > currentPwmValue) ledMask |= cLedMask18;
	} else {
		if (gDisplayBufferB[0] > currentPwmValue) ledMask |= cLedMask0;
		if (gDisplayBufferB[1] > currentPwmValue) ledMask |= cLedMask1;
		if (gDisplayBufferB[2] > currentPwmValue) ledMask |= cLedMask2;
		if (gDisplayBufferB[3] > currentPwmValue) ledMask |= cLedMask3;
		if (gDisplayBufferB[4] > currentPwmValue) ledMask |= cLedMask4;
		if (gDisplayBufferB[5] > currentPwmValue) ledMask |= cLedMask5;
		if (gDisplayBufferB[6] > currentPwmValue) ledMask |= cLedMask6;
		if (gDisplayBufferB[7] > currentPwmValue) ledMask |= cLedMask7;
		if (gDisplayBufferB[8] > currentPwmValue) ledMask |= cLedMask8;
		if (gDisplayBufferB[9] > currentPwmValue) ledMask |= cLedMask9;
		if (gDisplayBufferB[10] > currentPwmValue) ledMask |= cLedMask10;
		if (gDisplayBufferB[11] > currentPwmValue) ledMask |= cLedMask11;
		if (gDisplayBufferB[12] > currentPwmValue) ledMask |= cLedMask12;
		if (gDisplayBufferB[13] > currentPwmValue) ledMask |= cLedMask13;
		if (gDisplayBufferB[14] > currentPwmValue) ledMask |= cLedMask14;
		if (gDisplayBufferB[15] > currentPwmValue) ledMask |= cLedMask15;
		if (gDisplayBufferB[16] > currentPwmValue) ledMask |= cLedMask16;
		if (gDisplayBufferB[17] > currentPwmValue) ledMask |= cLedMask17;
		if (gDisplayBufferB[18] > currentPwmValue) ledMask |= cLedMask18;
	}
	// Send the new mask to the port
	PORT->Group[0].OUT.reg = (PORT->Group[0].OUT.reg & (~cPortMaskLed)) | (ledMask & cPortMaskLed);
	// Increase the PWM counter for the next call.
	++gPwmCounter;
	if (gPwmCounter>=cMaximumLevel) {
		gPwmCounter = 0;
	}
}


}


/// Implement the interrupt handler for the counter TC0
///
void TC0_Handler()
{
	// Call our interrupt handler.
	Display::onInterrupt();
}
