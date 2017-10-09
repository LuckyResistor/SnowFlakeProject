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


#include "PixelMath.hpp"

#include <cstdint>


/// The type used to represent a frame index.
///
typedef uint32_t FrameIndex;


/// A single frame of a scene.
///
/// A frame is the configuration for all pixels at one point in time.
///
class Frame
{
public:
	static const uint8_t cSize = 19;
	
public:
	/// Create a black frame.
	///
	Frame();
	
	/// Create a frame with all pixels at a given value.
	///
	Frame(PixelMath::Value pixelValue);
	
	/// dtor
	///
	~Frame();
	
public:
	/// Write this frame to the display buffer.
	///
	void writeToDisplay();	

public:
	/// The LED level for all LEDs in the range from 0.0-1.0.
	///
	PixelMath::Value pixelValue[cSize];
};
