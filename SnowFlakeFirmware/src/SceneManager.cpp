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
#include "SceneManager.hpp"


#include "scene/Black.hpp"
#include "scene/SimpleFlash.hpp"
#include "scene/SimpleShift.hpp"
#include "scene/SimpleRotation.hpp"
#include "scene/SimpleDiagonal.hpp"
#include "scene/SimpleRandomParticle.hpp"
#include "scene/SimpleRandomFlicker.hpp"
#include "scene/IceSparkle.hpp"


namespace SceneManager {


/// A simple structure for a scene entry.
///
typedef struct {
	const Scene::Name name; ///< The name for this scene.
	const uint32_t frameCount; ///< The frame count for this scene.
	const Scene::InitFn initFn; ///< The function to initialize the scene data.
	const Scene::GetFrameFn getFrameFn; ///< The function to get a new frame for this scene.
} SceneEntry;


/// Macro to simple define the scenes from the name.
///
#define LR_SCENE_ENTRY(name) {Scene::name, scene::name::cFrameCount, &scene::name::initialize, &scene::name::getFrame}


/// A static array with all registered scenes in this firmware.
///
const SceneEntry cScenes[] __aligned(4) = {
	LR_SCENE_ENTRY(Black),
	LR_SCENE_ENTRY(SimpleShift),
	LR_SCENE_ENTRY(SimpleRotation),
	LR_SCENE_ENTRY(SimpleDiagonal),
	LR_SCENE_ENTRY(SimpleFlash),
	LR_SCENE_ENTRY(SimpleRandomParticle),
	LR_SCENE_ENTRY(SimpleRandomFlicker),
	LR_SCENE_ENTRY(IceSparkle),
	{Scene::Black, 0, nullptr, nullptr} // The end mark.
};


void initialize()
{
	// nothing to do.
}


uint8_t getSceneCount()
{
	static uint8_t sceneCount = 0;
	if (sceneCount == 0) {
		const SceneEntry *entry = cScenes;
		while (entry->frameCount > 0) {
			++sceneCount;
			++entry;
		}
	}
	return sceneCount;
}


Scene getScene(Scene::Name sceneName)
{
	// Iterate through all scenes and return the matching one.
	const SceneEntry *entry = cScenes;
	do {
		if (entry->name == sceneName) {
			return Scene(entry->frameCount, entry->initFn, entry->getFrameFn);
		}
		++entry;
	} while (entry->frameCount > 0);
	// If for unknown reasons the searched scene is not found, just use the first one.
	return Scene(cScenes[0].frameCount, cScenes[0].initFn, cScenes[0].getFrameFn);
}


}