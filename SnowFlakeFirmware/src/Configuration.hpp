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


/// The configuration for trace outputs.
///
enum class TraceOutputPins : uint8_t {
	Disabled = 0, ///< The trace output is disabled.
	DataLines = 1, ///< The trace output is routed to the data-in and data-out lines.
	PA27_28 = 2 ///< The trace output is routed to the PA27 and PA28 pins (pin 25 + 27).
};

/// The source for the output signals.
///
enum class TraceOutputSource : uint8_t {
	
	/// The trace output is disabled.
	///
	Disabled, 
	
	/// Output A is set to high while a new frame is calculated.
	/// Output B is set to high at the frame synchronization.
	///
	FrameClaculationTime,
	
	/// Output A is set high while the display interrupt is running.
	/// Output B is set high for the interrupt of PWM counter 0.
	///
	DisplayInterruptTime,
};


/// The number of elements in the strand.
///
const uint8_t cConfigurationStrandElementCount = 5;

/// Should the start be delayed?
///
const bool cTraceStartDelayed = false; // false for production.

/// Use a very long negotiation duration.
///
/// This is used if you have attached programmer to the chip which delays the
/// start up to one second.
///
const bool cTraceLongNegotiation = true; // false for production.

/// Display the identifier for two seconds after initialization.
///
/// If you enable this, each snow flake will briefly show the identifier after
/// the negotiation. The identifier is displayed by lighting up the LED with the
/// identifier number and the center LED.
///
const bool cTraceShowIdentifierOnStart = true; // false for production.

/// Blend on the same scene, even there is only one defined.
///
/// You can enable this to test different entropy values for a single scene.
///
const bool cTraceBlendOnSingleScene = false; // false for production.

/// The configuration for the trace outputs pins.
///
const TraceOutputPins cTraceOutputPins = TraceOutputPins::Disabled; // Disabled for production.

/// The configuration for the trace output source.
///
const TraceOutputSource cTraceOutputSource = TraceOutputSource::Disabled; // Disabled for production.


