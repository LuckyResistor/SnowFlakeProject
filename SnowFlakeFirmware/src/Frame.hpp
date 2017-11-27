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


#include "PixelValue.hpp"

#include <cstdint>
#include <functional>


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
	Frame(PixelValue pixelValue);
	
	/// Create a frame, using the given function.
	///
	/// The function is called for each pixel, which is passed as the only argument.
	/// It has to return the pixel value.
	///
	Frame(std::function<PixelValue(uint8_t)> pixelFn);
	
	/// dtor
	///
	~Frame();
	
public:
	/// Write this frame to the display buffer.
	///
	void writeToDisplay();
	
	/// Blend this frame with another one.
	///
	/// @param frame The other frame to blend into.
	/// @param factor The blend factor. 0.0 = keep original, 1.0 = only the given frame.
	///
	void blendTo(const Frame &frame, Fixed16 factor);

	/// Multiple this frame with another one, value by value.
	///
	/// @param frame The other frame to multiple with this one.
	///
	void multipleWith(const Frame &frame);

	/// Add the values of another frame to this one, but limit the result to normals.
	///
	/// @param frame The other frame to add to this one.
	///
	void addWithLimit(const Frame &frame);

	/// Subtract the values of another frame to this one, but limit the result to normals.
	///
	/// @param frame The other frame to subtract from this one.
	///
	void subtractWithLimit(const Frame &frame);

public:
	/// The LED level for all LEDs in the range from 0.0-1.0.
	///
	PixelValue pixelValue[cSize];
};
