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

#include <sampgdk/a_samp.h>
#include <eigen3/Eigen/Core>

#include "../Player/PlayerManager.hpp"
#include "../Multilang/Language.hpp"
 
#include "Map.hpp"

namespace swcu {

HouseMapArea::HouseMapArea(float x, float y, float z, float radius,
    int virtualworld, int interior, int playerid) :
    SphereArea(x, y, z, radius, virtualworld, interior, playerid)
{
}

void HouseMapArea::onEnter(int playerid)
{
    SendClientMessage(playerid, 0xFFFFFFFF, "You've enter a house.");
}

void HouseMapArea::onLeave(int playerid)
{
    SendClientMessage(playerid, 0xFFFFFFFF, "You've exited the house.");
}

PrisonMapArea::PrisonMapArea(float x, float y, float z, float radius,
    int virtualworld, int interior, int playerid) :
    SphereArea(x, y, z, radius, virtualworld, interior, playerid)
{
}

void PrisonMapArea::onEnter(int playerid)
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return;
    if(!p->isPrisonTermExceeded())
    {
        SendClientMessage(playerid, 0xFFFFFFFF, t(p, INFO_PUT_IN_PRISON));
    }
}

void PrisonMapArea::onLeave(int playerid)
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return;
    if(!p->isPrisonTermExceeded())
    {
        SendClientMessage(playerid, 0xFFFFFFFF,
            t(p, WARN_ESCAPE_FROM_PRISON));
        p->teleportTo(mX, mY, mZ, 0.0, mWorld, mInterior);
    }
}

Map::Map() : mType(LANDSCAPE), mAddTime(0),
    mActivated(true), mVirtualWorld(0),
    mArea(0.0), mSaved(false)
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
    if(obj->save())
    {
        mObjects.push_back(std::move(obj));
        return true;
    }
    return false;
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
    if(veh->save())
    {
        mVehicles.push_back(std::move(veh));
        return true;
    }
    return false;
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
        if(dbCheckError())
        {
            mVirtualWorld = world;
            LOG(INFO) << "Map " << mName << "'s world is set to " << world;
            return true;
        }
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
        if(dbCheckError())
        {
            mOwner = owner;
            LOG(INFO) << "Map " << mName << "'s owner is set to " << owner;
            return true;
        }
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
        if(dbCheckError())
        {
            mId                     = id;
            mAddTime                = datetime.toTimeT();
            mName                   = name;
            mSaved                  = true;
            LOG(INFO) << "Map " << mName << " saved.";
            return true;
        }
    });
    return false;
}

bool Map::calculateBoundingSphere()
{
    if(!hasEntryInDatabase())
    {
        LOG(ERROR) << "Doesn't have an entry in database. Won't calculate"
            " bouding sphere.";
        return false;
    }
    if(mObjects.size() == 0)
    {
        LOG(WARNING) << "Attemt to calculate bouding sphere of an empty "
            "point set.";
        return false;
    }

    /**
     * Original codes below come from http://stackoverflow.com/questions/
     * 17331203/bouncing-bubble-algorithm-for-smallest-enclosing-sphere
     */

    /*** BEGIN ***/

    std::vector<Eigen::Vector3f>    vertices;

    for(auto& i : mObjects)
    {
        vertices.push_back(Eigen::Vector3f(i->mX, i->mY, i->mZ));
    }

    Eigen::Vector3f                 center = vertices[0];
    Eigen::Vector3f                 diff;
    float                           radius = 0.0001f;
    float                           len, alpha, alphaSq, alphaSqReci;

    for(int i = 0; i < 2; ++i)
    {
        for(auto& pos : vertices)
        {
            diff    = pos - center;
            len     = powf(diff(0) * diff(0) + diff(1) * diff(1) + 
                        diff(2) * diff(2), 0.5); // length
            if(len > radius)
            {
                alpha       = len / radius;
                alphaSq     = alpha * alpha;
                alphaSqReci = 1 / alphaSq;
                radius      = 0.5f * (alpha + 1 / alpha) * radius;
                center      = 0.5f * ((1 + alphaSqReci) * center
                    + (1 - alphaSqReci) * pos);
            }
        }
    }

    for(auto& pos : vertices)
    {
        diff    = pos - center;
        len     = powf(diff(0) * diff(0) + diff(1) * diff(1) + 
                    diff(2) * diff(2), 0.5); // length
        if(len > radius)
        {
            radius  = (radius + len) * 0.5f;
            center  = center + ((len - radius) / len * diff);
        }
    }

    /*** END ***/

    mBoundX         = center(0);
    mBoundY         = center(1);
    mBoundZ         = center(2);
    /**
     * Since I am considering objects as mass points, I should make their
     * bounding sphere a bit larger.
     */
    mBoundRadius    = radius + 5.0;

    _createBoundingSphereArea();

    MONGO_WRAPPER({
        getDBConn()->update(
            Config::colNameMap,
            QUERY("_id" << mId),
            BSON("$set" << BSON(
                "boundx" << mBoundX <<
                "boundy" << mBoundY <<
                "boundz" << mBoundZ <<
                "boundr" << mBoundRadius
            ))
        );
        if(dbCheckError())
        {
            LOG(INFO) << "Map " << mName << "'s bounding shpere updated.";
            return true;
        }
    });
    return false;
}

void Map::_createBoundingSphereArea()
{
    switch(mType)
    {
        case HOUSE:
        mBoundingArea.reset(new HouseMapArea(mBoundX, mBoundY, mBoundZ,
            mBoundRadius, mVirtualWorld, -1, -1));
        break;
        case PRISON:
        mBoundingArea.reset(new PrisonMapArea(mBoundX, mBoundY, mBoundZ,
            mBoundRadius, mVirtualWorld, -1, -1));
        break;
        default:
        LOG(WARNING) << "No proper bounding for map type " << mType;
        break;
    }
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
            auto doc = objcur->next();
            std::unique_ptr<Object> obj(new Object(doc, mVirtualWorld));
            mObjects.push_back(std::move(obj));
        }
        LOG(INFO) << "Loaded " << mObjects.size() << " object(s).";

        auto vehcur = getDBConn()->query(
            Config::colNameMapVehicle,
            QUERY("map" << mId)
        );
        while(vehcur->more())
        {
            auto doc = vehcur->next();
            std::unique_ptr<LandscapeVehicle>
                veh(new LandscapeVehicle(doc, mVirtualWorld));
            mVehicles.push_back(std::move(veh));
        }
        LOG(INFO) << "Loaded " << mVehicles.size() << " vehicle(s).";

        if(data.hasField("boundr"))
        {
            mBoundX     = data["boundx"].numberDouble();
            mBoundY     = data["boundy"].numberDouble();
            mBoundZ     = data["boundz"].numberDouble();
            mBoundRadius= data["boundr"].numberDouble();
            _createBoundingSphereArea();
        }
        else
        {
            calculateBoundingSphere();
        }

        LOG(INFO) << "Map " << mName << " loaded.";
        return true;
    });
    return false;
}

}
