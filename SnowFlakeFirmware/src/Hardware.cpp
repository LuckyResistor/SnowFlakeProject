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
#include "Hardware.h"


#include "Helper.h"

#include "sam.h"


namespace Hardware {


/// The port mask for all pins used for LEDs
///
const uint32_t cPortMaskLed = 
	(1UL<<0)|
	(1UL<<1)|
	(1UL<<2)|
	(1UL<<3)|
	(1UL<<4)|
	(1UL<<5)|
	(1UL<<6)|
	(1UL<<7)|
	(1UL<<8)|
	(1UL<<9)|
	(1UL<<10)|
	(1UL<<11)|
	(1UL<<16)|
	(1UL<<17)|
	(1UL<<18)|
	(1UL<<19)|
	(1UL<<22)|
	(1UL<<23)|
	(1UL<<24);

/// The mask for the data input.
///
const uint32_t cPortMaskDataIn = (1UL<<25);

/// The mask for the data output.
///
const uint32_t cPortMaskDataOut = (1UL<<15);


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
	while (!SYSCTRL->INTFLAG.bit.DFLLLCKC && !SYSCTRL->INTFLAG.bit.DFLLLCKF) {};
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


/// Initialize the configuration of the GPIO
///
void initializeGpio()
{
	// Configure all LED pins as outputs.
	PORT->Group[0].DIR.reg |= cPortMaskLed | cPortMaskDataOut;
	// Set all LED pins to high to turn the LEDs off.
	PORT->Group[0].OUT.reg |= cPortMaskLed;
	// Configure the data in port as input with pull-down.
	PORT->Group[0].WRCONFIG.reg =
		PORT_WRCONFIG_HWSEL |
		PORT_WRCONFIG_WRPINCFG |
		PORT_WRCONFIG_PULLEN |
		PORT_WRCONFIG_INEN |
		(cPortMaskDataIn >> 16);
}


void initialize()
{
	initializeDelayStart();
	initializeCpuClocks();
	initializeCpuSpeed();
	initializeGpio();
}

	
void setLedPinLevels(uint32_t outputMask)
{
	PORT->Group[0].OUT.reg = (PORT->Group[0].OUT.reg & (~cPortMaskLed)) | (outputMask & cPortMaskLed);
}


}
