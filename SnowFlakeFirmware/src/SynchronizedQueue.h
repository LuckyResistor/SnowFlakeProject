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
// with this program; if not, write to the Free Software Foundati
//
#pragma once


#include "Chip.hpp"

#include <cstdint>


/// Description of InterruptSafeQueue
///
template<typename ElementType, int8_t queueSize>
class SynchronizedQueue
{
public:
	/// Create a new synchronized queue
	///
	SynchronizedQueue() : _readPosition(0), _writePosition(0) {}	
	
public:
	/// Increment by one in the queue size range.
	///
	inline uint8_t incrementPosition(uint8_t position) {
		position += 1;
		if (position == queueSize) {
			position = 0;
		}
		return position;
	}

	/// Get an element from the queue.
	///
	/// @param element A reference to a variable where to store the element.
	/// @return `true` if an element was retrieved, or `false` if there ware no element in the queue.
	///
	bool getElement(ElementType &element) {
		__DSB();
		const auto writePosition = _writePosition;
		const auto readPosition = _readPosition;
		if (readPosition == writePosition) {
			return false;
		}
		element = _elements[readPosition];
		__DSB();
		_readPosition = incrementPosition(readPosition);
		return true;
	}

	/// Put an element into the queue.
	///
	/// @param element The element to put into the queue.
	/// @return `true` on success, `false` if the queue is full.
	///
	bool putElement(const ElementType &element) {
		__DSB();
		const auto readPosition = _readPosition;
		const auto writePosition = _writePosition;
		const auto nextWritePosition = incrementPosition(writePosition);
		if (readPosition == nextWritePosition) {
			return false;
		}
		_elements[writePosition] = element;
		_writePosition = nextWritePosition;
		return true;
	}

private:
	ElementType _elements[queueSize]; ///< The queued elements.
	volatile uint8_t _readPosition; ///< The current read position.
	volatile uint8_t _writePosition; ///< The current write position.
};
