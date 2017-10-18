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
	

void initialize()
{
	Hardware::initialize();
	Helper::initialize();
	Display::initialize();
	Communication::initialize();
	SceneManager::initialize();
	Player::initialize();

	// Fade the first scene from black.
	Player::displayScene(Scene::Black);
	Player::blendToScene(Scene::SimpleShift, 150);
	while (Player::getState() == Player::State::Blend) {
		Player::animate();
	}
}


void loop()
{
	// Common animation.
	while (true) {
		Player::animate();
	}
}


}
