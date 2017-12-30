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
#include "Hardware.hpp"


#include "Configuration.hpp"
#include "Helper.hpp"

#include "Chip.hpp"


namespace Hardware {


/// The trace output masks for output A.
///
const uint32_t cTraceOutputAMasks[] = {0, 1<<15, 1<<27};
	
/// The trace output masks for output B.
///
const uint32_t cTraceOutputBMasks[] = {0, 1<<25, 1<<28};

/// The currently selected mask for trace output A.
///
const uint32_t cTraceOutputAMask = cTraceOutputAMasks[static_cast<uint8_t>(cTraceOutputPins)];

/// The currently selected mask for trace output B.
///
const uint32_t cTraceOutputBMask = cTraceOutputBMasks[static_cast<uint8_t>(cTraceOutputPins)];


/// Insert a short delay in the startup to have time to stop before things start.
///
void initializeDelayStart()
{
	// Avoid deadlock, wait some cycles.
	Helper::delayNop(20000);
}


/// Initialize all required clock sources for the CPU and the buses
///
void initializeCpuClocks()
{
	// Change the timing of the NVM access 
	NVMCTRL->CTRLB.bit.RWS = NVMCTRL_CTRLB_RWS_HALF_Val; // 1 wait state for operating at 2.7-3.3V at 48MHz.
	
	// Enable the bus clock for the clock system.
	PM->APBAMASK.bit.GCLK_ = true;

	// A) Initialize the DFLL to run in closed-loop mode at 48MHz
	// A0. Make a software reset of the clock system.
	GCLK->CTRL.bit.SWRST = true;
	while (GCLK->CTRL.bit.SWRST && GCLK->STATUS.bit.SYNCBUSY) {};
	// A1. Make sure the OCM8M keeps running.
	SYSCTRL->OSC8M.bit.ONDEMAND = 0;
	// A1. Set the division factor to 64, which reduces the 1MHz source to 15.625kHz
	GCLK->GENDIV.reg =
		GCLK_GENDIV_ID(3) | // Select generator 3
		GCLK_GENDIV_DIV(64); // Set the division factor to 64
	// A2. Create generic clock generator 3 for the 15KHz signal of the DFLL
	GCLK->GENCTRL.reg =
		GCLK_GENCTRL_ID(3) | // Select generator 3
		GCLK_GENCTRL_SRC_OSC8M | // Select source OSC8M
		GCLK_GENCTRL_GENEN; // Enable this generic clock generator
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization
	// A3. Configure DFLL with the
	GCLK->CLKCTRL.reg =
		GCLK_CLKCTRL_ID_DFLL48M | // Target is DFLL48M
		GCLK_CLKCTRL_GEN(3) | // Select generator 3 as source.
		GCLK_CLKCTRL_CLKEN; // Enable the DFLL48M
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization
	// A4. Bug workaround, everything is locked if on demand isn't disabled at this point.
	SYSCTRL->DFLLCTRL.bit.ONDEMAND = false;
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}; // Wait for synchronization.
	// A5. Change the multiplication factor.	
	SYSCTRL->DFLLMUL.bit.MUL = 3072; // 48MHz / (1MHz / 64)
	SYSCTRL->DFLLMUL.bit.CSTEP = 1; // Coarse step = 1
	SYSCTRL->DFLLMUL.bit.FSTEP = 1; // Fine step = 1
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}; // Wait for synchronization.
	// A6. Start closed-loop mode
	SYSCTRL->DFLLCTRL.reg |=
		SYSCTRL_DFLLCTRL_MODE | // 1 = Closed loop mode.
		SYSCTRL_DFLLCTRL_QLDIS; // 1 = Disable quick lock.
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}; // Wait for synchronization.
	// A7. Clear the lock flags.
	SYSCTRL->INTFLAG.bit.DFLLLCKC = 1;
	SYSCTRL->INTFLAG.bit.DFLLLCKF = 1;
	SYSCTRL->INTFLAG.bit.DFLLRDY = 1;
	// A8. Enable the DFLL
	SYSCTRL->DFLLCTRL.bit.ENABLE = true;
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}; // Wait for synchronization.
	// A9. Wait for the fine and coarse locks.
	while (!(SYSCTRL->INTFLAG.bit.DFLLLCKC && SYSCTRL->INTFLAG.bit.DFLLLCKF)) {};
	// A10. Wait until the DFLL is ready.
	while (!SYSCTRL->INTFLAG.bit.DFLLRDY) {};
}


/// Switch the CPU speed to the 48MHz clock.
///
void initializeCpuSpeed()
{	
	// Set the divisor of generic clock 0 to 0
	GCLK->GENDIV.reg =
		GCLK_GENDIV_ID(0) | // Select generator 0
		GCLK_GENDIV_DIV(0);
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization		
	// Switch generic clock 0 to the DFLL
	GCLK->GENCTRL.reg =
		GCLK_GENCTRL_ID(0) | // Select generator 0
		GCLK_GENCTRL_SRC_DFLL48M | // Select source DFLL
		GCLK_GENCTRL_IDC | // Set improved duty cycle 50/50
		GCLK_GENCTRL_GENEN; // Enable this generic clock generator
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization
}


/// Initialize one ADC to get entropy for the random number generator.
///
void initializeAdcForRandom()
{
	// Enable power for the ADC peripheral.
	PM->APBCMASK.bit.ADC_ = true;
	// Send the main clock to the ADC.
	GCLK->CLKCTRL.reg =
		GCLK_CLKCTRL_ID_ADC |
		GCLK_CLKCTRL_GEN_GCLK0 |
		GCLK_CLKCTRL_CLKEN;
	while (GCLK->STATUS.bit.SYNCBUSY) {}; // Wait for synchronization

	// Select the internal 1V reference voltage.
	ADC->REFCTRL.reg =
		ADC_REFCTRL_REFSEL_INT1V;
	while (ADC->STATUS.bit.SYNCBUSY == true) {}; // Wait for synchronization.				
	// Select the internal temperature sensor as source.
	ADC->INPUTCTRL.reg =
		ADC_INPUTCTRL_GAIN_1X |
		ADC_INPUTCTRL_MUXNEG_GND |
		ADC_INPUTCTRL_MUXPOS_TEMP;
	while (ADC->STATUS.bit.SYNCBUSY == true) {}; // Wait for synchronization.
	// A slower sample speed.
	ADC->CTRLB.reg =
		ADC_CTRLB_PRESCALER_DIV16;
	while (ADC->STATUS.bit.SYNCBUSY == true) {}; // Wait for synchronization.		
	// Enable the ADC
	ADC->CTRLA.bit.ENABLE = true;
	while (ADC->STATUS.bit.SYNCBUSY == true) {}; // Wait for synchronization.
}


/// Get a single 12bit value from the ADC
///
uint16_t getAdcValue()
{
	// Clear the ready flag.
	ADC->INTFLAG.bit.RESRDY = true;
	// Start the ADC.
	ADC->SWTRIG.bit.START = true;
	while (ADC->STATUS.bit.SYNCBUSY == true) {}; // Wait for synchronization.
	// Wait for the result.
	while (ADC->INTFLAG.bit.RESRDY == false) {};
	// Read the result.
	return ADC->RESULT.reg;
}


uint32_t getEntropy()
{
	const uint8_t bitsToRoll = 5;
	// Start with a value.
	uint32_t result = 0xa927fc19ul;
	// Get 32 values from the ADC which just vary in the first bits of the value.
	for (uint8_t i = 0; i < 32; ++i) {
		const auto value = getAdcValue();
		const auto highBits = static_cast<uint8_t>(result >> (sizeof(uint32_t)*8-bitsToRoll));
		// Integrate the new seed.
		result ^= value;
		// Roll the value by five bits.
		result <<= bitsToRoll;
		result |= highBits;
	}
	return result;
}


/// Initialize the trace outputs
///
void initializeTraceOutputs()
{
	if (cTraceOutputPins != TraceOutputPins::Disabled) {
		// Configure the both trace outputs. Set the direction for these pins to output.
		PORT->Group[0].DIRSET.reg = (cTraceOutputAMask|cTraceOutputBMask);
		// Set both outputs to low.
		PORT->Group[0].OUTCLR.reg = (cTraceOutputAMask|cTraceOutputBMask);
	}
}


void initialize()
{
	if (cTraceStartDelayed) {
		initializeDelayStart();
	}
	initializeCpuClocks();
	initializeCpuSpeed();
	initializeAdcForRandom();
	initializeTraceOutputs();
}


void setPortConfiguration(PortName port, PortConfiguration configuration, PortPull pull)
{
	const uint32_t portMask = getMaskForPort(port);
	if (configuration == PortConfiguration::Output) {
		PORT->Group[0].DIRSET.reg = portMask;
	} else {
		PORT->Group[0].DIRCLR.reg = portMask;
	}
	uint8_t pinConfig = 0;
	if (configuration == PortConfiguration::Input) {
		pinConfig |= PORT_PINCFG_INEN;
	}
	if (pull != PortPull::None) {
		pinConfig |= PORT_PINCFG_PULLEN;
	}
	PORT->Group[0].PINCFG[static_cast<uint8_t>(port)].reg = pinConfig;
	if (pull == PortPull::Up) {
		PORT->Group[0].OUTSET.reg = portMask;
	} else if (pull == PortPull::Down) {
		PORT->Group[0].OUTCLR.reg = portMask;		
	}
}


void setPeripheralMultiplexing(PortName port, Multiplexing muxFunction)
{
	const uint8_t portIndex = static_cast<uint8_t>(port);
	if (muxFunction == Multiplexing::Off) {
		PORT->Group[0].PINCFG[portIndex].reg &= (~PORT_PINCFG_PMUXEN);
	} else {
		uint8_t nativeMuxValue = 0;
		switch (muxFunction) {
			case Multiplexing::A: nativeMuxValue = PORT_PMUX_PMUXE_A_Val; break;
			case Multiplexing::B: nativeMuxValue = PORT_PMUX_PMUXE_B_Val; break;
			case Multiplexing::C: nativeMuxValue = PORT_PMUX_PMUXE_C_Val; break;
			case Multiplexing::D: nativeMuxValue = PORT_PMUX_PMUXE_D_Val; break;
			case Multiplexing::E: nativeMuxValue = PORT_PMUX_PMUXE_E_Val; break;
			case Multiplexing::G: nativeMuxValue = PORT_PMUX_PMUXE_G_Val; break;
			case Multiplexing::H: nativeMuxValue = PORT_PMUX_PMUXE_H_Val; break;
			default: break;
		}
		const uint8_t mask = (((portIndex&1)==0)?0xf0:0x0f);
		const uint8_t data = (nativeMuxValue << (((portIndex&1)==0)?0:4));
		PORT->Group[0].PMUX[portIndex/2].reg = (PORT->Group[0].PMUX[portIndex/2].reg & mask) | data;
		PORT->Group[0].PINCFG[portIndex].reg |= PORT_PINCFG_PMUXEN;		
	}
}


void setOutput(PortName port, PortState output)
{
	const uint32_t mask = getMaskForPort(port);
	if (output == PortState::Low) {
		PORT->Group[0].OUTCLR.reg = mask;
	} else {
		PORT->Group[0].OUTSET.reg = mask;		
	}
}


PortState getInput(PortName port)
{
	const uint32_t mask = getMaskForPort(port);
	if ((PORT->Group[0].IN.reg & mask) != 0) {
		return PortState::High;
	} else {
		return PortState::Low;
	}
}


void setTraceOutputA()
{
	if (cTraceOutputPins != TraceOutputPins::Disabled) {
		PORT->Group[0].OUTSET.reg = cTraceOutputAMask;
	}
}


void clearTraceOutputA()
{
	if (cTraceOutputPins != TraceOutputPins::Disabled) {
		PORT->Group[0].OUTCLR.reg = cTraceOutputAMask;
	}	
}


void toggleTraceOutputA()
{
	if (cTraceOutputPins != TraceOutputPins::Disabled) {
		PORT->Group[0].OUTTGL.reg = cTraceOutputAMask;
	}
}


void setTraceOutputB()
{
	if (cTraceOutputPins != TraceOutputPins::Disabled) {
		PORT->Group[0].OUTSET.reg = cTraceOutputBMask;
	}
}


void clearTraceOutputB()
{
	if (cTraceOutputPins != TraceOutputPins::Disabled) {
		PORT->Group[0].OUTCLR.reg = cTraceOutputBMask;
	}
}


void toggleTraceOutputB()
{
	if (cTraceOutputPins != TraceOutputPins::Disabled) {
		PORT->Group[0].OUTTGL.reg = cTraceOutputBMask;
	}
}


}


