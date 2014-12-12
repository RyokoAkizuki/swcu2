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

#include <sstream>
#include <algorithm>

#include "MapManager.hpp"

namespace swcu {

MapManager::MapManager()
{
    getDBConn()->createCollection(Config::colNameMap);
    getDBConn()->ensureIndex(Config::colNameMap, 
        BSON("name" << 1), true);
    getDBConn()->ensureIndex(Config::colNameMap, 
        BSON("activated" << 1), false);

    getDBConn()->createCollection(Config::colNameMapObject);
    getDBConn()->ensureIndex(Config::colNameMapObject, 
        BSON("map" << 1), false);

    getDBConn()->createCollection(Config::colNameMapVehicle);
    getDBConn()->ensureIndex(Config::colNameMapVehicle, 
        BSON("map" << 1), false);
}

bool MapManager::parse(MapType type, const std::string& name, int world,
    const mongo::OID& owner, std::string source)
{
    std::unique_ptr<Map> map(new Map(type, world, owner));

    if(!map->save(name))
    {
        return false;
    }

    std::replace(source.begin(), source.end(), '(', ' ');
    std::replace(source.begin(), source.end(), ')', ' ');
    std::replace(source.begin(), source.end(), ',', ' ');
    std::replace(source.begin(), source.end(), ';', ' ');

    std::istringstream stream(source);

    std::string span;
    int model;
    float x, y, z, rx, ry, rz, angle;

    while (stream >> span)
    {
        if (span == "CreateObject" || span == "CreateDynamicObject")
        {
            stream >> model >> x >> y >> z >> rx >> ry >> rz;
            map->addObject(model, x, y, z, rx, ry, rz, false, -1);
        }
        else if (span == "CreateVehicle" || span == "AddStaticVehicle" 
            || span == "AddStaticVehicleEx")
        {
            stream >> model >> x >> y >> z >> angle;
            map->addVehicle(model, x, y, z, angle, 0, 60);
        }
    }

    map->calculateBoundingSphere();
    
    mLoadedMaps.insert(std::make_pair(name, std::move(map)));
    return true;
}

bool MapManager::loadMap(const std::string& name)
{
    std::unique_ptr<Map> map(new Map(name));
    if(map->hasEntryInDatabase())
    {
        mLoadedMaps.insert(std::make_pair(name, std::move(map)));
        return true;
    }
    return false;
}

bool MapManager::unloadMap(const std::string& name)
{
    if(mLoadedMaps.erase(name))
    {
        LOG(INFO) << "Map unloaded: " << name;
        return true;
    }
    return false;
}

bool MapManager::isMapLoaded(const std::string& name)
{
    return mLoadedMaps.count(name);
}

size_t MapManager::loadAllMaps()
{
    MONGO_WRAPPER({
        size_t count = 0;
        auto cur = getDBConn()->query(
            Config::colNameMap,
            QUERY("activated" << true)
        );
        while(cur->more())
        {
            std::unique_ptr<Map> map(new Map(cur->next()));
            std::string name = map->getName();
            mLoadedMaps.insert(std::make_pair(name, std::move(map)));
            ++count;
        }
        LOG(INFO) << "Loaded " << count << " map(s).";
        return count;
    });
    return 0;
}

}
