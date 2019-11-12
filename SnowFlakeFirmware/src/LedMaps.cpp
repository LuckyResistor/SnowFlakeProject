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


const Fixed16 cCircular[Display::cLedCount] = {
	Fixed16( 3.0f * (1.0f/4.0f)), // LED 00
	Fixed16( 3.0f * (1.0f/4.0f)), // LED 01
	Fixed16( 3.0f * (1.0f/4.0f)), // LED 02
	Fixed16( 3.0f * (1.0f/4.0f)), // LED 03
	Fixed16( 3.0f * (1.0f/4.0f)), // LED 04
	Fixed16( 3.0f * (1.0f/4.0f)), // LED 05
	Fixed16( 2.0f * (1.0f/4.0f)), // LED 06
	Fixed16( 2.0f * (1.0f/4.0f)), // LED 07
	Fixed16( 2.0f * (1.0f/4.0f)), // LED 08
	Fixed16( 2.0f * (1.0f/4.0f)), // LED 09
	Fixed16( 2.0f * (1.0f/4.0f)), // LED 10
	Fixed16( 2.0f * (1.0f/4.0f)), // LED 11
	Fixed16( 1.0f * (1.0f/4.0f)), // LED 12
	Fixed16( 1.0f * (1.0f/4.0f)), // LED 13
	Fixed16( 1.0f * (1.0f/4.0f)), // LED 14
	Fixed16( 1.0f * (1.0f/4.0f)), // LED 15
	Fixed16( 1.0f * (1.0f/4.0f)), // LED 16
	Fixed16( 1.0f * (1.0f/4.0f)), // LED 17
	Fixed16( 0.0f * (1.0f/4.0f)), // LED 18
};


const uint8_t cIndexRotation[6][Display::cLedCount] = {
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18},
	{ 5,  0,  1,  2,  3,  4, 11,  6,  7,  8,  9, 10, 17, 12, 13, 14, 15, 16, 18},
	{ 4,  5,  0,  1,  2,  3, 10, 11,  6,  7,  8,  9, 16, 17, 12, 13, 14, 15, 18},
	{ 3,  4,  5,  0,  1,  2,  9, 10, 11,  6,  7,  8, 15, 16, 17, 12, 13, 14, 18},
	{ 2,  3,  4,  5,  0,  1,  8,  9, 10, 11,  6,  7, 14, 15, 16, 17, 12, 13, 18},
	{ 1,  2,  3,  4,  5,  0,  7,  8,  9, 10, 11,  6, 13, 14, 15, 16, 17, 12, 18}
};


const PixelPosition cCanvas9x9[Display::cLedCount] = {
	PixelPosition(4, 0), // LED 00
	PixelPosition(8, 2), // LED 01
	PixelPosition(8, 6), // LED 02
	PixelPosition(4, 8), // LED 03
	PixelPosition(0, 6), // LED 04
	PixelPosition(0, 2), // LED 05
	PixelPosition(4, 2), // LED 06
	PixelPosition(7, 3), // LED 07
	PixelPosition(7, 5), // LED 08
	PixelPosition(4, 6), // LED 09
	PixelPosition(1, 5), // LED 10
	PixelPosition(1, 3), // LED 11
	PixelPosition(5, 3), // LED 12
	PixelPosition(6, 4), // LED 13
	PixelPosition(5, 5), // LED 14
	PixelPosition(3, 5), // LED 15
	PixelPosition(2, 4), // LED 16
	PixelPosition(3, 3), // LED 17
	PixelPosition(4, 4), // LED 18
};


}

