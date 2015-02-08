/*
 * Copyright 2014-2015 Yukino Hayakawa<tennencoll@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../Common/Common.hpp"
#include "../Streamer/Streamer.hpp"

#include "AreaManager.hpp"

namespace swcu {

Area::Area(int id) : mInGameId(id)
{
    if(mInGameId == 0)
    {
        LOG(ERROR) << "Failed to create area.";
    }
    else
    {
        AreaManager::get()._addArea(this);
    }
}

Area::~Area()
{
    if(mInGameId != 0)
    {
        DestroyDynamicArea(mInGameId);
    }
    AreaManager::get()._removeArea(this);
}

void Area::onEnter(int /* playerid */)
{
    LOG(WARNING) << "Unimplemented function Area::onEnter called.";
}

void Area::onLeave(int /* playerid */)
{
    LOG(WARNING) << "Unimplemented function Area::onLeave called.";
}

SphereArea::SphereArea(const kanko::Vector3& center, float radius,
    int world, int interior, int playerid) :
    Area(CreateDynamicSphere(
        center.x, center.y, center.z,
        radius, world, interior, playerid))
{}

BoxArea::BoxArea(const kanko::BBox& box,
    int world, int interior, int playerid) :
    Area(CreateDynamicCuboid(
        box.c.x - box.e.x,
        box.c.y - box.e.y,
        box.c.z - box.e.z,
        box.c.x + box.e.x,
        box.c.y + box.e.y,
        box.c.z + box.e.z,
        world, interior, playerid
    ))
{}

}
