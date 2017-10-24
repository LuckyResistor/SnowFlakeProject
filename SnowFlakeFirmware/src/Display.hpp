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


/// The display module
///
namespace Display {


/// The maximum level for the display
///
extern const uint8_t cMaximumLevel;

/// The LED count.
///
extern const uint8_t cLedCount;


/// Initialize the display module.
///
void initialize();

/// Set the level for one single LED.
///
/// @param ledIndex The LED index from 0-18.
/// @param level The LED brightness level from 0-64. 0 = dark, 64 = 100% on.
///
void setLedLevel(uint8_t ledIndex, uint8_t level);

/// Wait for the next frame and show.
///
/// At 250Hz, this is for a frame rate of ~31 fps, or every 32ms.
///
void synchronizeAndShow();

/// Activate a test pattern.
///
/// The test pattern simply toggles the LED state on each write to get
/// a better impression of the distribution of the value changes.
/// The initial LED state is high.
///
void activateTestPattern();

	
}

