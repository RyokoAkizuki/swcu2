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

#include "GangZoneManager.hpp"

namespace swcu {

GangZoneManager::GangZoneManager()
{
    getDBConn()->createCollection(Config::colNameGangZone);
    getDBConn()->ensureIndex(Config::colNameGangZone,
        BSON("name" << 1), true);
}

void GangZoneManager::loadAll()
{
    mZones.clear();
    MONGO_WRAPPER({
        auto cur = getDBConn()->query(
            Config::colNameGangZone,
            mongo::Query()
        );
        while(cur->more())
        {
            std::shared_ptr<GangZone> zone(new GangZone(cur->next()));
            int ingameid = zone->getInGameId();
            if(zone->isValid())
            {
                mZones.insert(std::make_pair(ingameid, std::move(zone)));
            }
        }
    });
}

void GangZoneManager::showAll()
{
    for(auto &i : mZones)
    {
        i.second->updatePlayersHUD();
    }
}

std::shared_ptr<GangZone> GangZoneManager::getGangZone(int32_t id)
{
    auto iter = mZones.find(id);
    if(iter == mZones.end())
    {
        LOG(WARNING) << "Building an invalid GangZone.";
        return std::shared_ptr<GangZone>();
    }
    return iter->second;
}

void GangZoneManager::updateWarStatus()
{
    for(auto &i : mZones)
    {
        i.second->updateWarStatus();
    }
}

}
