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
#include "IceSparkle.hpp"


#include "../PixelValue.hpp"
#include "../LedMaps.hpp"
#include "../InterpolatingArray.hpp"
#include "../RandomFrameCounters.hpp"
#include "../ValueArrays.hpp"
#include "../ShiftingMap.hpp"
#include "../Communication.hpp"
#include "../Configuration.hpp"


namespace scene {
namespace IceSparkle {


/// The ramp to apply the sparkle effect
///
const Fixed16 cSparkleRamp[] = {
	Fixed16(1.0f), Fixed16(1.0f), Fixed16(1.0f), Fixed16(0.8f), Fixed16(0.6f), Fixed16(0.5f), Fixed16(0.4f), Fixed16(0.3f),
	Fixed16(0.3f), Fixed16(0.2f), Fixed16(0.2f), Fixed16(0.1f), Fixed16(0.1f), Fixed16(0.1f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f),
};

/// The number of elements in the sparkle ramp array.
///
const uint16_t cSparkleRampCount = sizeof(cSparkleRamp)/sizeof(Fixed16);

/// The interpolating array for the sparkle ramp.
///
const InterpolatingArray<cSparkleRampCount> cSparkleRampInterpolation(cSparkleRamp);

/// The number of elements in the sparkle values array
///
const uint16_t cSparkleValuesCount = sizeof(ValueArrays::cRandom)/sizeof(Fixed16);

/// The interpolating array for the sparkle values.
///
const InterpolatingArray<cSparkleValuesCount> cSparkleValuesInterpolation(ValueArrays::cRandom);

/// The number of random elements used for the base animation.
///
const uint8_t cBaseAnimationElementCount = 32;

/// The interpolating array for the base animation
///
const InterpolatingArray<cBaseAnimationElementCount> cBaseAnimationInterpolation(ValueArrays::cRandom+cBaseAnimationElementCount);

/// The shifting map for the sparkle.
///
const ShiftingMap<0> gDiagonalShiftingMap(LedMaps::cDiagonal);

/// Create a random frame counter for a smooth base animation
///
const RandomFrameCounters<400, 600, 2> gBaseFrameCounters;

/// Create a random frame counter for a the sparkle
///
const RandomFrameCounters<40, 512, Display::cLedCount+2> gSparkleFrameCounters;

/// The data offset for the position shift in the scene data.
///
const uint16_t cPositionShiftDataOffset = 40;




void initialize(SceneData *data, uint8_t entropy)
{
	// Initialize the frame counters.
	gBaseFrameCounters.initialize(data);
	// Initialize the shifting map with a random direction from the given entropy value.
	gDiagonalShiftingMap.initialize(data, (entropy/43), Fixed16(0.1f));
	// Initialize the shift for the sparkle ramp.
	const auto position = Fixed16(static_cast<int16_t>(Communication::getIdentifier()));
	const auto maxPosition = Fixed16(static_cast<int16_t>(cConfigurationStrandElementCount));
	const auto positionShift = Fixed16(0.2f);
	if ((entropy & 1) == 0) {
		data->fixed16[cPositionShiftDataOffset] = (position / maxPosition * positionShift);		
	} else {
		data->fixed16[cPositionShiftDataOffset] = positionShift - (position / maxPosition * positionShift);		
	}
}


inline Fixed16 getPositionShift(SceneData *data)
{
	return data->fixed16[cPositionShiftDataOffset];
}


Frame getFrame(SceneData *data, FrameIndex frameIndex)
{
	// Create the base frame
	auto resultFrame = gBaseFrameCounters.getFrame(data, [](Fixed16 x)->PixelValue{
		return cBaseAnimationInterpolation.getSmoothValueAt(x) * Fixed16(0.3f) + Fixed16(0.3f);
	});
	// Create the sparkle values frame
	auto sparkleValueFrame = gSparkleFrameCounters.getFrame(data, [](Fixed16 x)->PixelValue{
		// Create a +/- 0.4 value from the random values.
		return cSparkleValuesInterpolation.getHardValueAt(x) * Fixed16(0.8f) - Fixed16(0.4f);
	});
	// Create a frame with the sparkle ramp and multiple the value frame with it.
	sparkleValueFrame.multipleWith(Frame([=](uint8_t pixelIndex)->PixelValue{
		auto shift = gDiagonalShiftingMap.getPositionWrapped(data, pixelIndex, cFrameCount, frameIndex);
		shift = PixelValue(shift + getPositionShift(data)).wrapped();
		return cSparkleRampInterpolation.getSmoothValueAt(shift);
	}));
	// Add the sparkle to the base animation with limits.
	resultFrame.addWithLimit(sparkleValueFrame);
	// The final result.
	return resultFrame;
}


}
}
