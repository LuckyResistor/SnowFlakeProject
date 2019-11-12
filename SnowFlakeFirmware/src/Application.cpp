//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2017-2019 by Lucky Resistor. See LICENSE for details.
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
void onButtonPress(Communication::ButtonPress buttonPress);


/// The application mode
///
enum class Mode : uint8_t {
	Automatic, ///< Scenes are switched in regular intervals.
	Single, ///< One scene is displayed forever.	
};


/// The configuration mode.
///
enum class ConfigurationMode : uint8_t {
	Mode, ///< Configure the play mode.
	SceneDuration, ///< Configure the scene duration.	
};

	
/// The application state
///
enum class State : uint8_t {
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
	
	/// Request Off.
	///
	/// The used did a long press in on mode, an off state is requested.
	/// This state is only valid for the master mode.
	///
	OffRequest,
		
	/// Off.
	///
	/// All LEDs are disabled. Slaves are waiting for new scenes to wake up.
	///
	Off,
	
	/// Request On.
	///
	/// Turn the platform on if it was turned off.
	///
	OnRequest,
	
	/// Request Configuration.
	///
	/// The user requested configuration mode.
	/// This intermediate step allow to turn off all other flakes while configuring the strand. 
	///
	ConfigurationRequest,
	
	/// Configuration.
	///
	/// The configuration is displayed.
	///
	Configuration,
};
	
/// The scenes for auto mode A
///
const Scene::Name cAutoScenesA[] = {
	Scene::SkyWithStars,
	Scene::IceSparkle,
	Scene::Waves,
	Scene::Circles,		
};

/// The scenes for auto mode B
///
const Scene::Name cAutoScenesB[] = {
	Scene::SkyWithStars,
	Scene::IceSparkle,
	Scene::Waves,
	Scene::Circles,
	Scene::Rain,
	Scene::SimpleRandomFlicker,
	Scene::SimpleRandomParticle,
	Scene::SimpleRotation,
	Scene::SimpleDiagonal,
	Scene::SimpleShift,
	Scene::SimpleFlash,
};


/// The scenes for auto mode C
///
const Scene::Name cAutoScenesC[] = {
	Scene::SimpleRandomFlicker,
	Scene::SimpleRandomParticle,
	Scene::SimpleRotation,
	Scene::SimpleDiagonal,
	Scene::SimpleShift,
	Scene::SimpleFlash,
};


/// The scenes for auto mode D
///
const Scene::Name cAutoScenesD[] = {
	Scene::SkyWithStars,
	Scene::IceSparkle,
	Scene::Waves,
	Scene::Circles,
	Scene::Rain,
};


/// One automatic scene mode.
///
struct AutoScenes {
	const Scene::Name *scenes;
	uint8_t count;
};

/// All different automatic scene modes.
///
const AutoScenes cAutoScenes[] = {
	{cAutoScenesA, sizeof(cAutoScenesA)/sizeof(Scene::Name)},
	{cAutoScenesB, sizeof(cAutoScenesB)/sizeof(Scene::Name)},
	{cAutoScenesC, sizeof(cAutoScenesC)/sizeof(Scene::Name)},
	{cAutoScenesD, sizeof(cAutoScenesD)/sizeof(Scene::Name)},
};
const uint8_t cAutoScenesCount = sizeof(cAutoScenes)/sizeof(AutoScenes);


/// Duration of a scene blend in frames.
///
const uint32_t cBlendDuration = 80;

/// Duration how long a scene is shown in milliseconds.
///
const uint32_t cSceneDurations[] = {15000, 30000, 60000, 120000, 360000, 600000};

/// The number of scene durations.
///
const uint8_t cSceneDurationsCount = sizeof(cSceneDurations)/sizeof(uint32_t);

/// Duration how long the configuration is shown in milliseconds.
///
const uint32_t cConfigurationDuration = 6000; // 6s

/// Configuration LEDs A
///
const uint8_t cConfigurationRowA[] = { 6,  7,  8,  9, 10, 11};

/// Configuration LEDs B
///
const uint8_t cConfigurationRowB[] = {17, 12, 13, 14, 15, 16};


/// The mask for a command.
///
const uint32_t cCmdMask = 0xffff0000ul;

/// The mask for the "next scene" command.
///
const uint32_t cCmdNextScene = 0xa5140000ul;

/// The mask for the "off" command.
///
const uint32_t cCmdOff = 0xa5150000ul;

/// The mask for the scene name portion.
///
const uint32_t cCmdNextScene_SceneNameMask = 0x000000fful;

/// The mask for the scene entropy portion.
///
const uint32_t cCmdNextScene_EntropyMask = 0x0000ff00ul;


/// The index of the next scene to display.
///
volatile Scene::Name gNextSceneName = Scene::Black;

/// The entropy for the next scene to display.
///
volatile uint8_t gNextSceneEntropy = 0;


/// The state of the application.
///
volatile State gState = State::Play;

/// The elapsed timer to measure the length of a scene.
///
ElapsedTimer gSceneElapsedTime;

/// The application mode.
///
volatile Mode gMode = Mode::Automatic;

/// The selected scene duration.
///
volatile uint8_t gSceneDurationIndex = 2;

/// The current scene duration.
///
uint32_t gSceneDuration = (60000 - 50);

/// The automatic scenes index.
///
volatile uint8_t gAutoScenesIndex = 0;

/// The scene for the single mode.
///
volatile Scene::Name gSingleModeScene = Scene::IceSparkle;

/// The configuration mode.
///
volatile ConfigurationMode gConfigurationMode = ConfigurationMode::Mode;


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
	Display::setLedLevel(Communication::getIdentifier(), 32);
	for (uint8_t i = 0; i < 10; ++i) {
		Display::setLedLevel(Display::cLedCount-1, 16);
		Display::synchronizeAndShow();
		Helper::delayMs(100);
		Display::setLedLevel(Display::cLedCount-1, 0);
		Display::synchronizeAndShow();
		Helper::delayMs(100);
	}
}


/// Display the configuration.
///
void displayConfiguration()
{
	// Display the indicator for configuration.
	Display::setAllLedLevels(gConfigurationMode == ConfigurationMode::Mode ? 0 : 5);
	const bool indicatorState = (Helper::getSystemTimeMs() & (1u<<3u)) != 0;
	uint8_t indicatorLevel = (indicatorState ? 12 : 20);
	for (uint8_t i = 0; i < 6; ++i) {
		Display::setLedLevel(i, indicatorLevel);
	}
	
	// Light the middle LED for automatic mode
	if (gConfigurationMode == ConfigurationMode::Mode) {
		if (gMode == Mode::Automatic) {
			Display::setLedLevel(18, 20);
			Display::setLedLevel(cConfigurationRowA[gAutoScenesIndex], 40);
		} else if (gMode == Mode::Single) {
			auto sceneNumber = static_cast<uint8_t>(gSingleModeScene) - 1;
			auto rowB = sceneNumber / 6;
			auto rowA = sceneNumber % 6;
			Display::setLedLevel(cConfigurationRowA[rowA], 40);
			Display::setLedLevel(cConfigurationRowB[rowB], 15);
			Display::setLedLevel(cConfigurationRowB[(rowB+1)%6], 15);
		}		
	} else {
		for (uint8_t i = 0; i <= gSceneDurationIndex; ++i) {
			Display::setLedLevel(cConfigurationRowB[i], 40+i*2);
		}
	}
	
	Display::synchronizeAndShow();
}


/// Send an immediate scene change to all slave boards.
///
void sendImmediateChange(Scene::Name nextSceneName, uint8_t nextSceneEntropy)
{
	Helper::delayMs(50);
	Communication::sendData(cCmdNextScene | static_cast<uint32_t>(nextSceneName) | (static_cast<uint32_t>(nextSceneEntropy) << 8));
	Communication::waitUntilReadyToSend();
	Helper::delayMs(50); // Wait until all boards are ready.
	Communication::sendSynchronization();
}


/// Set a random next scene for the current mode.
///
void setRandomNextScene()
{
	if (gMode == Mode::Automatic) {
		auto autoScenes = cAutoScenes[gAutoScenesIndex];
		auto scenes = autoScenes.scenes;
		auto scenesCount = autoScenes.count;
		gNextSceneName = scenes[Helper::getRandom8(0, scenesCount-1)];
	} else {
		gNextSceneName = gSingleModeScene;		
	}
	gNextSceneEntropy = Helper::getRandom8(0, 0xff);
}


/// The initialization for a board in master mode.
///
void masterInitialize()
{
	// Set the first scene to display.
	setRandomNextScene();
		
	// Send the scene number to all other boards.
	sendImmediateChange(gNextSceneName, gNextSceneEntropy);
			
	// Blend to the first scene from black.
	Player::displayScene(Scene::Black, 0);
	Player::blendToScene(gNextSceneName, gNextSceneEntropy, cBlendDuration);
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
	} else {
		Communication::registerButtonPressFunction(&onButtonPress);
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


/// Process the off request state.
///
void processOffRequest()
{
	// Select the black scene and send this to all boards.
	gNextSceneName = Scene::Black;
	gNextSceneEntropy = 0;
	sendImmediateChange(gNextSceneName, gNextSceneEntropy);
	Player::blendToScene(gNextSceneName, gNextSceneEntropy, cBlendDuration);
	gState = State::Off;
}


/// Process the off state.
///
void processOff()
{
	// Wait here, until the user decides to turn the platform on.
}


/// Process the on request state.
///
void processOnRequest()
{
	// Set the first scene to display.
	setRandomNextScene();

	// Send the scene number to all other boards.
	sendImmediateChange(gNextSceneName, gNextSceneEntropy);
			
	// Set the scene duration.
	gSceneDuration = cSceneDurations[gSceneDurationIndex] - 50;
			
	// Blend to the first scene from black.
	Player::displayScene(Scene::Black, 0);
	Player::blendToScene(gNextSceneName, gNextSceneEntropy, cBlendDuration);
	gSceneElapsedTime.start();
	gState = State::BlendScene;
}


/// Process the play state.
///
void processPlay()
{
	if (gMode == Mode::Automatic && gSceneElapsedTime.elapsedTime() >= gSceneDuration) {
		// Select a random new scene, which is not the currently played one.
		auto lastScene = gNextSceneName;
		setRandomNextScene();
		while (lastScene == gNextSceneName) {
			setRandomNextScene();
		}
		Communication::sendData(cCmdNextScene | static_cast<uint32_t>(gNextSceneName) | (static_cast<uint32_t>(gNextSceneEntropy) << 8));
		gState = State::SendSynchronization;
	}
}


/// Process the send synchronization state.
///
void processSendSynchronization()
{
	if (gSceneElapsedTime.elapsedTime() >= gSceneDuration) {
		Communication::sendSynchronization();
		gState = State::BlendScene;
	}
}


/// Process the blend state.
///
void processBlendScene()
{
	if (Communication::isReadyToSend()) {
		Player::blendToScene(gNextSceneName, gNextSceneEntropy, cBlendDuration);
		gState = State::Play;
		gSceneElapsedTime.start();
	}
}


/// Process the configuration request.
///
void processConfigurationRequest()
{
	sendImmediateChange(Scene::Black, 0);
	gState = State::Configuration;
}


/// Process the configuration state.
///
void processConfiguration()
{
	displayConfiguration();
	if (gSceneElapsedTime.elapsedTime() >= cConfigurationDuration) {
		processOnRequest();
	}
}


/// The loop for a board in master mode.
///
__attribute__((noreturn))
void masterLoop()
{
	while (true) {
		if (gState != State::Configuration) {
			// Animate the current scene.
			Player::animate();			
		}
		switch (gState) {
			case State::Play: processPlay(); break;
			case State::SendSynchronization: processSendSynchronization(); break;
			case State::BlendScene: processBlendScene(); break;
			case State::OffRequest: processOffRequest(); break;
			case State::Off: processOff(); break;
			case State::OnRequest: processOnRequest(); break;
			case State::ConfigurationRequest: processConfigurationRequest(); break;
			case State::Configuration: processConfiguration(); break;
		}
	}	
}


/// The function to receive data from the master.
///
void onDataReceived(uint32_t value)
{
	// Check incoming commands.
	if ((value & cCmdMask) == cCmdNextScene) {
		gNextSceneName = static_cast<Scene::Name>(static_cast<uint8_t>(value & cCmdNextScene_SceneNameMask));
		gNextSceneEntropy = static_cast<uint8_t>((value & cCmdNextScene_EntropyMask) >> 8);
	} else if ((value & cCmdMask) == cCmdOff) {
		gNextSceneName = Scene::Black;
		gNextSceneEntropy = 0;
	}
}


/// The function to receive the synchronization from the master.
///
void onSynchronization()
{
	gState = State::BlendScene;
}


void onButtonPress(Communication::ButtonPress buttonPress)
{
	if (buttonPress == Communication::ButtonPress::Long) {
		if (gState == State::Configuration) {
			if (gConfigurationMode == ConfigurationMode::Mode) {
				gConfigurationMode = ConfigurationMode::SceneDuration;
			} else {
				gConfigurationMode = ConfigurationMode::Mode;
			}
		} else if (gState != State::Off && gState != State::OffRequest) {
			gState = State::OffRequest;	
		} else if (gState == State::Off) {
			gState = State::OnRequest;
		}
	} else if (buttonPress == Communication::ButtonPress::Short) {
		if (gState == State::Off) {
			gState = State::OnRequest;			
		} else if (gState == State::Play || gState == State::BlendScene || gState == State::SendSynchronization) {
			gState = State::ConfigurationRequest;
			gConfigurationMode = ConfigurationMode::Mode;
			gSceneElapsedTime.start();
		} else if (gState == State::Configuration) {
			if (gConfigurationMode == ConfigurationMode::Mode) {
				if (gMode == Mode::Automatic) {
					gAutoScenesIndex += 1;
					if (gAutoScenesIndex >= cAutoScenesCount) {
						gAutoScenesIndex = 0;
						gMode = Mode::Single;
						gSingleModeScene = static_cast<Scene::Name>(1);
					}
					} else if (gMode == Mode::Single) {
					gSingleModeScene = static_cast<Scene::Name>(static_cast<uint8_t>(gSingleModeScene) + 1);
					if (gSingleModeScene >= SceneManager::getSceneCount()) {
						gAutoScenesIndex = 0;
						gMode = Mode::Automatic;
						gSingleModeScene = static_cast<Scene::Name>(1);
					}
				}				
			} else {
				gSceneDurationIndex += 1;
				if (gSceneDurationIndex >= cSceneDurationsCount) {
					gSceneDurationIndex = 0;
				}
			}
			gSceneElapsedTime.start();
		}
	}
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
			Player::blendToScene(gNextSceneName, gNextSceneEntropy, cBlendDuration);
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
