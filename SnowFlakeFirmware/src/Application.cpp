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
#include "Application.hpp"


#include "Communication.hpp"
#include "Display.hpp"
#include "Hardware.hpp"
#include "Helper.hpp"
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Player.hpp"

#include "Chip.hpp"


namespace Application {
	
	
/// The scenes to display.
///
const Scene::Name cScenesOnDisplay[] = {
	Scene::IceSparkle,
	Scene::SimpleRandomFlicker,
	Scene::SimpleRandomParticle,
	Scene::SimpleRotation,
	Scene::SimpleDiagonal,
	Scene::SimpleShift,
	Scene::SimpleFlash,
};

/// The number of scenes to display.
///
const uint8_t cScenesOnDisplayCount = sizeof(cScenesOnDisplay)/sizeof(Scene::Name);

/// Duration of a scene blend in frames.
///
const uint32_t cBlendDuration = 80;

/// Duration how long a scene is shown in milliseconds.
///
const uint32_t cSceneDuration = 20000;
	

/// The currently displayed scene
///
uint8_t gCurrentSceneIndex = 0;

/// The time of the last scene switch.
///
uint32_t gLastSceneBlend = 0;


void initialize()
{
	// Initialize all modules.
	Hardware::initialize();
	Helper::initialize();
	Display::initialize();
	Communication::initialize();
	SceneManager::initialize();
	Player::initialize();

	// Blend to the first scene from black.
	Player::displayScene(Scene::Black);
	gCurrentSceneIndex = Helper::getRandom8(0, cScenesOnDisplayCount-1);
	Player::blendToScene(cScenesOnDisplay[gCurrentSceneIndex], cBlendDuration);
	while (Player::getState() == Player::State::Blend) {
		Player::animate();
	}
	
	// Get the current time as initial scene blend time.
	gLastSceneBlend = Helper::getSystemTimeMs();
}


void loop()
{
	while (true) {
		// Animate the current scene.
		Player::animate();
		// Check if it's time to blend to the next scene.
		const uint32_t systemTime = Helper::getSystemTimeMs();
		if ((systemTime - gLastSceneBlend) >= cSceneDuration && cScenesOnDisplayCount > 1) {
			auto nextScene = Helper::getRandom8(0, cScenesOnDisplayCount-1);
			while (nextScene == gCurrentSceneIndex) {
				nextScene = Helper::getRandom8(0, cScenesOnDisplayCount-1);
			}
			gCurrentSceneIndex = nextScene;
			Player::blendToScene(cScenesOnDisplay[gCurrentSceneIndex], cBlendDuration);
			gLastSceneBlend = systemTime;	
		}
	}
}


}
