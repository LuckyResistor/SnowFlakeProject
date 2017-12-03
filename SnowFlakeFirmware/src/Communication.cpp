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


#include "Configuration.hpp"
#include "Hardware.hpp"
#include "Helper.hpp"

#include "Chip.hpp"


namespace Communication {


/// The type of the pulse.
///
enum class Pulse : uint8_t {
	ZeroBit, ///< The zero pulse.
	OneBit, ///< The one pulse.
	Break, ///< The break pulse.
	Synchronization ///< The synchronization pulse.
};


// Forward declarations
void sendPulse(Pulse pulse);
void sendNextBit();


/// The port number for the data input.
///
const Hardware::PortName cPortDataIn = Hardware::PortName::PA25;

/// The external interrupt index for the data input.
///
const uint8_t cExtInterruptIndexDataIn = 13;

/// The port number for the data output.
///
const Hardware::PortName cPortDataOut = Hardware::PortName::PA15;


/// The number of milliseconds to keep the high level on the output.
///
const uint16_t cNegotiationHighLevelDuration = 5; // ~5ms

/// The pause between pulses.
///
const uint16_t cPulseTimePause = 0x0200;

/// The timing for a zero bit.
///
const uint16_t cPulseTimeZeroBit = 0x0200;

/// The timing for a one bit.
///
const uint16_t cPulseTimeOneBit = 0x0400;

/// The timing for a break/separator signal.
///
const uint16_t cPulseTimeBreak = 0x0800;

/// The timing for a synchronization signal.
///
const uint16_t cPulseTimeSynchronization = 0x1000;

/// The detection tolerance of the timings
///
const uint16_t cPulseTimingTolerance = 0x0040;

/// The mask/magic for the identifier.
///
const uint32_t cIdentifierMask = 0x1B720000;


/// The received data.
///
volatile uint32_t gReceivedData = 0;

/// The error code.
///
Error gError = Error::None;

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

/// The current data to send.
///
volatile uint32_t gCurrentSendData = 0;

/// The current number of sent bits.
///
/// For each bit, this counter counts the bit and also the pause.
///
volatile uint8_t gCurrentSendBitCount = 0;


void initialize()
{
	// Disable the communication module if the data lines are used for tracing.
	if (cTraceOutputPins == TraceOutputPins::DataLines) {
		return;
	}
	
	// Data Output:
	// Configure the data out port as output with no input sampling.
	Hardware::setPortConfiguration(cPortDataOut, Hardware::PortConfiguration::Output);
	Hardware::setPeripheralMultiplexing(cPortDataOut, Hardware::Multiplexing::Off);
	
	// Data Input:
	// Configure the data in port as input with pull-down.
	Hardware::setPortConfiguration(cPortDataIn, Hardware::PortConfiguration::Input, Hardware::PortPull::Down);
	Hardware::setPeripheralMultiplexing(cPortDataIn, Hardware::Multiplexing::A);
	
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
		TC_CTRLA_WAVEGEN_MPWM |
		TC_CTRLA_MODE_COUNT16;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Configure the counter in one-shot mode.
	TC3->COUNT16.CTRLBSET.reg = TC_CTRLBSET_ONESHOT;
	// Configure default values for the CC registers.
	TC3->COUNT16.CC[1].reg = 0;
	TC3->COUNT16.CC[0].reg = 0;
	// Enable interrupt on overflow.
	TC3->COUNT16.INTENSET.bit.OVF = true;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};		
	// Enable the counter.
	TC3->COUNT16.CTRLA.bit.ENABLE = true;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Stop the counter and reset it to zero.
	TC3->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;
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


/// Connect the timer to the output.
///
void connectTimerWithDataOutput()
{
	// Change the MUX setting of the output pin to function E, which is connected to the timer.
	Hardware::setPortConfiguration(cPortDataOut, Hardware::PortConfiguration::Disabled);
	Hardware::setPeripheralMultiplexing(cPortDataOut, Hardware::Multiplexing::E);
}

	
bool waitForNegotiation()
{
	// Set output to high to signal next elements in the strand they are not the first one.
	Hardware::setOutput(cPortDataOut, Hardware::PortOutput::High);
	// Wait for the data input to get to high state and keep it on high level for the given time.
	bool isMaster = true;
	for (uint16_t i = 0; i < cNegotiationHighLevelDuration; ++i) {
		if (Hardware::getInput(cPortDataIn)) {
			isMaster = false;
		}
		Helper::delayMs(1);
	}
	// Set the output back to low.
	Hardware::setOutput(cPortDataOut, Hardware::PortOutput::Low);
	// Wait until the input is back to low.
	bool hasTimeout = true;
	for (uint16_t i = 0; i < cNegotiationHighLevelDuration; ++i) {
		if (!Hardware::getInput(cPortDataIn)) {
			hasTimeout = false;
			break;
		}
		Helper::delayMs(1);
	}
	if (hasTimeout) {
		gError = Error::TimeoutWaitingForLow;
		return false;
	}
	// Attach the output port to the timer.
	connectTimerWithDataOutput();
	// If we are in slave mode, we have to wait for the index number.
	if (!isMaster) {
		
		// FIXME! Wait on the identifier from the previous element.
		
	} else {
		// In master mode, we send the identifier to the next element.
		Helper::delayMs(cNegotiationHighLevelDuration);
		sendData(cIdentifierMask + 1); // send identifier 1 to the next element.
	}
	return true;
}


Communication::Error getError()
{
	return gError;
}


uint8_t getIdentifier()
{
	return gIdentifier;
}


uint8_t getStrandLength()
{
	return gStrandLength;
}


void sendData(uint32_t data)
{
	// Disable the communication module if the data lines are used for tracing.
	if (cTraceOutputPins == TraceOutputPins::DataLines) {
		return;
	}

	// If a send is in progress, wait until it finishes.
	while (gCurrentSendBitCount > 0) {
		Helper::delayMs(5);
	}

	// Store the data and trigger sending for the first bit.
	gCurrentSendData = data;
	sendNextBit();			
}


void sendSynchronization()
{
	// Disable the communication module if the data lines are used for tracing.
	if (cTraceOutputPins == TraceOutputPins::DataLines) {
		return;
	}

	// If a send is in progress, wait until it finishes.
	while (gCurrentSendBitCount > 0) {
		Helper::delayMs(5);
	}

	// Start sending a synchronization signal.
	gCurrentSendBitCount = 32;
	sendPulse(Pulse::Synchronization);	
}


uint32_t getData()
{
	return gReceivedData;
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
	
	const bool inputLevel = Hardware::getInput(cPortDataIn);
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
				gReceivedData = gCurrentReadData;
				if (gReadDataFn != nullptr) {
					gReadDataFn(gReceivedData);
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


/// Setup the timer to send a single signal.
///
void sendPulse(Pulse pulse)
{
	// Prepare the timer for the pulse.
	uint16_t pulseTime = cPulseTimePause;
	switch (pulse) {
		case Pulse::ZeroBit: pulseTime = cPulseTimeZeroBit; break;
		case Pulse::OneBit: pulseTime = cPulseTimeOneBit; break;
		case Pulse::Break: pulseTime = cPulseTimeBreak; break;
		case Pulse::Synchronization: pulseTime = cPulseTimeSynchronization; break;
	}
	TC3->COUNT16.CC[0].reg = pulseTime+cPulseTimePause;
	TC3->COUNT16.CC[1].reg = pulseTime;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Trigger the timer
	TC3->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};	
}


/// Send the next bit.
///
void sendNextBit()
{
	if ((gCurrentSendData & 1) == 0) {
		sendPulse(Pulse::ZeroBit);
	} else {
		sendPulse(Pulse::OneBit);
	}
	gCurrentSendData >>= 1;
	++gCurrentSendBitCount;
}


/// Method called if the timer overflows.
///
/// This happens after a single signal was sent.
///
void onTimerInterrupt()
{
	// Clear the interrupt.
	TC3->COUNT16.INTFLAG.bit.OVF = true;	
	
	// Prepare to send the next bit.
	if (gCurrentSendBitCount < 32) {
		// Send the next bit, until all 32 bits are sent.
		sendNextBit();		
	} else if (gCurrentSendBitCount == 32) {
		// After the 32 bit, send a break signal.
		++gCurrentSendBitCount;
		gCurrentSendData = 0;
		sendPulse(Pulse::Break);
	} else if (gCurrentSendBitCount > 32) {
		// Reset the bit counter to signal the end of the send process.
		gCurrentSendBitCount = 0;
		gCurrentSendData = 0;
	}
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

