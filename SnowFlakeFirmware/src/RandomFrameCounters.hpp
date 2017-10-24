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


#include "Display.hpp"
#include "Frame.hpp"
#include "Helper.hpp"
#include "SceneData.hpp"

#include <functional>


/// This is a helper class which implements random frame counters for all LEDs in the array.
///
/// It is a template to eliminate any memory usage. The compiler will just create 
/// custom code in your scene with the given template parameter.
///
/// The frame counter will use the first data elements in the scene data,
/// two 16bit integer for each LED in the display.
///
template<uint16_t minimumFrames, uint16_t maximumFrames>
class RandomFrameCounters
{
public:
	/// Initialize the scene data.
	///
	/// @param data The scene data.
	///
	void initialize(SceneData *data) const
	{
		// Initialize all the frame counter values.
		for (uint8_t index = 0; index < Display::cLedCount; ++index) {
			// Get a random value for the maximum frame count.
			const uint16_t frameCount = Helper::getRandom16(minimumFrames, maximumFrames);
			// Get a random value for the current frame.
			const uint16_t startFrame = Helper::getRandom16(0, frameCount-1);
			// Assign the new random values.
			const uint8_t dataIndex = (index*2);
			data->int16[dataIndex] = frameCount;
			data->int16[dataIndex+1] = startFrame;
		}
	}
	
	/// Calculate a single frame.
	///
	/// @param data The scene data.
	/// @param pixelFn A function to calculate a single pixel in the scene. This function is
	///   called once for each pixel in the frame with "x" as argument. "x" is a normal
	///   value between 0.0 and 1.0. 
	///
	Frame getFrame(SceneData *data, std::function<PixelValue(Fixed16)> pixelFn) const
	{
		Frame frame;
		for (uint8_t index = 0; index < Frame::cSize; ++index) {
			// Get the current pixel value.
			const uint8_t dataIndex = (index*2);
			const uint16_t frameCount = data->int16[dataIndex];
			const uint16_t currentFrame = data->int16[dataIndex+1];
			const Fixed16 position = PixelValue::normalFromRange<uint16_t>(0, frameCount, currentFrame);
			frame.pixelValue[index] = pixelFn(position);
			// Increase the frame counter.
			if (++(data->int16[dataIndex+1]) >= frameCount) {
				data->int16[dataIndex+1] = 0;
				// Get a new random value for the maximum frame count.
				const uint16_t frameCount = Helper::getRandom16(minimumFrames, maximumFrames);
				data->int16[dataIndex] = frameCount;
			}
		}
		return frame;
	}
};
