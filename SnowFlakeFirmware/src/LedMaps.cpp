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
#include "LedMaps.hpp"


namespace LedMaps {
	
	
const Fixed16 cClockwise[Display::cLedCount] = {
	Fixed16( 0.0f * (1.0f/12.0f)), // LED 00
	Fixed16( 2.0f * (1.0f/12.0f)), // LED 01
	Fixed16( 4.0f * (1.0f/12.0f)), // LED 02
	Fixed16( 6.0f * (1.0f/12.0f)), // LED 03
	Fixed16( 8.0f * (1.0f/12.0f)), // LED 04
	Fixed16(10.0f * (1.0f/12.0f)), // LED 05
	Fixed16( 0.0f * (1.0f/12.0f)), // LED 06
	Fixed16( 2.0f * (1.0f/12.0f)), // LED 07
	Fixed16( 4.0f * (1.0f/12.0f)), // LED 08
	Fixed16( 6.0f * (1.0f/12.0f)), // LED 09
	Fixed16( 8.0f * (1.0f/12.0f)), // LED 10
	Fixed16(10.0f * (1.0f/12.0f)), // LED 11
	Fixed16( 1.0f * (1.0f/12.0f)), // LED 12
	Fixed16( 3.0f * (1.0f/12.0f)), // LED 13
	Fixed16( 5.0f * (1.0f/12.0f)), // LED 14
	Fixed16( 7.0f * (1.0f/12.0f)), // LED 15
	Fixed16( 9.0f * (1.0f/12.0f)), // LED 16
	Fixed16(11.0f * (1.0f/12.0f)), // LED 17
	Fixed16( 0.0f * (1.0f/12.0f)), // LED 18
};
	

const Fixed16 cDiagonal[Display::cLedCount] = {
	Fixed16( 2.0f * (1.0f/9.0f)), // LED 00
	Fixed16( 6.0f * (1.0f/9.0f)), // LED 01
	Fixed16( 8.0f * (1.0f/9.0f)), // LED 02
	Fixed16( 6.0f * (1.0f/9.0f)), // LED 03
	Fixed16( 2.0f * (1.0f/9.0f)), // LED 04
	Fixed16( 0.0f * (1.0f/9.0f)), // LED 05
	Fixed16( 3.0f * (1.0f/9.0f)), // LED 06
	Fixed16( 5.0f * (1.0f/9.0f)), // LED 07
	Fixed16( 6.0f * (1.0f/9.0f)), // LED 08
	Fixed16( 5.0f * (1.0f/9.0f)), // LED 09
	Fixed16( 3.0f * (1.0f/9.0f)), // LED 10
	Fixed16( 2.0f * (1.0f/9.0f)), // LED 11
	Fixed16( 4.0f * (1.0f/9.0f)), // LED 12
	Fixed16( 5.0f * (1.0f/9.0f)), // LED 13
	Fixed16( 5.0f * (1.0f/9.0f)), // LED 14
	Fixed16( 4.0f * (1.0f/9.0f)), // LED 15
	Fixed16( 3.0f * (1.0f/9.0f)), // LED 16
	Fixed16( 3.0f * (1.0f/9.0f)), // LED 17
	Fixed16( 4.0f * (1.0f/9.0f)), // LED 18	
};


}