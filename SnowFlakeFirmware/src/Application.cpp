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
#include "Application.h"


#include "Display.h"
#include "Hardware.h"
#include "Helper.h"

#include "sam.h"


namespace Application {
	

void initialize()
{
	Hardware::initialize();
	Helper::initialize();
	Display::initialize();
}


void loop()
{
	while (true) {
		for (uint8_t i = 0; i < 0x80; ++i) {
			for (uint8_t j = 0; j < Display::cLedCount; ++j) {
				uint8_t level = (i + (5*j)) & 0x7f;
				if (level >= 0x40) {
					level = 0x80 - level;
				}
				Display::setLedLevel(j, level);
			}
			Display::show();
			Helper::delayNop(500000);			
		}
	}
}


}
