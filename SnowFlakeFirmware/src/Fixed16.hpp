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


/// Fixed 16bit math.
///
class Fixed16
{
public:
	/// The internal type of the pixel value.
	///
	typedef int32_t Type;

public: // Constructors
	/// Create a zero pixel value.
	///
	constexpr Fixed16() : _value(0) {}

	/// Create a pixel value from the internal type.
	///
	constexpr Fixed16(Type value) : _value(value) {};

	/// Create a pixel value from the given float.
	///
	constexpr Fixed16(float value) : _value(static_cast<Type>((value>=0)?(value*65536.0f+0.5f):(value*65536.0f-0.5f))) {}

	/// Create a pixel value from the given integer.
	///
	constexpr Fixed16(int16_t value) : _value(static_cast<Type>(value)<<16) {};

	/// Copy constructor.
	///
	Fixed16(const Fixed16 &value);

public: // Assignment
	Fixed16& operator=(const Fixed16 &other);

public: // Operators
	Fixed16 operator+(const Fixed16 &other) const;
	Fixed16 operator-(const Fixed16 &other) const;
	Fixed16 operator*(const Fixed16 &other) const;
	Fixed16 operator/(const Fixed16 &other) const;

public: // Manipulators
	Fixed16& operator+=(const Fixed16 &other);
	Fixed16& operator-=(const Fixed16 &other);
	Fixed16& operator*=(const Fixed16 &other);
	Fixed16& operator/=(const Fixed16 &other);

public: // Saturating operators
	Fixed16 saturatingAdd(const Fixed16 &other) const;
	Fixed16 saturatingSubtract(const Fixed16 &other) const;
	Fixed16 saturatingMultiply(const Fixed16 &other) const;
	Fixed16 saturatingDivide(const Fixed16 &other) const;

public: // Comparators
	bool operator==(const Fixed16 &other) const;
	bool operator!=(const Fixed16 &other) const;
	bool operator<=(const Fixed16 &other) const;
	bool operator>=(const Fixed16 &other) const;
	bool operator<(const Fixed16 &other) const;
	bool operator>(const Fixed16 &other) const;

public: // Constants
	static constexpr Fixed16 one() { return Fixed16(static_cast<Type>(0x00010000)); }
	static constexpr Fixed16 pi() { return Fixed16(static_cast<Type>(205887)); }
	static constexpr Fixed16 e() { return Fixed16(static_cast<Type>(178145)); }
	static constexpr Fixed16 minimum() { return Fixed16(static_cast<Type>(0x80000000)); }
	static constexpr Fixed16 maximum() { return Fixed16(static_cast<Type>(0x7FFFFFFF)); }
	static constexpr Fixed16 overflow() { return Fixed16(static_cast<Type>(0x80000000)); }

public: // Checks
	/// Check if this value is zero
	///
	bool isZero() const {
		return _value == 0;
	}
	
public: // Math
	/// Get the absolute value.
	///
	inline Fixed16 getAbsolute() const {
		return Fixed16(_value < 0 ? -_value : _value);
	}
	
	/// Get the floor value.
	///
	inline Fixed16 getFloor() const {
		return Fixed16(static_cast<Type>(_value & 0xffff0000u));
	}
	
	/// Get the ceil value.
	///
	inline Fixed16 getCeiling() const {
		return Fixed16(static_cast<Type>((_value & 0xFFFF0000UL) + (((_value & 0x0000FFFFUL) != 0)?0x00010000:0x00000000)));
	}

	/// Get just the fraction.
	///
	inline Fixed16 getFraction() const {
		return Fixed16(static_cast<Type>(_value & 0x0000ffffu));
	}
	
	/// Get the minimum from this and the other value.
	///
	inline Fixed16 getMinimum(const Fixed16 &other) const {
		return Fixed16((_value < other._value) ? _value : other._value);
	}
	
	/// Get the maximum from this and the other value.
	///
	inline Fixed16 getMaximum(const Fixed16 &other) const {
		return Fixed16((_value > other._value) ? _value : other._value);
	}
	
	/// Clamp this value to the selected range.
	///
	inline Fixed16 getClamped(const Fixed16 &minimum, const Fixed16 &maximum) const {
		return (*this).getMinimum(maximum).getMaximum(minimum);
	}

public: // Conversions
	/// Access the raw value.
	///
	inline Type toRawValue() const { return _value; }

	/// Get the integer part of this value.
	///
	inline int16_t toRawInteger() const { return (_value >> 16); }
		
	/// Get the fraction part of this value.
	///
	inline uint16_t toRawFraction() const {
		return static_cast<uint16_t>(_value & 0x0000ffffu);
	}

	/// Get a float value.
	///
	float toFloat() const;

protected:
	Type _value;
};

