//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2019 by Lucky Resistor. See LICENSE for details.
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


#include "Display.hpp"
#include "Frame.hpp"
#include "PixelValue.hpp"
#include "PixelPosition.hpp"

#include <string.h>


/// A pixel canvas used to create bitmap effects.
///
/// This canvas uses `width` and `height` pixels.
///
/// If you design a pattern, where you like to "paint" to the canvas and
/// translate the painted matrix to the LEDs, this is the right class
/// for you. You also need a LED translation map, to convert the matrix
/// to actual LED positions.
///
/// Using this class takes a lot of memory, so make sure you share this 
/// memory between `getFrame` calls.
///
template<uint16_t width, uint16_t height>
class PixelCanvas
{
public:
	/// The width of this canvas.
	///
	static constexpr uint16_t cWidth = width;
	
	/// The height of this canvas.
	///
	static constexpr uint16_t cHeight = height;
		
public:
	/// Clear the canvas before use.
	///
	void clearCanvas()
	{
		memset(_pixel, 0, sizeof(_pixel));
	}
	
	/// Set a pixel in the canvas.
	///
	void setPixel(const PixelPosition &position, const PixelValue value)
	{
		_pixel[position.y][position.x] = value;
	}
	
	/// Get a pixel in the canvas.
	///
	const PixelValue& getPixel(const PixelPosition &position) const
	{
		return _pixel[position.y][position.x];
	}
	
	/// Calculate a single frame.
	///
	/// @param ledMap A map with positions for each LED.
	///
	Frame getFrame(const PixelPosition *ledMap) const
	{
		Frame frame;
		for (uint8_t i = 0; i < Display::cLedCount; ++i) {
			frame.pixelValue[i] = getPixel(ledMap[i]);
		}
		return frame;
	}
	
private:
	PixelValue _pixel[height][width]; ///< The pixels of the canvas.
};


/// The shared 9x9 canvas.
///
extern PixelCanvas<9, 9> gPixelCanvas9x9;
