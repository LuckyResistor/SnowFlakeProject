//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2019 by Lucky Resistor. See LICENSE for details.
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
#include "AutoScenes.hpp"


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


/// All different automatic scene modes.
///
const AutoScenes cAutoScenes[] = {
	{cAutoScenesA, sizeof(cAutoScenesA)/sizeof(Scene::Name)},
	{cAutoScenesB, sizeof(cAutoScenesB)/sizeof(Scene::Name)},
	{cAutoScenesC, sizeof(cAutoScenesC)/sizeof(Scene::Name)},
	{cAutoScenesD, sizeof(cAutoScenesD)/sizeof(Scene::Name)},
};
const uint8_t cAutoScenesCount = sizeof(cAutoScenes)/sizeof(AutoScenes);

