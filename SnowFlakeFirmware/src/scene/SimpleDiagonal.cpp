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
#include "SimpleDiagonal.hpp"


#include "../InterpolatingArray.hpp"
#include "../LedMaps.hpp"


namespace scene {
namespace SimpleDiagonal {


/// The bars array with the ramp for the effect.
///
const Fixed16 cBars[] = {
	Fixed16(1.0f), Fixed16(0.2f)
};

/// The number of elements in the bars array.
///
const uint8_t cBarsElementCount = sizeof(cBars)/sizeof(Fixed16);
	
/// The interpolating array.
///
const InterpolatingArray<cBarsElementCount> cBarsInterpolation(cBars);


void initialize(SceneData*, uint8_t)
{
	// empty
}


Frame getFrame(SceneData*, FrameIndex frameIndex)
{
	return Frame([=](uint8_t pixelIndex)->PixelValue{
		const auto position = PixelValue(PixelValue::normalFromRange<uint32_t>(0, cFrameCount, frameIndex) - (LedMaps::cDiagonal[pixelIndex] * Fixed16(1.2f)));
		return cBarsInterpolation.getSmoothValueAt(position.wrapped());
	});
}


}
}
