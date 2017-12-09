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
#include "Configuration.hpp"
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
	Scene::SkyWithStars,
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
const uint32_t cSceneDuration = 30000;


/// The currently displayed scene
///
uint8_t gCurrentSceneIndex = 0;

/// The time of the last scene switch.
///
uint32_t gLastSceneBlend = 0;


/// Display an error state with the LEDs of the board.
///
__attribute__((noreturn))
void displayError(uint8_t errorCode)
{
	while (true) {
		Display::setLedLevel(errorCode, Display::cMaximumLevel);
		Display::synchronizeAndShow();
		Helper::delayMs(500);	
		Display::setLedLevel(errorCode, 0);
		Display::synchronizeAndShow();
		Helper::delayMs(500);
	}
}


/// Run a communication test.
///
/// This will send a new value every two seconds and display it as binary 
/// pattern on the snow flake.
///
void communicationTestMaster()
{
	Helper::delayMs(2000);
	uint32_t value = 0;
	while (true) {
		for (uint8_t i = 0; i < Display::cLedCount; ++i) {
			const bool isBitSet = ((value&(1UL<<i)) != 0);
			Display::setLedLevel(i, (isBitSet ? Display::cMaximumLevel : 0));
		}
		Display::synchronizeAndShow();
		Communication::sendData(value);
		value += 1;
		Helper::delayMs(2000);
	}
}


/// Display the identifier of the board.
///
void displayBoardIdentifier()
{
	Display::setAllLedLevels(0);
	Display::setLedLevel(Communication::getIdentifier(), Display::cMaximumLevel);
	for (uint8_t i = 0; i < 10; ++i) {
		Display::setLedLevel(Display::cLedCount-1, Display::cMaximumLevel);
		Display::synchronizeAndShow();
		Helper::delayMs(200);
		Display::setLedLevel(Display::cLedCount-1, 0);
		Display::synchronizeAndShow();
		Helper::delayMs(200);
	}
}



void initialize()
{
	// Initialize all modules.
	Hardware::initialize();
	Helper::initialize();
	Display::initialize();
	Communication::initialize();
	SceneManager::initialize();
	Player::initialize();

	// Wait for the negotiation in the communication between the elements.
	if (!Communication::waitForNegotiation()) {
		// If there was any error, go into error mode.
		displayError(static_cast<uint8_t>(Communication::getError()));
	}

	if (cTraceShowIdentifierOnStart) {
		displayBoardIdentifier();
	}	
	
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
