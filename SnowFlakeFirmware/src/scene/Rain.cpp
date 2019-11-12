//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2019 by Lucky Resistor. See LICENSE for details.
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
#include "Rain.hpp"


#include "../LedMaps.hpp"
#include "../PixelCanvas.hpp"
#include "../PixelParticleMap.hpp"
#include "../Communication.hpp"

#include <random>


namespace scene {
namespace Rain {


/// The map with all particles to use for the display.
///
PixelParticleMap<32, 0> gPixelParticleMap;

/// The offset to access the render offset value.
///
static constexpr uint8_t cRenderYOffsetIndex = gPixelParticleMap.cNextOffset;

/// The distance between the platforms in pixels.
///
static constexpr uint8_t cPlatformDistance = 10;

/// The length of the trail.
///
static constexpr uint8_t cParticleTrailLength = 8;

/// The minimum speed.
///
static constexpr uint8_t cMinimumSpeed = 0x10;

/// The maximum speed.
///
static constexpr uint8_t cMaximumSpeed = 0x40;

/// The total virtual canvas height
///
static constexpr uint16_t cVirtualCanvasHeight = 0x4000;

/// The trail levels.
///
const PixelValue cParticleTrailLevels[cParticleTrailLength+1] = {
	PixelValue(0.0f),
	PixelValue(1.0f),
	PixelValue(0.7f),
	PixelValue(0.5f),
	PixelValue(0.4f),
	PixelValue(0.3f),
	PixelValue(0.2f),
	PixelValue(0.1f),
	PixelValue(0.05f)
};


/// The pixel canvas to use for this effect.
///
#define gCanvas gPixelCanvas9x9


void initialize(SceneData *sceneData, uint8_t entropy)
{
	// Pseudo random using seed.
	std::ranlux24_base generator;
	generator.seed(entropy);
	
	// Prepare all particles.
	std::uniform_int_distribution<uint8_t> randomX(0, gCanvas.cWidth-1);
	std::uniform_int_distribution<uint16_t> randomY(0, cVirtualCanvasHeight-1);
	std::uniform_int_distribution<uint8_t> randomSpeed(0x10, 0x40);
	for (uint8_t i = 0; i < gPixelParticleMap.cParticleCount; ++i) {
		auto particle = gPixelParticleMap.accessParticle(sceneData, i);
		particle->x = randomX(generator);
		particle->y = randomY(generator);
		particle->ySpeed = randomSpeed(generator);
	}
	
	// Set the offset to render the canvas.
	sceneData->int8[cRenderYOffsetIndex] = (Communication::getIdentifier() + 1)	* cPlatformDistance;
}


Frame getFrame(SceneData *sceneData, FrameIndex)
{
	// Clear the canvas.
	gCanvas.clearCanvas();
	// Animate and render all particles to the canvas.
	for (uint8_t i = 0; i < gPixelParticleMap.cParticleCount; ++i) {
		// Animate
		auto particle = gPixelParticleMap.accessParticle(sceneData, i);
		particle->y += particle->ySpeed;
		if (particle->y >= cVirtualCanvasHeight) {
			particle->y -= cVirtualCanvasHeight;
		}
		// First, check if the particle is on the canvas.
		const auto y1 = sceneData->int8[cRenderYOffsetIndex];
		const auto y2 = y1 + gCanvas.cHeight;
		const auto x = particle->x;
		const auto yI = static_cast<uint8_t>(particle->y >> 8);
		const auto yF = PixelValue(static_cast<int32_t>(particle->y & 0x00ff) << 8);
		const auto yMax = yI;
		const auto yMin = yMax - cParticleTrailLength + 1;
		const auto f1 = yF.inverted();
		const auto f2 = yF;
		// Just check if the start or end is in the canvas (faster).
		if ((yMin >= y1 && yMin < y2) || (yMax >= y1 && yMax < y2)) {
			const auto speedFactor = PixelValue(static_cast<int16_t>(particle->ySpeed - cMinimumSpeed)) / PixelValue(static_cast<int16_t>(cMaximumSpeed-cMinimumSpeed));
			const auto levelFactor = speedFactor / PixelValue(2.0f) + PixelValue(0.5f);
			for (uint8_t i = 0; i < cParticleTrailLength; ++i) {
				auto p = PixelPosition(x, (yMax-i-1)-y1);
				if (p.y < gCanvas.cHeight) {
					const auto l1 = cParticleTrailLevels[i];
					const auto l2 = cParticleTrailLevels[i+1];
					const auto value = (l1 * f1 + l2 * f2) * levelFactor;
					if (gCanvas.getPixel(p) < value) {
						gCanvas.setPixel(p, value);						
					}
				}
			}
		}
	}
	// Render the canvas into a frame.
	return gCanvas.getFrame(LedMaps::cCanvas9x9);
}


}
}
