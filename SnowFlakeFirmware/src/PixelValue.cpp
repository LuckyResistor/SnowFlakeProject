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
#include "PixelValue.hpp"


PixelValue::PixelValue(const PixelValue &copy)
	: Fixed16(copy)
{
}


PixelValue::PixelValue(Fixed16 value)
	: Fixed16(value)
{
}


PixelValue& PixelValue::operator=(const Fixed16 &other)
{
	_value = other.toRawValue();
	return *this;
}


PixelValue& PixelValue::operator=(const PixelValue &other)
{
	_value = other.toRawValue();
	return *this;
}


constexpr PixelValue PixelValue::maximum()
{
	return PixelValue(1.0f);
}


constexpr PixelValue PixelValue::minimum()
{
	return PixelValue(0.0f);
}


constexpr PixelValue PixelValue::middle()
{
	return PixelValue(0.5f);
}


PixelValue PixelValue::inverted() const
{
	return maximum()-(*this);
}


PixelValue PixelValue::limited() const
{
	if ((*this) < minimum()) {
		return minimum();
	} else if ((*this) > maximum()) {
		return maximum();
	} else {
		return *this;
	}
}


PixelValue PixelValue::wrapped() const
{
	return getFraction();
}


uint8_t PixelValue::convertToRange64() const
{
	return static_cast<uint8_t>(toRawValue() >> 10);
}


PixelValue PixelValue::operator+(const PixelValue &other) const
{
	return PixelValue(Fixed16::operator+(other));
}


PixelValue PixelValue::operator-(const PixelValue &other) const
{
	return PixelValue(Fixed16::operator-(other));
}


PixelValue PixelValue::operator*(const PixelValue &other) const
{
	return PixelValue(Fixed16::operator*(other));
}


PixelValue PixelValue::operator/(const PixelValue &other) const
{
	return PixelValue(Fixed16::operator/(other));
}


bool PixelValue::operator>=(const PixelValue &other) const
{
	return Fixed16::operator>=(other);
}


bool PixelValue::operator>(const PixelValue &other) const
{
	return Fixed16::operator>(other);
}


bool PixelValue::operator<=(const PixelValue &other) const
{
	return Fixed16::operator<=(other);
}


bool PixelValue::operator<(const PixelValue &other) const
{
	return Fixed16::operator<(other);
}


bool PixelValue::operator!=(const PixelValue &other) const
{
	return Fixed16::operator!=(other);
}


bool PixelValue::operator==(const PixelValue &other) const
{
	return Fixed16::operator==(other);
}


PixelValue& PixelValue::operator/=(const PixelValue &other)
{
	Fixed16::operator/=(other);
	return *this;
}


PixelValue& PixelValue::operator*=(const PixelValue &other)
{
	Fixed16::operator*=(other);
	return *this;
}


PixelValue& PixelValue::operator-=(const PixelValue &other)
{
	Fixed16::operator*=(other);
	return *this;
}


PixelValue& PixelValue::operator+=(const PixelValue &other)
{
	Fixed16::operator*=(other);
	return *this;
}


PixelValue PixelValue::normalFromRange(uint32_t firstValue, uint32_t lastValue, uint32_t currentValue)
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


PixelValue PixelValue::normalFromRange(uint8_t firstValue, uint8_t lastValue, uint8_t currentValue)
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


PixelValue PixelValue::bounced() const
{
	auto scaledValue = (*this) * PixelValue(2.0f);
	if (scaledValue < PixelValue::maximum()) {
		return scaledValue;
	} else {
		return PixelValue(2.0f)-scaledValue;
	}
}


