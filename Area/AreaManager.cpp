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

#include "AreaManager.hpp"

namespace swcu {

bool AreaManager::_addArea(Area* area)
{
    if(area == nullptr)
    {
        LOG(WARNING) << "You are using an invalid pointer.";
        return false;
    }
    auto r = mAreas.insert(std::make_pair(area->getId(), area));
    return r.second;
}

bool AreaManager::_removeArea(Area* area)
{
    if(area == nullptr)
    {
        LOG(WARNING) << "You are using an invalid pointer.";
        return false;
    }
    return mAreas.erase(area->getId()) > 0;
}

void AreaManager::handleEnterAreaCallback(int playerid, int areaid)
{
    auto iter = mAreas.find(areaid);
    if(iter != mAreas.end())
    {
        iter->second->onEnter(playerid);
    }
}

void AreaManager::handleLeaveAreaCallback(int playerid, int areaid)
{
    auto iter = mAreas.find(areaid);
    if(iter != mAreas.end())
    {
        iter->second->onLeave(playerid);
    }
}

}
