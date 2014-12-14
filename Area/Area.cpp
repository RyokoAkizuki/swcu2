/*
 * Copyright 2014 Yukino Hayakawa<tennencoll@gmail.com>
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
        LOG(INFO) << "Area created with id " << id;
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

SphereArea::SphereArea(float x, float y, float z, float radius,
        int world, int interior, int playerid) :
    Area(CreateDynamicSphere(x, y, z, radius, world, interior, playerid)),
    mX(x), mY(y), mZ(z), mRadius(radius), mWorld(world),
    mInterior(interior), mPlayer(playerid)
{}

}
