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
#include "Player.hpp"


#include "Configuration.hpp"
#include "Display.hpp"
#include "Hardware.hpp"
#include "SceneManager.hpp"

#include <cstring>


namespace Player {


/// A structure which combined the scene data with the scene itself.
///
struct Slot {
	uint32_t frame; ///< The current frame for this slot.
	SceneData data; ///< The scene data.
	Scene scene; ///< The scene itself.
};

/// Slot A
///
Slot gSlotA;

/// Slot B
///
Slot gSlotB;

/// The current state of the player.
///
State gState = State::SingleScene;

/// The frame counter for blending scenes.
///
uint32_t gBlendCurrentFrame = 0;

/// The number of frames used for the blend.
///
uint32_t gBlendLastFrame = 0;


void initialize()
{
	gSlotA.frame = 0;
	gSlotA.scene = SceneManager::getScene(Scene::Black);
	gSlotB.frame = 0;
}


Frame getNextFrame()
{
	// Check the current animation mode.
	if (gState == State::SingleScene) {
		// Just display the scene from slot A
		auto frame = gSlotA.scene.getFrame(&(gSlotA.data), gSlotA.frame);
		if (++gSlotA.frame >= gSlotA.scene.getFrameCount()) {
			gSlotA.frame = 0;
		}
		return frame;
	} else {
		// Blend scene from slot A into scene from slot B
		auto frameA = gSlotA.scene.getFrame(&(gSlotA.data), gSlotA.frame);
		auto frameB = gSlotB.scene.getFrame(&(gSlotB.data), gSlotB.frame);
		if (++gSlotA.frame >= gSlotA.scene.getFrameCount()) {
			gSlotA.frame = 0;
		}
		if (++gSlotB.frame >= gSlotB.scene.getFrameCount()) {
			gSlotB.frame = 0;
		}
		const float factor = static_cast<float>(gBlendCurrentFrame)/static_cast<float>(gBlendLastFrame);
		frameA.blendTo(frameB, factor);
		++gBlendCurrentFrame;
		if (gBlendCurrentFrame >= gBlendLastFrame) {
			std::memcpy(&gSlotA, &gSlotB, sizeof(Slot));
			gState = State::SingleScene;
		}
		return frameA;
	}
}


void animate()
{
	// Create a trace output if enabled.
	if (cTraceOutputSource == TraceOutputSource::FrameClaculationTime) {
		Hardware::setTraceOutputA();
	}
	// Get the next frame and write it into the display buffer.
	auto frame = getNextFrame();
	frame.writeToDisplay();
	// Create a trace output if enabled.
	if (cTraceOutputSource == TraceOutputSource::FrameClaculationTime) {
		Hardware::clearTraceOutputA();
	}
	// Wait for sync and display the new frame.
	Display::synchronizeAndShow();
}


void displayScene(Scene::Name sceneName)
{
	gSlotA.frame = 0;
	gSlotA.data.clear();
	gSlotA.scene = SceneManager::getScene(sceneName);
	gState = State::SingleScene;
}


void blendToScene(Scene::Name sceneName, uint32_t durationFrames)
{
	gSlotB.frame = 0;
	gSlotB.data.clear();
	gSlotB.scene = SceneManager::getScene(sceneName);
	gBlendCurrentFrame = 0;
	gBlendLastFrame = durationFrames;
	gState = State::Blend;
}
	
	
Player::State getState()
{
	return gState;
}


}


