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
#include "Scene.hpp"


#include <cstring>


namespace {
void emptyInitScene(SceneData*)
{
	// empty
}	
Frame emptyGetFrame(SceneData*, FrameIndex)
{
	return Frame();
}
}


SceneData::SceneData()
{
	std::memset(&int32Data, 0, sizeof(uint32_t)*cSize);
}


void SceneData::clear()
{
	std::memset(&int32Data, 0, sizeof(uint32_t)*cSize);
}


Scene::Scene()
	: _frameCount(10), _initFn(&emptyInitScene), _getFrameFn(&emptyGetFrame)
{
}


Scene::Scene(uint32_t frameCount, InitFn initFn, GetFrameFn getFrameFn)
	: _frameCount(frameCount), _initFn(initFn), _getFrameFn(getFrameFn)
{
}


Scene::Scene(const Scene &copy)
	: _frameCount(copy._frameCount), _initFn(copy._initFn), _getFrameFn(copy._getFrameFn)
{
}


const Scene& Scene::operator=(const Scene &assign)
{
	_frameCount = assign._frameCount;
	_initFn = assign._initFn;
	_getFrameFn = assign._getFrameFn;
	return assign;
}


void Scene::init(SceneData *data)
{
	(*_initFn)(data);
}


Frame Scene::getFrame(SceneData *data, FrameIndex frameIndex)
{
	return (*_getFrameFn)(data, frameIndex);
}


