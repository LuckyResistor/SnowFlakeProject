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
#pragma once


#include "SceneData.hpp"


/// This helper class contains the calculations for a LED map with a random rotation.
///
/// The template parameter `rotationSceneDataIndex` contains the index in the int8 array
/// of the scene data where the current rotation for the scene is stored.
///
template<uint8_t rotationSceneDataIndex>
class ShiftingMap
{
	public:
	/// Create a new shifting map instance.
	///
	/// @param ledMap The pointer to the ledMap to use.
	/// @param range The range for the displayed values on the map.
	/// @param rotationSceneDataIndex The index in the scene data int8 array where to store the rotation value.
	///
	constexpr ShiftingMap(const Fixed16* const ledMap, const Fixed16 range)
	: _ledMap(ledMap), _range(range)
	{
	}

public:
	/// Initialize the scene data for the shifting map.
	///
	void initialize(SceneData *sceneData, uint8_t rotation) const {
		rotation &= 0x7;
		if (rotation > 5) {
			rotation -= 6;
		}
		sceneData->int8[rotationSceneDataIndex] = rotation;
	}

	/// Get the wrapped position for a given frame.
	///
	PixelValue getPositionWrapped(SceneData *sceneData, uint8_t pixelIndex, FrameIndex frameCount, FrameIndex frameIndex) const
	{
		const auto normal = PixelValue::normalFromRange<FrameIndex>(0, frameCount, frameIndex);
		const auto rotation = sceneData->int8[rotationSceneDataIndex];
		const auto pixelIndexAfterRotation = LedMaps::cIndexRotation[rotation][pixelIndex];
		const auto mapShift = (_ledMap[pixelIndexAfterRotation] * _range);
		const auto result = PixelValue(normal - mapShift);
		return result.wrapped();
	}
	
	private:
	const Fixed16* const _ledMap;
	const Fixed16 _range;
};

