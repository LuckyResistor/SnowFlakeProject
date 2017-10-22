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
#include "SimpleRandomParticle.hpp"


#include "../InterpolatingArray.hpp"
#include "../RandomFrameCounters.hpp"


namespace scene {
namespace SimpleRandomParticle {


/// Create a random frame counter to show the particles.
///
const RandomFrameCounters<150, 200> gRandomFrameCounters;

/// The animation for a single particle..
///
const Fixed16 cAnimation[] = {
	Fixed16(0.0f), Fixed16(0.1f), Fixed16(0.5f), Fixed16(0.8f), Fixed16(0.9f), Fixed16(1.0f), Fixed16(0.2f), Fixed16(0.1f),
};

/// The interpolating array.
///
const InterpolatingArray<sizeof(cAnimation)/sizeof(Fixed16)> cAnimationInterpolation(cAnimation);


void initialize(SceneData *data)
{
	gRandomFrameCounters.initialize(data);
}


Frame getFrame(SceneData *data, FrameIndex)
{
	return gRandomFrameCounters.getFrame(data, [](Fixed16 x)->PixelValue{
		return cAnimationInterpolation.getValueAt(x);
	});
}


}
}
