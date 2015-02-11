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

#include <map>

#include "../Utility/Singleton.hpp"

#include "Map.hpp"

namespace swcu {

class MapManager : public Singleton<MapManager>
{
protected:
    std::map<std::string, std::shared_ptr<Map>> mLoadedMaps;

protected:
                    MapManager();
                    
    friend class Singleton<MapManager>;
    friend class MapViewDialog;

public:
    virtual         ~MapManager() {}

    /**
     * This function parses an input stream in order to create a map
     * using the information from that. It will parse:
     * CreateObject|CreateDynamicObject(model, x, y, z, rx, ry, rz)
     * CreateVehicle|AddStaticVehicle|AddStaticVehicleEx
     *     (model, x, y, z, angle, color1, color2)
     */
            std::shared_ptr<Map> parse(MapType type, const std::string& name, int world,
                const mongo::OID& owner, std::string source);
            bool    loadMap(const std::string& name);
            bool    unloadMap(const std::string& name);
            bool    isMapLoaded(const std::string& name);
            std::shared_ptr<Map> findMap(const std::string& name);
            
    /**
     * Load all activated maps from database.
     * @return Amount of loaded maps.
     */
            size_t  loadAllMaps();

            void    addWebServices();
};

}
