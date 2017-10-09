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
#include "TestFlash.hpp"


namespace scene {
namespace SimpleShift {


void initialize(SceneData *data)
{
	// empty
}


Frame getFrame(SceneData *data, FrameIndex frameIndex)
{
	return Frame([=](uint8_t pixelIndex)->PixelMath::Value{
		return PixelMath::bounceValue(PixelMath::wrap(
			PixelMath::normalFromRange<uint32_t>(0, 50, frameIndex) + 
			PixelMath::normalFromRange<uint8_t>(0, Frame::cSize, pixelIndex)
		));
	});
}


}
}
