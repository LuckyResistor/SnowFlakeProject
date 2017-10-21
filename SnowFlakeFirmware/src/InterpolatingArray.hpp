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


/// This is a helper which implements interpolating an array (vector) of values.
///
template<uint8_t elementCount>
class InterpolatingArray
{
public:
	/// Create a new interpolating array using the given values.
	///
	constexpr InterpolatingArray(const Fixed16* const values) 
		: _values(values)
	{			
	}

public:
	/// Get the value at the given position.
	///
	Fixed16 getValueAt(Fixed16 position) const 
	{
		const auto preciseElementIndex = (position * Fixed16(static_cast<int16_t>(elementCount)));
		const uint8_t firstElementIndex = static_cast<uint8_t>(preciseElementIndex.toRawInteger());
		const auto fraction = preciseElementIndex.getFraction();
		if (fraction.isZero()) {
			return _values[firstElementIndex];
		} else {
			const uint8_t secondElementIndex = ((firstElementIndex == (elementCount-1)) ? 0 : (firstElementIndex + 1));
			return _values[firstElementIndex]*(Fixed16(1.0f)-fraction) + _values[secondElementIndex]*fraction;
		}
	}

private:
	const Fixed16* const _values;
};
