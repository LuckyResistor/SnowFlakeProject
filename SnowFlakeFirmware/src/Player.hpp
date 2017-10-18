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


#include "Scene.hpp"


/// The player module animates the scenes and blends between scenes.
///
namespace Player {


/// The state of the player.
///
enum class State : uint8_t {
	SingleScene, ///< A single scene is played.
	Blend, ///< The player blends from scene A to scene B.
};


/// Initialize the player module.
///
/// By default, this will play the scene `Black`.
///
void initialize();

/// Animate one frame and wait for synchronization.
///
/// Call this method in a loop to animate the scene. To time the
/// length of a scene, use the system tick.
///
void animate();

/// Display a scene immediately.
///
/// This will immediately switch to the given scene.
///
/// @param sceneName The name of the new scene to display.
///
void displayScene(Scene::Name sceneName);

/// Blend to another scene smoothly.
///
/// @param sceneName The name of the new scene to display.
/// @param durationFrames The duration of the blend in frames. Has to be > 0.
///
void blendToScene(Scene::Name sceneName, uint32_t durationFrames);

/// Get the current state of the player.
///
/// You can use this function to check if a blend has finished.
///
State getState();


}

