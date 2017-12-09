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


/// The hardware abstraction layer.
///
namespace Hardware {


/// The port.
///
enum class PortName : uint8_t {
	PA00 = 0,
	PA01 = 1,
	PA02 = 2,
	PA03 = 3,
	PA04 = 4,
	PA05 = 5,
	PA06 = 6,
	PA07 = 7,
	PA08 = 8,
	PA09 = 9,
	PA10 = 10,
	PA11 = 11,
	PA14 = 14,
	PA15 = 15,
	PA16 = 16,
	PA17 = 17,
	PA18 = 18,
	PA19 = 19,
	PA22 = 22,
	PA23 = 23,
	PA24 = 24,
	PA25 = 25,
	PA27 = 27,
	PA28 = 28,
	PA30 = 30,
	PA31 = 31,
};


/// The multiplexing function.
///
enum class Multiplexing : uint8_t {
	Off, ///< Do not connect to any peripheral.
	A, ///< Connect to external interrupt.
	B, ///< Connect to analog input (REF, ADC, AC, PTC, DAC).
	C, ///< Connect to SERCOM.
	D, ///< Connect to SERCOM-ALT.
	E, ///< Connect to timer (TC).
	G, ///< Connect to COM.
	H, ///< Connect to Generic Clock.
};

/// Port Configuration.
///
enum class PortConfiguration : uint8_t {
	Disabled, ///< Remove the port from the GPIO (Hi-Z)
	Output, ///< Configure the port as output.
	Input, ///< Configure the port as input.
};

/// Pull up/down.
///
enum class PortPull : uint8_t {
	None, ///< Use no pull up/down resistor.
	Up, ///< Connect a pull up resistor.
	Down ///< Connect a pull down resistor.
};

/// The port state.
///
enum class PortState : uint8_t {
	Low, ///< Set the port to high state.
	High ///< Set the port to low state.
};


/// Initialize the MCU hardware.
///
void initialize();	


/// Set the basic port configuration.
///
void setPortConfiguration(PortName port, PortConfiguration configuration, PortPull pull = PortPull::None);

/// Set the peripheral multiplexing function for a pin.
///
void setPeripheralMultiplexing(PortName port, Multiplexing muxFunction);

/// Get the mask for a port.
///
constexpr uint32_t getMaskForPort(PortName port) {
	return (1UL << static_cast<uint8_t>(port));
}

/// Set the output for a port configured as output.
///
void setOutput(PortName port, PortState output);

/// Get the input state from a port configured as input.
///
PortState getInput(PortName port);

/// Set trace output A to high state.
///
void setTraceOutputA();

/// Set trace output A to low state.
///
void clearTraceOutputA();

/// Toggle trace output A.
///
void toggleTraceOutputA();

/// Set trace output B to high state.
///
void setTraceOutputB();

/// Set trace output B to low state.
///
void clearTraceOutputB();

/// Toggle trace output B
///
void toggleTraceOutputB();

	
}
