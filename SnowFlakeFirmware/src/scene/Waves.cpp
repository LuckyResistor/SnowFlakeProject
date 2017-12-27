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
#include "Waves.hpp"


#include "../InterpolatingArray.hpp"
#include "../LedMaps.hpp"
#include "../ShiftingMap.hpp"


namespace scene {
namespace Waves {


/// The bars array with the ramp for the effect.
///
const Fixed16 cWave[] = {
	Fixed16(1.0f), Fixed16(0.3f), Fixed16(0.0f), Fixed16(0.3f)
};

/// The number of elements in the bars array.
///
const uint8_t cWaveCount = sizeof(cWave)/sizeof(Fixed16);
	
/// The interpolating array.
///
const InterpolatingArray<cWaveCount> cWaveInterpolation(cWave);

/// The shifting map with a simple diagonal pattern.
///
const ShiftingMap<0> cDiagonalShiftingMap(LedMaps::cDiagonal, Fixed16(1.2f));


void initialize(SceneData *sceneData, uint8_t entropy)
{
	cDiagonalShiftingMap.initialize(sceneData, (entropy/43));
}


Frame getFrame(SceneData *sceneData, FrameIndex frameIndex)
{
	return Frame([=](uint8_t pixelIndex)->PixelValue{
		return cWaveInterpolation.getSmoothValueAt(cDiagonalShiftingMap.getPositionWrapped(sceneData, pixelIndex, cFrameCount, frameIndex));
	});
}


}
}
