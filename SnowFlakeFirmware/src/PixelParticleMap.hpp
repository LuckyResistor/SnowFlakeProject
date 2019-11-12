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
#pragma once


#include "SceneData.hpp"


/// A simple pixel particle
///
struct __attribute__((packed)) PixelParticle 
{
	uint8_t x; ///< The x position of the particle.
	uint16_t y; ///< The y position of the particle.
	uint8_t ySpeed; ///< The speed of the particle.
};


template<uint8_t particleCount, uint8_t offset>
class PixelParticleMap
{
public:
	/// The size in bytes used of the scene data.
	///
	static constexpr uint16_t cDataSize = sizeof(PixelParticle) * particleCount;	
	
	/// Get the next offset.
	///
	static constexpr uint16_t cNextOffset = cDataSize + offset;

	/// Get the particle count.
	///
	static constexpr uint8_t cParticleCount = particleCount;
	
public:
	
	
	/// Access one particle in the scene data.
	///
	PixelParticle* accessParticle(SceneData *data, const uint8_t index) const
	{
		const auto dataIndex = offset+sizeof(PixelParticle)*index;
		auto int8Value = &(data->int8[dataIndex]);
		return reinterpret_cast<PixelParticle*>(int8Value);
	}	
};