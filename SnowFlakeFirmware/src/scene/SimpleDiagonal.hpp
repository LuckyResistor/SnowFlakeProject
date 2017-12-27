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


#include "../Scene.hpp"


/// \namespace scene::SimpleDiagonal
/// A simple diagonal scene.


namespace scene {
namespace SimpleDiagonal {


/// The number of frames for this scene
///
const FrameIndex cFrameCount = 150;

/// The function to initialize this scene.
///
void initialize(SceneData *data, uint8_t entropy);

/// The function to get a frame from this scene.
///
Frame getFrame(SceneData *data, FrameIndex frameIndex);


}
}

