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
	std::memset(&pixelValue, 0, sizeof(PixelValue)*cSize);
}


Frame::Frame(PixelValue pixelValue)
{
	for (uint8_t i = 0; i < cSize; ++i) {
		this->pixelValue[i] = pixelValue;
	}
}


Frame::Frame(std::function<PixelValue(uint8_t)> pixelFn)
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
		Display::setLedLevel(i, pixelValue[i].convertToRange64());
	}	
}


void Frame::blendTo(const Frame &frame, Fixed16 factor)
{
	const auto factorA = PixelValue(factor).inverted();
	const auto factorB = PixelValue(factor);
	for (uint8_t i = 0; i < cSize; ++i) {
		pixelValue[i] = (pixelValue[i]*factorA + frame.pixelValue[i]*factorB);
	}
}


void Frame::multipleWith(const Frame &frame)
{
	for (uint8_t i = 0; i < cSize; ++i) {
		pixelValue[i] *= frame.pixelValue[i];
	}
}


void Frame::addWithLimit(const Frame &frame)
{
	for (uint8_t i = 0; i < cSize; ++i) {
		pixelValue[i] = (pixelValue[i] + frame.pixelValue[i]).limited();
	}
}


void Frame::subtractWithLimit(const Frame &frame)
{
	for (uint8_t i = 0; i < cSize; ++i) {
		pixelValue[i] = (pixelValue[i] - frame.pixelValue[i]).limited();
	}
}

