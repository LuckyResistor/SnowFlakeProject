//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2017-2019 by Lucky Resistor. See LICENSE for details.
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
#include "ElapsedTimer.hpp"
#include "Hardware.hpp"
#include "Helper.hpp"

#include "Chip.hpp"

#include <algorithm>
#include <limits>


namespace Communication {


/// The operating mode.
///
enum class Mode : uint8_t {
	Negotiation, ///< In negotiation mode.
	Master, ///< In master mode.
	Slave, ///< In slave mode.
};


/// The type of the pulse.
///
enum class Pulse : uint8_t {
	ZeroBit, ///< The zero pulse.
	OneBit, ///< The one pulse.
	Break, ///< The break pulse.
	Synchronization ///< The synchronization pulse.
};


// Forward declarations
void resetReadBuffer();
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


/// The pause between pulses.
///
const uint16_t cCounterTimePause = 0x0200;

/// The timing for a zero bit.
///
const uint16_t cCounterTimeZeroBit = 0x0200;

/// The timing for a one bit.
///
const uint16_t cCounterTimeOneBit = 0x0400;

/// The timing for a break/separator signal.
///
const uint16_t cCounterTimeBreak = 0x0800;

/// The timing for a synchronization signal.
///
const uint16_t cCounterTimeSynchronization = 0x1000;

/// The detection tolerance of the timings
///
const uint16_t cCounterTimeTolerance = 0x0080;


/// The duration of a counter cycle for the following timer calculations.
///
/// Formula:
///    (1 / CPU_FREQ) * COUNTER_DIVISOR => duration in seconds.
///
/// CPU_FREQ        = 48'000'000
/// COUNTER_DIVISOR = 64
///
const uint64_t cCpuFrequency = 48000000LL;
const uint64_t cPulseCounterDivisor = 64LL;
const uint64_t cUnitFactorNs = 1000000000LL;
const uint64_t cUnitDivisorNStoMS = 1000000LL;
const uint64_t cCounterCycleDurationNs = (cUnitFactorNs*cPulseCounterDivisor)/cCpuFrequency;
const uint64_t cMaximumTimeOneBitNs = (std::max<uint64_t>(cCounterTimeZeroBit, cCounterTimeOneBit)+static_cast<uint64_t>(cCounterTimePause))*cCounterCycleDurationNs;
const uint64_t cMaximumTimeBreakNs = static_cast<uint64_t>(cCounterTimeBreak+cCounterTimePause)*cCounterCycleDurationNs;
const uint64_t cMaximumTime32BitValueNs = (cMaximumTimeOneBitNs*32LL)+cMaximumTimeBreakNs;


/// A basic unit for the initial delays/timeouts.
///
const uint16_t cNegotiationTimeBlock = 50; // ~50ms

/// The number of milliseconds to keep the high level on the output.
///
const uint16_t cNegotiationHighLevelDuration = (cTraceLongNegotiation ? (cNegotiationTimeBlock*8) : cNegotiationTimeBlock);

/// The timeout waiting for the low level in milliseconds.
///
/// This has to be slightly longer then the high level itself.
///
const uint16_t cNegotiationWaitForLowLevelTimeout = cNegotiationHighLevelDuration + cNegotiationTimeBlock;

/// The number of milliseconds to wait after the high level.
///
/// This is the delay after the master board sets the data out line back to low,
/// before the first identifier is sent to the next board.
///
const uint16_t cNegotiationDelayBeforeIdentifier = cNegotiationTimeBlock;

/// The number of milliseconds which takes to send one single data value.
///
const uint16_t cMaximumTimeToSendValue = static_cast<uint16_t>(cMaximumTime32BitValueNs/cUnitDivisorNStoMS)+1;

/// The number of milliseconds to wait for the identifier.
///
/// This is calculated by the theoretical maximum time it takes for the identifier
/// passed down the whole strand. This equals the maximum time to send one value,
/// multiplied by the maximum number of elements in the strand plus one. This last 
/// increment is required, because the last element will wait until the own identifier
/// is sent to the next element before it is ready to wait for the synchronization.
///
const uint16_t cNegotiationWaitForIdentifierTimeout = cMaximumTimeToSendValue * (cConfigurationStrandElementCount + 1);

/// The number of milliseconds to wait for the initial synchronization pulse.
///
/// The master waits `cNegotiationWaitForIdentifierTimeout` after sending the first identifier
/// before it sends the initial synchronization pulse. Therefore this timeout has to be longer
/// than the timeout to wait for the identifier. 
///
const uint16_t cNegotiationWaitForInitialSynchronization = cNegotiationWaitForIdentifierTimeout + cMaximumTimeToSendValue;

/// The time-out for data being sent.
///
const uint32_t cDataSendTimeout = cMaximumTimeToSendValue + 10;


/// The mask/magic for the identifier.
///
const uint32_t cIdentifierMask = 0x1B720000;


/// The received data.
///
volatile uint32_t gReceivedData = 0;

/// Flag that new data was received.
///
volatile bool gNewDataReceived = false;

/// Flag is a synchronization pulse was received.
///
volatile bool gNewSynchronizationReceived = false;

/// The error code.
///
Error gError = Error::None;

/// The identifier for this board.
///
uint8_t gIdentifier = 0;

/// The current communication mode.
///
volatile Mode gMode = Mode::Negotiation;

/// The registered synchronization function.
///
SynchronizationFn gSynchronizationFn = nullptr;

/// The registered read data function.
///
ReadDataFn gReadDataFn = nullptr;

/// The button press function.
///
ButtonPressFn gButtonPressFn = nullptr;


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

/// A flag if the input data should be mirrored to the data output.
///
volatile bool gDataMirrorEnabled = false;


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
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Configure default values for the CC registers.
	TC3->COUNT16.CC[0].reg = cCounterTimeBreak + cCounterTimePause;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	TC3->COUNT16.CC[1].reg = cCounterTimeBreak;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Enable interrupt on overflow.
	TC3->COUNT16.INTENSET.bit.OVF = true;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Enable the counter.
	TC3->COUNT16.CTRLA.bit.ENABLE = true;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Stop the counter and reset it to zero.
	TC3->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
	
	// Enable the actual interrupt for counter 3
	NVIC_ClearPendingIRQ(TC3_IRQn);
	NVIC_SetPriority(TC3_IRQn, 3);
	NVIC_EnableIRQ(TC3_IRQn);
}


/// Connect the timer to the data output.
///
void enableTimerDataOutput()
{
	// Change the MUX setting of the output pin to function E, which is connected to the timer.
	Hardware::setPortConfiguration(cPortDataOut, Hardware::PortConfiguration::Disabled);
	Hardware::setPeripheralMultiplexing(cPortDataOut, Hardware::Multiplexing::E);
}


/// Revert the data output port into regular IO port
///
void disableTimerDataOutput()
{
	Hardware::setPortConfiguration(cPortDataOut, Hardware::PortConfiguration::Output);
	Hardware::setPeripheralMultiplexing(cPortDataOut, Hardware::Multiplexing::Off);	
}


/// Enable the interrupt to capture values from data in.
///
void enableDataInputInterrupt()
{
	__disable_irq();
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_SetPriority(EIC_IRQn, 3);
	NVIC_EnableIRQ(EIC_IRQn);
	__enable_irq();
}


/// Enable data mirroring.
///
void enableDataMirroringMode()
{
	disableTimerDataOutput();
	gDataMirrorEnabled = true;
}

/// Enable the timer for input detection.
///
void enableInputDetection(bool isMaster)
{
	// Prepare counter 2 to measure the incoming pulse length.
	PM->APBCMASK.bit.TC2_ = true; // Enable power for counter 2
	// The counter already have the main clock from the configuration before.
	// Configure the counter as simple infinite 16bit counter at the same frequency as counter 3
	TC2->COUNT16.CTRLA.reg =
		(isMaster ? TC_CTRLA_PRESCALER_DIV1024 : TC_CTRLA_PRESCALER_DIV64) |
		TC_CTRLA_WAVEGEN_NFRQ |
		TC_CTRLA_MODE_COUNT16;
	while (TC2->COUNT16.STATUS.bit.SYNCBUSY) {};
	// Enable the counter.
	TC2->COUNT16.CTRLA.bit.ENABLE = true;
	while (TC2->COUNT16.STATUS.bit.SYNCBUSY) {};	
	// Enable the interrupt.
	enableDataInputInterrupt();
}


/// Wait for the given state to change.
///
/// @param state The current state which shall change.
/// @param timeout The timeout in milliseconds.
/// @return true if the state changes, false if there was a timeout.
///
bool waitForInputStateChange(Hardware::PortState state, uint32_t timeout)
{
	ElapsedTimer elapsedTimer;
	elapsedTimer.start();
	while (Hardware::getInput(cPortDataIn) == state) {
		if (elapsedTimer.hasTimeout(timeout)) {
			return false;
		}
	}	
	return true;
}


/// Put the output into high state and wait for a signal on input.
///
/// @return true if the input signal was detected, false if there was a timeout.
///
bool signalNextAndWaitForSignalFromPrevious()
{
	// Set output to high to signal next elements in the strand they are not the first one.
	Hardware::setOutput(cPortDataOut, Hardware::PortState::High);
	// Wait for the data input to go into high state.
	return waitForInputStateChange(Hardware::PortState::Low, cNegotiationHighLevelDuration);
}


/// Negotiation in master mode.
///
/// @return true on success, false on any error.
///
bool masterNegotiation()
{
	// Set the output back to low, this will happen after the `cNegotiationHighLevelDuration` time.
	Hardware::setOutput(cPortDataOut, Hardware::PortState::Low);
	// Attach the output port to the timer.
	enableTimerDataOutput();
	// Wait a moment to give other elements the time to prepare for the incoming identifier.
	Helper::delayMs(cNegotiationDelayBeforeIdentifier);
	// Send identifier 1 to the next element.
	if (!sendData(cIdentifierMask + 1)) {
		gError = Error::TimeoutWaitingToSendData;
		return false;		
	}
	// Wait until all identifiers are sent along the strand.
	Helper::delayMs(cNegotiationWaitForIdentifierTimeout);
	// Send the synchronization signal, to start all boards at the same time.
	if (!sendSynchronization()) {
		gError = Error::TimeoutWaitingToSendData;
		return false;		
	}
	// Wait for the synchronization being sent.
	if (!waitUntilReadyToSend()) {
		gError = Error::TimeoutWaitingToSendData;
		return false;
	}
	// Enable the detection of button presses.
	enableInputDetection(true);
	// Set the mode.
	gMode = Mode::Master;
	// Success.
	return true;
}


/// Negotiation in slave mode.
///
/// @return true on success, false on any error.
///
bool slaveNegotiation()
{
	// As slave we wait for the low state from the master to keep this synchronized.
	if (!waitForInputStateChange(Hardware::PortState::High, cNegotiationWaitForLowLevelTimeout)) {
		gError = Error::TimeoutWaitingForLow;
		return false;		
	}
	// Set the output to low synchronized with the master. 
	Hardware::setOutput(cPortDataOut, Hardware::PortState::Low);
	// Connect the timer to the output
	enableTimerDataOutput();
	// Enable the interrupt to capture data from data in.
	enableInputDetection(false);
	// Reset the read buffer
	resetReadBuffer();
	// Wait for a received value. Expecting an identifier from the previous element.
	if (!waitForData(cNegotiationWaitForIdentifierTimeout)) {
		gError = Error::TimeoutWaitingForIdentifier;
		return false;		
	}
	// Check what we got. Make sure this is a valid identifier.
	const auto value = readData();
	if ((value & 0xffffff00) == cIdentifierMask) {
		gIdentifier = static_cast<uint8_t>(value & 0xff);
		if (gIdentifier == 0 || gIdentifier >= 0x40) {
			gError = Error::InvalidIdentifier;
			return false;
		}
	} else {
		gError = Error::InvalidIdentifier;
		return false;
	}
	// Looks good. After receiving the correct identifier, send the next identifier down the strand.
	sendData(cIdentifierMask + gIdentifier + 1);
	// Wait until this value was sent.
	if (!waitUntilReadyToSend()) {
		gError = Error::TimeoutWaitingToSendData;
		return false;
	}
	// Enable the data mirroring mode.
	enableDataMirroringMode();
	// Now wait for the synchronization signal from the master board.
	if (!waitForSynchonization(cNegotiationWaitForInitialSynchronization)) {
		gError = Error::TimeoutWaitingForInitialSynchronization;
		return false;		
	}
	// Set the mode.
	gMode = Mode::Slave;
	// Success.
	return true;
}

	
bool waitForNegotiation()
{
	// Check if there is a previous element.
	const bool hasPreviousElement = signalNextAndWaitForSignalFromPrevious();
	if (!hasPreviousElement) {
		return masterNegotiation();
	} else {
		return slaveNegotiation();
	}
}


Communication::Error getError()
{
	return gError;
}


uint8_t getIdentifier()
{
	return gIdentifier;
}


bool sendData(uint32_t data)
{
	// Disable the communication module if the data lines are used for tracing.
	if (cTraceOutputPins == TraceOutputPins::DataLines) {
		return false;
	}

	// If a send is in progress, wait until it finishes.
	ElapsedTimer elapsedTimer;
	elapsedTimer.start();
	while (!isReadyToSend()) {
		if (elapsedTimer.hasTimeout(cDataSendTimeout)) {
			return false;
		}
	}

	// Store the data and trigger sending for the first bit.
	gCurrentSendData = data;
	sendNextBit();
	
	// Success.
	return true;
}


bool sendSynchronization()
{
	// Disable the communication module if the data lines are used for tracing.
	if (cTraceOutputPins == TraceOutputPins::DataLines) {
		return false;
	}

	// If a send is in progress, wait until it finishes.
	ElapsedTimer elapsedTimer;
	elapsedTimer.start();
	while (!isReadyToSend()) {
		if (elapsedTimer.hasTimeout(cDataSendTimeout)) {
			return false;
		}
	}

	// Start sending a synchronization signal.
	gCurrentSendBitCount = 33;
	sendPulse(Pulse::Synchronization);
	
	// Success.
	return true;
}


bool isReadyToSend()
{
	return gCurrentSendBitCount == 0;
}


bool waitUntilReadyToSend()
{
	ElapsedTimer elapsedTimer;
	elapsedTimer.start();
	while (!isReadyToSend()) {
		if (elapsedTimer.hasTimeout(cDataSendTimeout)) {
			return false; // Time-out
		}
	}
	
	// Success.
	return true;
}


bool waitForData(uint32_t timeout)
{
	gNewDataReceived = false;
	ElapsedTimer elapsedTimer;
	elapsedTimer.start();
	while (!gNewDataReceived) {
		if (elapsedTimer.hasTimeout(timeout)) {
			return false; // Time-out
		}
	}
	
	// Success.
	return true;
}


uint32_t readData()
{
	return gReceivedData;
}


bool waitForSynchonization(uint32_t timeout)
{
	gNewSynchronizationReceived = false;
	ElapsedTimer elapsedTimer;
	elapsedTimer.start();
	while (!gNewSynchronizationReceived) {
		if (elapsedTimer.hasTimeout(timeout)) {
			return false; // Time-out
		}
	}
	
	// Success.
	return true;
}


void registerSynchronisationFunction(SynchronizationFn synchronizationFn)
{
	gSynchronizationFn = synchronizationFn;
}


void registerReadDataFunction(ReadDataFn readDataFn)
{
	gReadDataFn = readDataFn;
}


void registerButtonPressFunction(ButtonPressFn buttonPressFn)
{
	gButtonPressFn = buttonPressFn;
}


/// Restart timer 2
///
void restartTimer2()
{
	// This will set the counter back to zero and start it from there.
	TC2->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
	while (TC2->COUNT16.STATUS.bit.SYNCBUSY) {}
	// Also clear the overflow flag.
	TC2->COUNT16.INTFLAG.bit.OVF = true;
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


/// Reset the input read buffer.
///
void resetReadBuffer()
{
	gCurrentReadData = 0;
	gCurrentReadBitCount = 0;
	gReceivedData = 0;
	gNewDataReceived = false;
}


/// Check if a pulse matches the given time
///
inline bool isPulseTimeEqual(const uint16_t pulseLength, const uint16_t expectedLength)
{
	return (pulseLength > (expectedLength - cCounterTimeTolerance) && pulseLength < (expectedLength + cCounterTimeTolerance));
}


/// Handling of pulses in slave mode.
///
void handlePulseInSlaveMode(uint16_t pulseLength)
{
	// Check which pulse matches.
	if (isPulseTimeEqual(pulseLength, cCounterTimeZeroBit)) {
		gCurrentReadData >>= 1;
		++gCurrentReadBitCount;
	} else if (isPulseTimeEqual(pulseLength, cCounterTimeOneBit)) {
		gCurrentReadData >>= 1;
		gCurrentReadData |= 0x80000000UL;
		++gCurrentReadBitCount;
	} else if (isPulseTimeEqual(pulseLength, cCounterTimeBreak)) {
		if (gCurrentReadBitCount == 32) {
			gReceivedData = gCurrentReadData;
			gNewDataReceived = true;
			if (gReadDataFn != nullptr) {
				gReadDataFn(gReceivedData);
			}
		}
		gCurrentReadBitCount = 0;
		gCurrentReadData = 0;
	} else if (isPulseTimeEqual(pulseLength, cCounterTimeSynchronization)) {
		if (gSynchronizationFn != nullptr) {
			gSynchronizationFn();
		}
		gNewSynchronizationReceived = true;
		gCurrentReadBitCount = 0;
		gCurrentReadData = 0;
	} else {
		// Ignore any unknown pulse lengths. Could be noise.
	}
}


/// Handling of pulses in master mode (button presses by the user).
///
void handlePulseInMasterMode(uint16_t pulseLength)
{
	const uint16_t cMinimumPress = 0x0080; // Everything shorter than this is noise
	const uint16_t cShortPress = 0x8000; // ~700ms
	if (gButtonPressFn != nullptr) {
		if (pulseLength > cMinimumPress && pulseLength <= cShortPress) {
			gButtonPressFn(ButtonPress::Short);
			} else if (pulseLength > cShortPress) {
			gButtonPressFn(ButtonPress::Long);
		}
	}
}


/// Method called on each external interrupt.
///
void onExternalInterrupt()
{
	// Clear all interrupt flags.
	EIC->INTFLAG.reg |= (1UL<<cExtInterruptIndexDataIn);
	
	// Check the state of the data input line.
	auto inputPortState = Hardware::getInput(cPortDataIn);
	
	// Mirror the signal if this is enabled
	if (gDataMirrorEnabled) {
		Hardware::setOutput(cPortDataOut, inputPortState);		
	}
	// Measure the pulse lengths and detect the received signals.
	if (inputPortState == Hardware::PortState::High) {
		restartTimer2();
	} else {
		// Get the length of the pulse.
		uint16_t pulseLength = getTimer2Value();
		// Check for an overflow, in this case use the maximum pulse length.
		if (TC2->COUNT16.INTFLAG.bit.OVF) {
			pulseLength = std::numeric_limits<uint16_t>::max();
		}
		if (gMode == Mode::Master) {
			handlePulseInMasterMode(pulseLength);
		} else {
			handlePulseInSlaveMode(pulseLength);
		}
	}		
}


/// Setup the timer to send a single signal.
///
void sendPulse(Pulse pulse)
{
	// Prepare the timer for the pulse.
	uint16_t pulseTime = cCounterTimePause;
	switch (pulse) {
		case Pulse::ZeroBit: pulseTime = cCounterTimeZeroBit; break;
		case Pulse::OneBit: pulseTime = cCounterTimeOneBit; break;
		case Pulse::Break: pulseTime = cCounterTimeBreak; break;
		case Pulse::Synchronization: pulseTime = cCounterTimeSynchronization; break;
	}
	TC3->COUNT16.CC[0].reg = pulseTime+cCounterTimePause;
	while (TC3->COUNT16.STATUS.bit.SYNCBUSY) {};
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

