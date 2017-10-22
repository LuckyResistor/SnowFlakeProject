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


#include "Frame.hpp"
#include "SceneData.hpp"


/// The base class for a single scene for the snow flake.
///
/// A scene is a single effect which is displayed on the LEDs of the snow flake.
///
class Scene
{
public:
	/// An enumeration to name all defined scenes in this firmware.
	///
	enum Name : uint8_t {
		Black,
		IceSparkle,
		SimpleShift,
		SimpleRotation,
		SimpleFlash,
		SimpleRandomParticle
	};
	
public:
	/// Function pointer to the initialization function.
	///
	typedef void(*InitFn)(SceneData*);
	
	/// Function pointer to the `getFrame` function.
	///
	typedef Frame(*GetFrameFn)(SceneData*, FrameIndex frameIndex);	

public:
	/// Create a empty scene.
	///
	/// This is a non functional scene, used as placeholder.
	///
	Scene();
	
	/// Create a new scene.
	///
	/// @param frameCount The frame count for this scene. Has to be >0.
	/// @param initSceneFn The function to initialize the scene data.
	/// @param getFrameFn The function to get a new frame for the scene.
	///
	Scene(uint32_t frameCount, InitFn initFn, GetFrameFn getFrameFn);
	
	/// Copy constructor
	///
	Scene(const Scene &copy);
	
	/// Assign operator.
	///
	const Scene& operator=(const Scene &assign);
	
public:
	/// Get the frame count.
	///
	inline uint32_t getFrameCount() const { return _frameCount; }

	/// Initialize the given scene data.
	///
	void init(SceneData *data);
	
	/// Get a frame for this scene.
	///
	Frame getFrame(SceneData *data, FrameIndex frameIndex);
	
private:
	uint32_t _frameCount; ///< The frame count for this scene.
	InitFn _initFn; ///< The function to initialize the scene data.
	GetFrameFn _getFrameFn; ///< The function to get a new frame for this scene.
};



