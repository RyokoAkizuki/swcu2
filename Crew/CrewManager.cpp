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

#include "Crew.hpp"

#include "CrewManager.hpp"

namespace swcu {

CrewManager::CrewManager()
{
    getDBConn()->createCollection(Config::colNameCrew);
    getDBConn()->ensureIndex(Config::colNameCrew,
        BSON("name" << 1), true);
}

std::shared_ptr<Crew> CrewManager::getCrew(const mongo::OID& id)
{
    auto idstr = id.str();
    auto iter = mCrews.find(idstr);
    if(iter != mCrews.end())
    {
        return iter->second;
    }
    auto crew = std::make_shared<Crew>(id);
    mCrews.insert(std::make_pair(idstr, crew));
    return crew;
}

}
