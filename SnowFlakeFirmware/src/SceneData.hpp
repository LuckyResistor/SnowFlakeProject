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

#include <cstdint>


/// Private data area for a scene.
///
class SceneData
{
public:
	/// The size of the data array.
	///
	static const uint8_t cSize = 48;
	
public:
	/// Create a zero scene data structure.
	///
	SceneData();
	
public:
	/// Clear existing scene data.
	///
	void clear();
	
public:	
	union {
		Fixed16 fixed16[cSize]; ///< An array of fixed point data.
		uint32_t int32[cSize]; ///< Or alternatively store 32bit integer data.
		uint16_t int16[cSize*2]; ///< Or alternatively store 16bit integer data.
		uint8_t int8[cSize*4]; ///< Or alternatively store 8bit integer data.
	};
};




