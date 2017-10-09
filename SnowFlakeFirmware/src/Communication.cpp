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
#include "Communication.hpp"


#include "Display.hpp" // DEBUG

#include "sam.h"


namespace Communication {


/// The port number for the data input.
///
const uint8_t cPortIndexDataIn = 25;

/// The mask for the data input.
///
const uint32_t cPortMaskDataIn = (1UL<<cPortIndexDataIn);

/// The write config mask for the data input.
///
const uint32_t cWriteConfigMaskDataIn = (cPortIndexDataIn>=8?(PORT_WRCONFIG_HWSEL|(cPortMaskDataIn>>16)):(cPortMaskDataIn));

/// The external interrupt index for the data input.
///
const uint8_t cExtInterruptIndexDataIn = 13;

/// The port number for the data output.
///
const uint8_t cPortIndexDataOut = 15;

/// The mask for the data output.
///
const uint32_t cPortMaskDataOut = (1UL<<cPortIndexDataOut);

/// The write config mask for the data output.
///
const uint32_t cWriteConfigMaskDataOut = (cPortIndexDataIn>=8?(PORT_WRCONFIG_HWSEL|(cPortMaskDataIn>>16)):(cPortMaskDataIn));


/// The timing for a zero bit.
///
const uint16_t cPulseTimeZeroBit = 0x0200;

/// The timing for a one bit.
///
const uint16_t cPulseTimeOneBit = 0x0400;

/// The timing for a break/separator signal.
///
const uint16_t cPulseTimeBreak = 0x0600;

/// The timing for a synchronization signal.
///
const uint16_t cPulseTimeSynchronization = 0x0800;

/// The detection tolerance of the timings
///
const uint16_t cPulseTimingTolerance = 0x0040;


/// The sent/received data.
///
volatile uint32_t gData = 0;

/// The identifier for this board.
///
uint8_t gIdentifier = 0;

/// The length of the strand
///
uint8_t gStrandLength = 5; 

/// The registered synchronization function.
///
SynchronizationFn gSynchronizationFn = nullptr;

/// The registered read data function.
///
ReadDataFn gReadDataFn = nullptr;


/// The timestamp on raising edge.
///
uint16_t gInputRisingTimeStamp = 0;

/// The current data which was read.
///
uint32_t gCurrentReadData = 0;

/// The current number of bits read.
///
uint8_t gCurrentReadBitCount = 0;


void setHardwareCommunicationForward()
{
	__disable_irq();
	// Data Output:
	// Configure the data out port as output with no input sampling.
	PORT->Group[0].DIR.reg |= cPortMaskDataOut;
	PORT->Group[0].OUTCLR.reg = cPortMaskDataOut;
	PORT->Group[0].PINCFG[cPortIndexDataOut].reg = 0;
	// Data Input:
	// Configure the data in port as input with pull-down.
	PORT->Group[0].DIR.reg &= (~cPortMaskDataIn);
	PORT->Group[0].OUTCLR.reg = cPortMaskDataIn;
	PORT->Group[0].PMUX[cPortIndexDataIn/2].reg &= ~(((cPortIndexDataIn&1)!=0)?0x0f:0xf0); // Set MUX to zero (function A)
	PORT->Group[0].PINCFG[cPortIndexDataIn].reg =
		PORT_PINCFG_INEN |
		PORT_PINCFG_PULLEN |
		PORT_PINCFG_PMUXEN;
	__enable_irq();
}


void setHardwareCommunicationBackward()
{
	__disable_irq();
	// Data Output:
	// Configure the data out port as input with pull-down.
	PORT->Group[0].DIR.reg &= (~cPortMaskDataOut);
	PORT->Group[0].OUTCLR.reg = cPortMaskDataOut;
	PORT->Group[0].PMUX[cPortIndexDataOut/2].reg &= ~(((cPortIndexDataOut&1)!=0)?0x0f:0xf0); // Set MUX to zero (function A)
	PORT->Group[0].PINCFG[cPortIndexDataOut].reg =
		PORT_PINCFG_INEN |
		PORT_PINCFG_PULLEN |
		PORT_PINCFG_PMUXEN;
	// Data Input:
	// Configure the data in port as output.
	PORT->Group[0].DIR.reg |= cPortMaskDataIn;
	PORT->Group[0].OUTCLR.reg = cPortMaskDataIn;
	PORT->Group[0].PINCFG[cPortIndexDataIn].reg = 0;
	__enable_irq();	
}


void initialize()
{
	setHardwareCommunicationForward();
	
	// Configure the external interrupt to capture all input data.
	PM->APBAMASK.bit.EIC_ = true; // Enable the EIC component clock.
	// Configure the clock for the EIC
	GCLK->CLKCTRL.reg =
		GCLK_CLKCTRL_ID_EIC |
		GCLK_CLKCTRL_GEN_GCLK0 |
		GCLK_CLKCTRL_CLKEN;
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization
	// Reset the EIC (to be sure).
	EIC->CTRL.bit.SWRST = true;
	while (EIC->CTRL.bit.SWRST||EIC->STATUS.bit.SYNCBUSY) {}; // Wait until the component is enabled.
	// Configure to receive an interrupt on both edges and enable filtering.
	EIC->CONFIG[cExtInterruptIndexDataIn/8].reg |= ((EIC_CONFIG_FILTEN0|EIC_CONFIG_SENSE0_BOTH)<<((cExtInterruptIndexDataIn&7)*4));
	while (EIC->STATUS.bit.SYNCBUSY) {}; // Wait until the component is enabled.
	// Configure an interrupt for the sensing on the port
	EIC->INTFLAG.reg = 0x0000ffffUL; // Clear all interrupts.
	EIC->INTENSET.reg = (1UL<<cExtInterruptIndexDataIn); // Enable interrupt for the selected line.
	while (EIC->STATUS.bit.SYNCBUSY) {}; // Wait until the component is enabled.
	// Enable the EIC after configuration.
	EIC->CTRL.bit.ENABLE = true; 
	while (EIC->STATUS.bit.SYNCBUSY) {}; // Wait until the component is enabled.
		
	// Enable the actual interrupt for external interrupt lines.
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_SetPriority(EIC_IRQn, 3);
	NVIC_EnableIRQ(EIC_IRQn);
	
	// Prepare counter 3 to send pulses on the output.
	PM->APBCMASK.bit.TC3_ = true; // Enable power for counter 3
	// Send the main clock to the counter.
	GCLK->CLKCTRL.reg =
		GCLK_CLKCTRL_ID_TC2_TC3 |
		GCLK_CLKCTRL_GEN_GCLK0 |
		GCLK_CLKCTRL_CLKEN;
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization
	// Configure the counter
	TC3->COUNT16.CTRLA.reg =
		TC_CTRLA_PRESCALER_DIV64 |
		TC_CTRLA_WAVEGEN_NFRQ |
		TC_CTRLA_MODE_COUNT16;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Configure the counter in one-shot mode.
	TC3->COUNT16.CTRLBSET.bit.ONESHOT = true;
	// Configure the CC1 value as start of the pulse. Output goes HIGH at this point.
	TC3->COUNT16.CC[1].reg = 0x0010;
	// Configure the CC0 value as stop of the pulse. Output goes LOW at this point.
	TC3->COUNT16.CC[0].reg = 0x0200;
	// Enable interrupt on overflow.
	TC3->COUNT16.INTENSET.bit.OVF = true;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};		
	// Enable the counter.
	TC3->COUNT16.CTRLA.bit.ENABLE = true;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	
	// Enable the actual interrupt for counter 3
	NVIC_ClearPendingIRQ(TC3_IRQn);
	NVIC_SetPriority(TC3_IRQn, 3);
	NVIC_EnableIRQ(TC3_IRQn);
	
	// Prepare counter 2 to measure the incoming pulse length.
	PM->APBCMASK.bit.TC2_ = true; // Enable power for counter 2
	// The counter already have the main clock from the configuration before.
	// Configure the counter as simple infinite 16bit counter at the same frequency as counter 3
	TC2->COUNT16.CTRLA.reg =
		TC_CTRLA_PRESCALER_DIV64 |
		TC_CTRLA_WAVEGEN_NFRQ |
		TC_CTRLA_MODE_COUNT16;
	while (TC2->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Enable the counter.
	TC2->COUNT16.CTRLA.bit.ENABLE = true;
	while (TC2->COUNT16.STATUS.bit.SYNCBUSY) {};	
}
	
	
void waitForNegotiation()
{
	// FIXME!
}


uint8_t getIdentifier()
{
	return gIdentifier;
}


uint8_t getStandLength()
{
	return gStrandLength;
}


void sendData(uint32_t data)
{
	// FIXME!
}


void sendSynchronization()
{
	// FIXME!
}


uint32_t getData()
{
	return gData;
}


void registerSynchronisationFunction(SynchronizationFn synchronizationFn)
{
	gSynchronizationFn = synchronizationFn;
}


void registerReadDataFunction(ReadDataFn readDataFn)
{
	gReadDataFn = readDataFn;
}


/// Read the current value of timer 2
///
uint16_t getTimer2Value()
{
	// Start a read synchronization.
	TC2->COUNT16.READREQ.reg = 
		TC_READREQ_RREQ |
		TC_COUNT16_COUNT_OFFSET;
	while (TC2->COUNT16.STATUS.bit.SYNCBUSY) {}
	return TC2->COUNT16.COUNT.reg;
}


/// Method called on each external interrupt.
///
void onExternalInterrupt()
{
	// Clear all interrupt flags.
	EIC->INTFLAG.reg |= (1UL<<cExtInterruptIndexDataIn);
	
	const bool inputLevel = ((PORT->Group[0].IN.reg & cPortMaskDataIn)!=0);
	if (inputLevel == true) {
		// Capture the current timestamp.
		gInputRisingTimeStamp = getTimer2Value();
	} else {
		// Capture the current timestamp.
		const uint16_t timeStamp = getTimer2Value();
		// Calculate the pulse length
		const uint16_t pulseLength = timeStamp - gInputRisingTimeStamp;
		// Check which pulse matches.
		if (pulseLength > (cPulseTimeZeroBit-cPulseTimingTolerance) && pulseLength < (cPulseTimeZeroBit+cPulseTimingTolerance)) {
			gCurrentReadData >>= 1;
			++gCurrentReadBitCount;
		} else if (pulseLength > (cPulseTimeOneBit-cPulseTimingTolerance) && pulseLength < (cPulseTimeOneBit+cPulseTimingTolerance)) {
			gCurrentReadData >>= 1;
			gCurrentReadData |= 0x80000000UL;
			++gCurrentReadBitCount;
		} else if (pulseLength > (cPulseTimeBreak-cPulseTimingTolerance) && pulseLength < (cPulseTimeBreak+cPulseTimingTolerance)) {
			if (gCurrentReadBitCount == 32) {
				gData = gCurrentReadData;
				if (gReadDataFn != nullptr) {
					gReadDataFn(gData);
				}
			}
			gCurrentReadBitCount = 0;
			gCurrentReadData = 0;		
		} else if (pulseLength > (cPulseTimeSynchronization-cPulseTimingTolerance) && pulseLength < (cPulseTimeSynchronization+cPulseTimingTolerance)) {
			if (gSynchronizationFn != nullptr) {
				gSynchronizationFn();
			}
			gCurrentReadBitCount = 0;
			gCurrentReadData = 0;		
		} else {
			// Unknown signal, reset everything.
			gCurrentReadBitCount = 0;
			gCurrentReadData = 0;
		}
	}
}


/// Method called if the timer overflows.
///
/// This happens after a single signal was sent.
///
void onTimerInterrupt()
{
	// Clear the interrupt.
	TC3->COUNT16.INTFLAG.bit.OVF = true;	
	
	// FIXME!
}


}


void EIC_Handler()
{
	Communication::onExternalInterrupt();
}


void TC3_Handler()
{
	Communication::onTimerInterrupt();
}

