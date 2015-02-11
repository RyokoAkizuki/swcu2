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

#include <sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "../Web/WebServiceManager.hpp"

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

std::shared_ptr<Map> MapManager::parse(MapType type, const std::string& name, int world,
    const mongo::OID& owner, std::string source)
{
    std::shared_ptr<Map> map(new Map(type, world, owner, name));

    if(!map->isValid())
    {
        return map;
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

    map->updateBounding();

    mLoadedMaps.insert(std::make_pair(name, map));
    return map;
}

bool MapManager::loadMap(const std::string& name)
{
    std::shared_ptr<Map> map(new Map(name));
    if(map->isValid())
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
    mLoadedMaps.clear();
    LOG(INFO) << "Loaded maps cleared.";
    MONGO_WRAPPER({
        size_t count = 0;
        auto cur = getDBConn()->query(
            Config::colNameMap,
            QUERY("activated" << true)
        );
        while(cur->more())
        {
            std::shared_ptr<Map> map(new Map(cur->next()));
            std::string name = map->getName();
            auto r = mLoadedMaps.insert(std::make_pair(name, std::move(map)));
            if(r.second)
            {
                ++count;
            }
            else
            {
                LOG(WARNING) << "Error occurred while loading map " << name;
            }
        }
        LOG(INFO) << "Loaded " << count << " map(s).";
        return count;
    });
    return 0;
}

std::shared_ptr<Map> MapManager::findMap(const std::string& name)
{
    auto iter = mLoadedMaps.find(name);
    if(iter == mLoadedMaps.end())
    {
        return std::shared_ptr<Map>();
    }
    else
    {
        return iter->second;
    }
}

void MapManager::addWebServices()
{
    /**
     * Get map indices.
     * Example URI:
     * /maps/
     */
    WebServiceManager::get().bindMethod(
        "^/maps/([^/]*)$", "GET",
    [this](std::ostream& response, HTTPRequertPtr /* request*/ ) {
        std::stringstream array;
        array << "{ \"data\" : [";
        for(auto& i : mLoadedMaps)
        {
            array << i.second->getJSON() << ",";
        }
        std::string resp = array.str();
        resp[resp.size() - 1] = ']';
        resp += "}";
        writeResponse(response, 200, CONTENT_TYPE_APP_JSON, resp);
    });
    /**
     * Get info of a map.
     * Example URI:
     * /map/name/exampleMap
     */
    WebServiceManager::get().bindMethod(
        "^/maps/name/([^/]+)$", "GET",
    [this](std::ostream& response, HTTPRequertPtr request) {
        auto map = findMap(UTF8ToGBK(request->path_match[1]));
        if(map == nullptr)
        {
            writeResponse(response, 404, CONTENT_TYPE_TEXT_PLAIN, "");
            return;
        }
        writeResponse(response, 200, CONTENT_TYPE_APP_JSON, map->getJSON());
    });
    /**
     * Add a map
     * Example URI:
     * /map/add
     */
    WebServiceManager::get().bindMethod(
        "^/maps/add", "POST",
    [this](std::ostream& response, HTTPRequertPtr request) {
        std::istreambuf_iterator<char> eos;
        std::string s(std::istreambuf_iterator<char>(request->content), eos);
        ParamSet p;
        parseParam(s, p);
        std::string name = p["name"];
        boost::algorithm::trim(name);
        auto map = parse(
            MapType(atoi(p["type"].c_str())),
            UTF8ToGBK(name),
            -1,
            mongo::OID(),
            p["code"]
        );
        if(map->isValid())
        {
            writeResponse(response, 200, CONTENT_TYPE_TEXT_PLAIN,
                STR("地图添加成功\n"
                    "名称: " << GBKToUTF8(map->getName()) << "\n" <<
                    "交通工具数量: " << map->getVehicleCount() << "\n"
                    "Obj数量: " << map->getObjectCount()
                )
            );
        }
        else
        {
            writeResponse(response, 200, CONTENT_TYPE_TEXT_PLAIN,
                "地图添加失败, 可能是已经有重名的地图, "
                "或者服务器发生了错误, 请检查SAMP服务器日志."
            );
        }
    });
}

}
