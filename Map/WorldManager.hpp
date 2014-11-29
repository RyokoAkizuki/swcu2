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

#pragma once

#include <unordered_map>
#include <unordered_set>

#include "../Utility/Singleton.hpp"

#include "Items.hpp"

namespace swcu {

class WorldManager : public Singleton<WorldManager>
{
protected:
    typedef std::unordered_map<int, std::shared_ptr<Object>>
                                    ObjectMap;
    typedef std::unordered_map<int, std::shared_ptr<Vehicle>>
                                    VehicleMap;
    /*
    typedef std::unordered_map<int, std::shared_ptr<MapIcon>>
                                    MapIconMap;
    */

    std::unordered_set<std::string> mLoadedMaps;
    ObjectMap                       mObjects;
    VehicleMap                      mVehicles;
    /*
    MapIconMap                      mMapIcons;
    */

public:
    virtual         ~WorldManager() {}

            bool    loadMap(const mongo::OID& id);
            bool    unloadMap(const mongo::OID& id);
            bool    isMapLoaded(const mongo::OID& id);
    /**
     * Load all activated maps from database.
     * @return Amount of loaded maps.
     */
            size_t  loadAllMaps();

            std::shared_ptr<Object>     findObject(int id);
            std::shared_ptr<Vehicle>    findVehicle(int id);

protected:
            bool    _loadMap(const mongo::OID& mapId);
};

}
