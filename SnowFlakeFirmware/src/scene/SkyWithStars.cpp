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
#include "SkyWithStars.hpp"


#include "../PixelValue.hpp"
#include "../LedMaps.hpp"
#include "../InterpolatingArray.hpp"
#include "../RandomFrameCounters.hpp"
#include "../ValueArrays.hpp"


namespace scene {
namespace SkyWithStars {


/// The star brightness array
///
const Fixed16 cStarBrightness[] = {
	Fixed16(0.5f), Fixed16(0.8f), Fixed16(0.8f), Fixed16(0.8f), Fixed16(0.8f), Fixed16(0.5f), Fixed16(0.3f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.3f), Fixed16(0.3f), Fixed16(0.0f), Fixed16(0.2f), Fixed16(0.2f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.3f), Fixed16(0.3f), Fixed16(0.3f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.2f), Fixed16(0.2f),
	Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.4f), Fixed16(0.4f),
	Fixed16(0.4f), Fixed16(0.0f), Fixed16(0.3f), Fixed16(0.5f), Fixed16(0.5f), Fixed16(0.5f), Fixed16(0.3f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.2f), Fixed16(0.2f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.3f), Fixed16(0.3f), Fixed16(0.3f), Fixed16(0.0f), Fixed16(0.0f),
	Fixed16(0.2f), Fixed16(0.2f), Fixed16(0.0f), Fixed16(0.0f), Fixed16(0.5f), Fixed16(0.5f), Fixed16(0.0f), Fixed16(0.3f),
};

/// The number of elements in the star brightness array.
///
const uint16_t cStarBrightnessCount = sizeof(cStarBrightness)/sizeof(Fixed16);

/// The interpolating array for the star brightness animation
///
const InterpolatingArray<cStarBrightnessCount> cStarBrightnessInterpolation(cStarBrightness);

/// Create a random frame counter for a star animation
///
const RandomFrameCounters<1500, 2500, 0> gStarFrameCounters;

/// The number of elements in the sparkle values array
///
const uint16_t cSparkleValuesCount = sizeof(ValueArrays::cRandom)/sizeof(Fixed16);

/// The interpolating array for the sparkle values.
///
const InterpolatingArray<cSparkleValuesCount> cSparkleValuesInterpolation(ValueArrays::cRandom);

/// Create a random frame counter for a the sparkle
///
const RandomFrameCounters<1600, 2400, Display::cLedCount> gSparkleFrameCounters;


void initialize(SceneData *data)
{
	gStarFrameCounters.initialize(data);
}


Frame getFrame(SceneData *data, FrameIndex)
{
	// Create the frame with the stars
	auto resultFrame = gStarFrameCounters.getFrame(data, [](Fixed16 x)->PixelValue{
		return cStarBrightnessInterpolation.getSmoothValueAt(x);
	});
	// Create the sparkle values frame
	auto sparkleValueFrame = gSparkleFrameCounters.getFrame(data, [](Fixed16 x)->PixelValue{
		// Create a +/- 0.1 value from the random values.
		return cSparkleValuesInterpolation.getHardValueAt(x) * Fixed16(0.1f);
	});
	// Add the sparkle to the sky
	resultFrame.subtractWithLimit(sparkleValueFrame);
	// The final result.
	return resultFrame;
}


}
}
