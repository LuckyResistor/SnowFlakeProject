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
#include "ElapsedTimer.hpp"
#include "Hardware.hpp"
#include "Helper.hpp"
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Player.hpp"

#include "Chip.hpp"


namespace Application {
	
	
// Forward declarations.
void onDataReceived(uint32_t value);
void onSynchronization();

	
/// The application state
///
enum class State {
	/// The current scene is playing.
	///
	Play,
	
	/// Master sent the next scene index and waiting to send the synchronization.
	///
	SendSynchronization, 
	
	/// Blend the scene.
	///
	/// Master is waiting for the synchronization finished to be sent which
	/// triggers the blend.
	///
	/// Slave received the synchronization and starts the blend.
	///
	BlendScene,
};
	
/// The scenes to display.
///
const Scene::Name cScenesOnDisplay[] = {
	Scene::SkyWithStars,
	Scene::IceSparkle,
	Scene::Waves,
	Scene::Circles,
	
	//Ignore the simple scenes for production release.
	//Scene::SimpleRandomFlicker,
	//Scene::SimpleRandomParticle,
	//Scene::SimpleRotation,
	//Scene::SimpleDiagonal,
	//Scene::SimpleShift,
	//Scene::SimpleFlash,
};

/// The number of scenes to display.
///
const uint8_t cScenesOnDisplayCount = sizeof(cScenesOnDisplay)/sizeof(Scene::Name);

/// Duration of a scene blend in frames.
///
const uint32_t cBlendDuration = 80;

/// Duration how long a scene is shown in milliseconds.
///
const uint32_t cSceneDuration = 60000;


/// The mask for a command.
///
const uint32_t cCmdMask = 0xffff0000ul;

/// The mask for the "next scene" command.
///
const uint32_t cCmdNextScene = 0xa5140000ul;

/// The mask for the scene index portion.
///
const uint32_t cCmdNextScene_SceneMask = 0x000000fful;

/// The mask for the scene entropy portion.
///
const uint32_t cCmdNextScene_EntropyMask = 0x0000ff00ul;


/// The index of the next scene to display.
///
volatile uint8_t gNextSceneIndex = 0;

/// The entropy for the next scene to display.
///
volatile uint8_t gNextSceneEntropy = 0;


/// The state of the application.
///
volatile State gState = State::Play;

/// The elapsed timer to measure the length of a scene.
///
ElapsedTimer gSceneElapsedTime;


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


/// The initialization for a board in master mode.
///
void masterInitialize()
{
	// Decide about the first scene to display.
	gNextSceneIndex = Helper::getRandom8(0, cScenesOnDisplayCount-1);
	gNextSceneEntropy = Helper::getRandom8(0, 0xff);
		
	// Send the scene number to all other boards.
	Helper::delayMs(50);
	Communication::sendData(cCmdNextScene | static_cast<uint32_t>(gNextSceneIndex) | (static_cast<uint32_t>(gNextSceneEntropy) << 8));
	Communication::waitUntilReadyToSend();
	Helper::delayMs(50); // Wait until all boards are ready.
	Communication::sendSynchronization();
			
	// Blend to the first scene from black.
	Player::displayScene(Scene::Black, 0);
	Player::blendToScene(cScenesOnDisplay[gNextSceneIndex], gNextSceneEntropy, cBlendDuration);
	gSceneElapsedTime.start();
	while (Player::getState() == Player::State::Blend) {
		Player::animate();
	}
}


/// The initialization for a board in slave mode.
///
void slaveInitialize()
{
	Player::displayScene(Scene::Black, 0);
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

	// Register the functions to receive data and the synchronization for boards in slave mode.
	if (Communication::getIdentifier() != 0) {
		Communication::registerReadDataFunction(&onDataReceived);
		Communication::registerSynchronisationFunction(&onSynchronization);		
	}

	// If configured display the board identifier first.
	if (cTraceShowIdentifierOnStart) {
		displayBoardIdentifier();
	}	
	
	// Continue with the initialization based on the received identifier
	if (Communication::getIdentifier() == 0) {
		masterInitialize();
	} else {
		slaveInitialize();
	}
}


/// The loop for a board in master mode.
///
__attribute__((noreturn))
void masterLoop()
{
	while (true) {
		// Animate the current scene.
		Player::animate();
		// Check if blending is required/wanted.
		if (cScenesOnDisplayCount > 1 || cTraceBlendOnSingleScene) {
			// Check if its time to think about the next scene
			if (gState == State::Play && gSceneElapsedTime.elapsedTime() > (cSceneDuration - 50)) {
				// Select a random new scene, which is not the currently played one.
				auto nextScene = Helper::getRandom8(0, cScenesOnDisplayCount-1);
				while (nextScene == gNextSceneIndex && cScenesOnDisplayCount != 1) {
					nextScene = Helper::getRandom8(0, cScenesOnDisplayCount-1);
				}
				gNextSceneIndex = nextScene;
				gNextSceneEntropy = Helper::getRandom8(0, 0xff);
				Communication::sendData(cCmdNextScene | static_cast<uint32_t>(gNextSceneIndex) | (static_cast<uint32_t>(gNextSceneEntropy) << 8));
				gState = State::SendSynchronization;
			} else if (gState == State::SendSynchronization && gSceneElapsedTime.elapsedTime() >= cSceneDuration) {
				Communication::sendSynchronization();
				gState = State::BlendScene;
			} else if (gState == State::BlendScene && Communication::isReadyToSend()) {
				Player::blendToScene(cScenesOnDisplay[gNextSceneIndex], gNextSceneEntropy, cBlendDuration);
				gState = State::Play;
				gSceneElapsedTime.start();
			}			
		}
	}	
}


/// The function to receive data from the master.
///
void onDataReceived(uint32_t value)
{
	// Check incoming commands.
	if ((value & cCmdMask) == cCmdNextScene) {
		gNextSceneIndex = static_cast<uint8_t>(value & cCmdNextScene_SceneMask);
		gNextSceneEntropy = static_cast<uint8_t>((value & cCmdNextScene_EntropyMask) >> 8);
	}
}


/// The function to receive the synchronization from the master.
///
void onSynchronization()
{
	gState = State::BlendScene;
}


/// The loop for a board in slave mode.
///
__attribute__((noreturn))
void slaveLoop()
{
	while (true) {
		// Animate the current scene.
		Player::animate();
		// Act on application states
		if (gState == State::BlendScene) {
			Player::blendToScene(cScenesOnDisplay[gNextSceneIndex], gNextSceneEntropy, cBlendDuration);
			gState = State::Play;			
		}
	}
}


void loop()
{
	if (Communication::getIdentifier() == 0) {
		masterLoop();
	} else {
		slaveLoop();
	}
}


}
