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


/// Simple math for pixel calculations
///
namespace PixelMath {

		
/// The type used to represent one pixel value.
///
typedef float Value;


/// Invert a pixel value.
///
inline Value invert(Value value) {
	return 1.0f-value;
}

	
/// Put a value into the acceptable range.
///
inline Value limit(Value value) {
	if (value < 0.0f) {
		return 0.0f;
	} else if (value > 1.0f) {
		return 1.0f;
	} else {
		return value;
	}
}

	
/// Convert a pixel value to the display value.
///
inline uint8_t convertToInt(Value value, uint8_t maximumInt) {
	return static_cast<uint8_t>(static_cast<Value>(maximumInt)*limit(value));
}


/// Get the normal value from the given range.
///
/// @param firstValue The first value in the range for 0.0.
/// @param lastValue The last value in the range for 1.0.
/// @param currentValue The current value.
///
template<typename T>
inline Value normalFromRange(T firstValue, T lastValue, T currentValue) {
	if (firstValue == lastValue) {
		return 0.0f;
	} else if (firstValue < lastValue) {
		const auto position = static_cast<Value>(currentValue-firstValue);
		const auto length = static_cast<Value>(lastValue-firstValue);
		return position/length;
	} else {
		const auto position = static_cast<Value>(currentValue-lastValue);
		const auto length = static_cast<Value>(firstValue-lastValue);
		return invert(position/length);
	}
}


};

