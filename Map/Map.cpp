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

#include "Map.hpp"

namespace swcu {

Map::Map() : mType(LANDSCAPE), mAddTime(0),
    mActivated(true), mVirtualWorld(0),
    mArea(0.0), mStreamedAreaId(0), mSaved(false)
{
}

Map::Map(MapType type, int world, const mongo::OID& owner) : Map::Map()
{
    mType = type; mOwner = owner; mVirtualWorld = world;
}

Map::Map(const std::string& name) : Map::Map()
{
    mName = name;
    reload();
}

Map::Map(const mongo::BSONObj& data) : mSaved(false)
{
    _loadMap(data);
}

bool Map::addObject(int model, float x, float y, float z,
    float rx, float ry, float rz, bool editable, int interior)
{
    if(!hasEntryInDatabase())
    {
        LOG(ERROR) << "Can't add object to a map that doesn't have "
            "an entry in the database.";
        return false;
    }
    std::unique_ptr<Object>
        obj(new Object(model, x, y, z, rx, ry, rz, editable, mId,
            mVirtualWorld, interior));
    obj->save();
    mObjects.push_back(std::move(obj));
    LOG(INFO) << "Object added to map.";
    return true;
}

bool Map::addVehicle(int model, float x, float y, float z,
    float angle, int interior, int respawndelay)
{
    if(!hasEntryInDatabase())
    {
        LOG(ERROR) << "Can't add vehicle to a map that doesn't have "
            "an entry in the database.";
        return false;
    }
    std::unique_ptr<LandscapeVehicle>
        veh(new LandscapeVehicle(model, x, y, z, angle, mId,
            mVirtualWorld, interior, respawndelay));
    veh->save();
    mVehicles.push_back(std::move(veh));
    LOG(INFO) << "Vehicle added to map.";
    return true;
}

bool Map::setWorld(int world)
{
    if(!hasEntryInDatabase())
    {
        LOG(ERROR) << "Can't modify a map that doesn't have "
            "an entry in the database.";
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->update(
            Config::colNameMap,
            QUERY("_id" << mId),
            BSON("$set" << BSON(
                "world" << world
            ))
        );
        mVirtualWorld = world;
        LOG(INFO) << "Map " << mName << "'s world is set to " << world;
        return true;
    });
    return false;
}

bool Map::setOwner(const mongo::OID& owner)
{
    if(!hasEntryInDatabase())
    {
        LOG(ERROR) << "Can't modify a map that doesn't have "
            "an entry in the database.";
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->update(
            Config::colNameMap,
            QUERY("_id" << mId),
            BSON("$set" << BSON(
                "owner" << owner
            ))
        );
        mOwner = owner;
        LOG(INFO) << "Map " << mName << "'s owner is set to " << owner;
        return true;
    });
    return false;
}

bool Map::reload()
{
    mObjects.clear();
    mVehicles.clear();
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(
            Config::colNameMap,
            QUERY("name" << GBKToUTF8(mName))
        );
        if(doc.isEmpty())
        {
            LOG(WARNING) << "Map " << mName << " can't be found.";
            return false;
        }
        else
        {
            return _loadMap(doc);
        }
    });
    return false;
}

bool Map::save(const std::string& name)
{
    if(hasEntryInDatabase())
    {
        LOG(ERROR) << "This Map has already been saved.";
        return false;
    }
    MONGO_WRAPPER({
        mongo::OID id           = mongo::OID::gen();
        auto datetime           = mongo::jsTime();
        getDBConn()->insert(
            Config::colNameMap,
            BSON(
                "_id"           << id               <<
                "type"          << mType            <<
                "name"          << GBKToUTF8(name)  <<
                "owner"         << mOwner           <<
                "addtime"       << datetime         <<
                "activated"     << mActivated       <<
                "world"         << mVirtualWorld    <<
                "area"          << mArea
                )
            );
        auto err = getDBConn()->getLastError();
        if (err.size())
        {
            LOG(ERROR) << "Map saving failed: " << err;
            return false;
        }
        mId                     = id;
        mAddTime                = datetime.toTimeT();
        mName                   = name;
        mSaved                  = true;
        LOG(INFO) << "Map " << mName << " saved.";
        return true;
    });
    return false;
}

bool Map::_loadMap(const mongo::BSONObj& data)
{
    if(hasEntryInDatabase())
    {
        LOG(ERROR) << "This Map object is assigned with another map.";
        return false;
    }
    MONGO_WRAPPER({
        mId             = data["_id"].OID();
        mType           = MapType(data["type"].numberInt());
        mName           = UTF8ToGBK(data["name"].str());
        mOwner          = data["owner"].OID();
        mAddTime        = data["addtime"].date().toTimeT();
        mActivated      = data["activated"].boolean();
        mVirtualWorld   = data["world"].numberInt();
        mArea           = data["area"].numberDouble();
        mSaved          = true;

        auto objcur = getDBConn()->query(
            Config::colNameMapObject,
            QUERY("map" << mId)
        );
        while(objcur->more())
        {
            auto data = objcur->next();
            std::unique_ptr<Object> obj(new Object(data, mVirtualWorld));
            mObjects.push_back(std::move(obj));
        }
        LOG(INFO) << "Loaded " << mObjects.size() << " object(s).";

        auto vehcur = getDBConn()->query(
            Config::colNameMapVehicle,
            QUERY("map" << mId)
        );
        while(vehcur->more())
        {
            auto data = vehcur->next();
            std::unique_ptr<LandscapeVehicle>
                veh(new LandscapeVehicle(data, mVirtualWorld));
            mVehicles.push_back(std::move(veh));
        }
        LOG(INFO) << "Loaded " << mVehicles.size() << " vehicle(s).";

        LOG(INFO) << "Map " << mName << " loaded.";
        return true;
    });
    return false;
}

}
