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


#include "Fixed16.hpp"


/// A simple fixed fraction math pixel value.
///
/// This class is based on calculations from the LibFixMath library.
///
class PixelValue : public Fixed16
{	
public:
	/// Create a zero pixel value.
	///
	constexpr PixelValue() : Fixed16() {}

	/// Create a pixel value from a native value.
	///
	constexpr PixelValue(Type value) : Fixed16(value) {}

	/// Create a pixel value from a `float`.
	///
	constexpr explicit PixelValue(float value) : Fixed16(value) {}

	/// Create a pixel value from an integer.
	///
	constexpr explicit PixelValue(int16_t value) : Fixed16(value) {}

	/// Create a pixel value from a Fixed16 value.
	///
	PixelValue(Fixed16 value);

	/// Create a copy of a pixel value.
	///
	PixelValue(const PixelValue &copy);

	/// Assignment
	///
	PixelValue& operator=(const PixelValue &other);

	/// Assignment
	///
	PixelValue& operator=(const Fixed16 &other);

public: // Constants
	static constexpr PixelValue maximum() { return PixelValue(1.0f); }
	static constexpr PixelValue minimum() { return PixelValue(0.0f); }
	static constexpr PixelValue middle() { return PixelValue(0.5f); }

public: // Pixel helper methods.
	/// Get the inverted value.
	///
	/// x = 1.0 - x
	///
	PixelValue inverted() const;

	/// Get a value in the normalized limits between 0.0 and 1.0.
	///
	/// 5.0 => 1.0
	/// -3.0 => 0.0
	/// 0.5 => 0.5
	///
	PixelValue limited() const;

	/// Get the value wrapped to the acceptable range of 0.0 to 0.9999...
	///
	/// 3.3 => 0.3
	/// -0.5 => 0.5
	/// 0.5 => 0.5
	///
	PixelValue wrapped() const;

	/// Convert a linear ramp into a up/down bounce.
	///
	PixelValue bounced() const;

public: // Creation
	/// Get the a normal value for the given range.
	///
	/// The difference between firstValue and lastValue must not be larger than 0x7fff.
	///
	/// @param firstValue The first value in the range for 0.0.
	/// @param lastValue The last value in the range for 1.0.
	/// @param currentValue The current value.
	///
	template<class T>
	static PixelValue normalFromRange(T firstValue, T lastValue, T currentValue);

public: // Conversion
	/// Convert this pixel value to the display value.
	///
	/// Converts 0.0-1.0 to 0-64 (including 64).
	/// This does no range check. The value must be in the valid range.
	///
	uint8_t convertToRange64() const;
	
public: // Overloaded operators
	PixelValue operator+(const PixelValue &other) const;
	PixelValue operator-(const PixelValue &other) const;
	PixelValue operator*(const PixelValue &other) const;
	PixelValue operator/(const PixelValue &other) const;
	PixelValue& operator+=(const PixelValue &other);
	PixelValue& operator-=(const PixelValue &other);
	PixelValue& operator*=(const PixelValue &other);
	PixelValue& operator/=(const PixelValue &other);
	bool operator==(const PixelValue &other) const;
	bool operator!=(const PixelValue &other) const;
	bool operator<=(const PixelValue &other) const;
	bool operator>=(const PixelValue &other) const;
	bool operator<(const PixelValue &other) const;
	bool operator>(const PixelValue &other) const;
};


template<class T>
PixelValue PixelValue::normalFromRange(T firstValue, T lastValue, T currentValue)
{
	if (firstValue == lastValue) {
		return PixelValue::minimum();
	} else if (firstValue < lastValue) {
		const auto position = PixelValue(static_cast<int16_t>(currentValue-firstValue));
		const auto length = PixelValue(static_cast<int16_t>(lastValue-firstValue));
		return position/length;
	} else {
		const auto position = PixelValue(static_cast<int16_t>(currentValue-lastValue));
		const auto length = PixelValue(static_cast<int16_t>(firstValue-lastValue));
		return (position/length).inverted();
	}
}
