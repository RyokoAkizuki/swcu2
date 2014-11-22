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

#include <algorithm>

#include "WorldManager.hpp"

namespace swcu {

bool WorldManager::loadMap(const mongo::OID& id)
{
    MONGO_WRAPPER({
        auto map = getDBConn()->findOne(
            Config::colNameMap,
            QUERY("_id" << id)
        );
        if(map.isEmpty())
        {
            LOG(ERROR) << "Map not found.";
            return false;
        }
        return _loadMap(map["_id"].OID());
    });
    return false;
}

bool WorldManager::unloadMap(const mongo::OID& id)
{
    if(!isMapLoaded(id))
    {
        return false;
    }
    for(auto i = mObjects.begin(); i != mObjects.end();)
    {
        if(i->second->getParentMap() == id)
        {
            i = mObjects.erase(i);
        }
        else
        {
            ++i;
        }
    }
    for(auto i = mVehicles.begin(); i != mVehicles.end();)
    {
        if(i->second->getParentMap() == id)
        {
            i = mVehicles.erase(i);
        }
        else
        {
            ++i;
        }
    }
    mLoadedMaps.erase(id.str());
    LOG(INFO) << "Map unloaded: " << id.str();
    return true;
}

bool WorldManager::isMapLoaded(const mongo::OID& id)
{
    return mLoadedMaps.count(id.str());
}

size_t WorldManager::loadAllMaps()
{
    MONGO_WRAPPER({
        size_t count = 0;
        mongo::BSONObj retField = BSON("_id" << 1);
        auto cur = getDBConn()->query(
            Config::colNameMap,
            QUERY("activated" << true),
            0, 0,
            &retField
        );
        while(cur->more())
        {
            auto map = cur->next()["_id"].OID();
            if(_loadMap(map))
            {
                ++count;
            }
        }
        return count;
    });
    return 0;
}

std::shared_ptr<Object> WorldManager::findObject(int id)
{
    auto iter = mObjects.find(id);
    if(iter == mObjects.end())
    {
        LOG(ERROR) << "Object not found. ID: " << id;
        return std::make_shared<FakeObject>();
    }
    return iter->second;
}

std::shared_ptr<Vehicle> WorldManager::findVehicle(int id)
{
    auto iter = mVehicles.find(id);
    if(iter == mVehicles.end())
    {
        LOG(ERROR) << "Vehicle not found. ID: " << id;
        return std::make_shared<FakeVehicle>();
    }
    return iter->second;
}

bool WorldManager::_loadMap(const mongo::OID& mapId)
{
    MONGO_WRAPPER({
        {
            auto cur = getDBConn()->query(
                Config::colNameMapObject,
                QUERY("parentmap" << mapId)
            );
            while(cur->more())
            {
                auto obj = cur->next();
                std::shared_ptr<Object> newObj(new Object(
                    obj["model"].numberInt(),
                    obj["x"].numberDouble(),
                    obj["y"].numberDouble(),
                    obj["z"].numberDouble(),
                    obj["rx"].numberDouble(),
                    obj["ry"].numberDouble(),
                    obj["rz"].numberDouble(),
                    obj["world"].numberInt(),
                    obj["interior"].numberInt(),
                    obj["_id"].OID(),
                    mapId,
                    obj["owner"].OID()
                ));
                mObjects.insert(std::make_pair(newObj->getInGameID(),
                    std::move(newObj)));
            }
        }

        {
            auto cur = getDBConn()->query(
                Config::colNameMapVehicle,
                QUERY("parentmap" << mapId)
            );
            while(cur->more())
            {
                auto veh = cur->next();
                auto com = veh["components"].Array();
                std::vector<int> components;
                for(auto &i : com)
                {
                    components.push_back(i.numberInt());
                }
                std::shared_ptr<Vehicle> newVeh(new Vehicle(
                    veh["model"].numberInt(),
                    veh["x"].numberDouble(),
                    veh["y"].numberDouble(),
                    veh["z"].numberDouble(),
                    veh["rotate"].numberDouble(),
                    veh["color1"].numberInt(),
                    veh["color2"].numberInt(),
                    veh["world"].numberInt(),
                    veh["interior"].numberInt(),
                    veh["respawndelay"].numberInt(),
                    veh["paintjob"].numberInt(),
                    components,
                    veh["_id"].OID(),
                    mapId,
                    veh["owner"].OID()
                ));
                mVehicles.insert(std::make_pair(newVeh->getInGameID(),
                    std::move(newVeh)));
            }
        }
        mLoadedMaps.insert(mapId.str());
        LOG(INFO) << "Map loaded: " << mapId.str();
        return true;
    });
    return false;
}

}
