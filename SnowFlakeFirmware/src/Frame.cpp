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
#include "Frame.hpp"


#include "Display.hpp"

#include <cstring>


Frame::Frame()
{
	std::memset(&pixelValue, 0, sizeof(PixelMath::Value)*cSize);
}


Frame::Frame(PixelMath::Value pixelValue)
{
	for (uint8_t i = 0; i < cSize; ++i) {
		this->pixelValue[i] = pixelValue;
	}
}


Frame::Frame(std::function<PixelMath::Value(uint8_t)> pixelFn)
{
	for (uint8_t i = 0; i < cSize; ++i) {
		this->pixelValue[i] = pixelFn(i);
	}
}


Frame::~Frame()
{
}


void Frame::writeToDisplay()
{
	for (uint8_t i = 0; i < cSize; ++i) {
		Display::setLedLevel(i, PixelMath::convertToInt(pixelValue[i], Display::cMaximumLevel));
	}	
}

