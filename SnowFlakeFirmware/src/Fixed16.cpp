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
// 
#include "Fixed16.hpp"


// Note: Parts of this code were converted from the libfixmath library into C++.
// https://github.com/PetteriAimonen/libfixmath


namespace {


const Fixed16::Type cMaximumValue = 0x7FFFFFFF; ///< The maximum value.
const Fixed16::Type cMinimumValue = 0x80000000; ///< The minimum value.
const Fixed16::Type cOverflowValue = 0x80000000; ///< The value used to indicate overflows.
const Fixed16::Type cOneValue = 0x00010000; ///< The value for one.

	
Fixed16::Type fixedAdd(Fixed16::Type a, Fixed16::Type b)
{
	const auto sum = a + b;
	if (!((a^b) & 0x80000000) && ((a^sum) & 0x80000000)) {
		return cOverflowValue;	
	}
	return sum;
}


Fixed16::Type fixedSubtract(Fixed16::Type a, Fixed16::Type b)
{
	const auto diff = a - b;
	if (((a^b) & 0x80000000) && ((a^diff) & 0x80000000)) {
		return cOverflowValue;
	}
	return diff;
}


Fixed16::Type fixedMultiply(Fixed16::Type a, Fixed16::Type b)
{
	const int32_t partA = (a >> 16);
	const int32_t partC = (b >> 16);
	const uint32_t partB = (a & 0xFFFF);
	const uint32_t partD = (b & 0xFFFF);
	const int32_t resultAC = partA*partC;
	const int32_t resultADplusCB = partA*partD + partC*partB;
	const uint32_t resultBD = partB*partD;
	
	int32_t productHigh = resultAC + (resultADplusCB >> 16);
	uint32_t productLow = resultBD + (resultADplusCB << 16);
	if (productLow < resultBD) {
		productHigh++;		
	}
	if ((productHigh >> 31) != (productHigh >> 15)) {
		return cOverflowValue;
	}
	const uint32_t productLowCopy = productLow;
	productLow -= 0x8000;
	productLow -= (static_cast<uint32_t>(productHigh) >> 31);
	if (productLow > productLowCopy) {
		productHigh--;
	}
	Fixed16::Type result = (productHigh << 16) | (productLow >> 16);
	result += 1;
	return result;
}


Fixed16::Type fixedDivide(Fixed16::Type a, Fixed16::Type b)
{
	// Basic binary restoring division algorithm.
	if (b == 0) {
		return cMinimumValue;
	}
	
	uint32_t remainder = (a >= 0) ? a : (-a);
	uint32_t divider = (b >= 0) ? b : (-b);

	uint32_t quotient = 0;
	uint32_t bit = 0x10000;
	
	// The algorithm requires D >= R
	while (divider < remainder) {
		divider <<= 1;
		bit <<= 1;
	}
	
	if (!bit) {
		return cOverflowValue;
	}
	
	if (divider & 0x80000000) {
		// Perform one step manually to avoid overflows later.
		// We know that divider's bottom bit is 0 here.
		if (remainder >= divider) {
			quotient |= bit;
			remainder -= divider;
		}
		divider >>= 1;
		bit >>= 1;
	}
	
	// Main division loop
	while (bit && remainder) {
		if (remainder >= divider) {
			quotient |= bit;
			remainder -= divider;
		}
		remainder <<= 1;
		bit >>= 1;
	}
	
	if (remainder >= divider) {
		quotient++;
	}
	
	Fixed16::Type result = quotient;
	
	// Figure out the sign of result
	if ((a ^ b) & 0x80000000) {
		if (result == cMinimumValue) {
			return cOverflowValue;
		}
		result = -result;
	}
	
	return result;
}


Fixed16::Type fixedSaturatingAdd(Fixed16::Type a, Fixed16::Type b)
{
	const auto result = fixedAdd(a, b);
	if (result == cOverflowValue) {
		if (a >= 0) {
			return cMaximumValue;
			} else {
			return cMinimumValue;
		}
	}
	return result;
}


Fixed16::Type fixedSaturatingSubtract(Fixed16::Type a, Fixed16::Type b)
{
	const auto result = fixedSubtract(a, b);
	if (result == cOverflowValue) {
		if (a >= 0) {
			return cMaximumValue;
			} else {
			return cMinimumValue;
		}
	}
	return result;
}


Fixed16::Type fixedSaturatingMultiply(Fixed16::Type a, Fixed16::Type b)
{
	const auto result = fixedMultiply(a, b);
	if (result == cOverflowValue) {
		if ((a >= 0) == (b >= 0)) {
			return cMaximumValue;
		} else {
			return cMinimumValue;
		}
	}
	return result;
}


Fixed16::Type fixedSaturatingDivide(Fixed16::Type a, Fixed16::Type b)
{
	const auto result = fixedDivide(a, b);
	if (result == cOverflowValue) {
		if ((a >= 0) == (b >= 0)) {
			return cMaximumValue;
		} else {
			return cMinimumValue;
		}
	}
	return result;
}


}


Fixed16::Fixed16(const Fixed16 &value)
	: _value(value._value)
{
}


Fixed16 Fixed16::saturatingAdd(const Fixed16 &other) const
{
	return Fixed16(fixedSaturatingAdd(_value, other._value));
}


Fixed16& Fixed16::operator-=(const Fixed16 &other)
{
	_value -= other._value;
	return *this;
}


Fixed16 Fixed16::operator-(const Fixed16 &other) const
{
	return Fixed16(_value - other._value);
}


Fixed16 Fixed16::saturatingSubtract(const Fixed16 &other) const
{
	return Fixed16(fixedSaturatingSubtract(_value, other._value));
}


Fixed16& Fixed16::operator*=(const Fixed16 &other)
{
	_value = fixedMultiply(_value, other._value);
	return *this;
}


Fixed16 Fixed16::operator*(const Fixed16 &other) const
{
	return Fixed16(fixedMultiply(_value, other._value));
}


Fixed16 Fixed16::saturatingMultiply(const Fixed16 &other) const
{
	return Fixed16(fixedSaturatingMultiply(_value, other._value));
}


Fixed16& Fixed16::operator/=(const Fixed16 &other)
{
	_value = fixedDivide(_value, other._value);
	return *this;
}


Fixed16 Fixed16::operator/(const Fixed16 &other) const
{
	return Fixed16(fixedDivide(_value, other._value));
}


Fixed16 Fixed16::saturatingDivide(const Fixed16 &other) const
{
	return Fixed16(fixedSaturatingDivide(_value, other._value));
}


bool Fixed16::operator>=(const Fixed16 &other) const
{
	return (_value >= other._value);
}


bool Fixed16::operator>(const Fixed16 &other) const
{
	return (_value > other._value);
}


float Fixed16::toFloat() const
{
	return static_cast<float>(_value)/static_cast<float>(cOneValue);
}


bool Fixed16::operator<=(const Fixed16 &other) const
{
	return (_value <= other._value);
}


bool Fixed16::operator<(const Fixed16 &other) const
{
	return (_value < other._value);
}


bool Fixed16::operator!=(const Fixed16 &other) const
{
	return (_value != other._value);
}


bool Fixed16::operator==(const Fixed16 &other) const
{
	return (_value == other._value);
}


Fixed16& Fixed16::operator+=(const Fixed16 &other)
{
	_value += other._value;
	return *this;
}


Fixed16 Fixed16::operator+(const Fixed16 &other) const
{
	return Fixed16(_value + other._value);
}


Fixed16& Fixed16::operator=(const Fixed16 &other)
{
	_value = other._value;
	return *this;
}


