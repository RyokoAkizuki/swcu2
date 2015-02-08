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

#include <kanko/Common/Console.hpp>
#include <sampgdk/a_samp.h>
#include <algorithm>

#include "../Player/PlayerManager.hpp"

#include "Map.hpp"

namespace swcu {

HouseMapArea::HouseMapArea(Map* map) :
    SphereArea(map->mBoundSphereCenter,
        map->mBoundSphereRadius, map->mVirtualWorld, -1, -1),
    mMap(map)
{
}

void HouseMapArea::onEnter(int playerid)
{
    std::string msg = "你进入了" + mMap->getName();
    SendClientMessage(playerid, 0xFFFFFFFF, msg.c_str());
}

void HouseMapArea::onLeave(int playerid)
{
    SendClientMessage(playerid, 0xFFFFFFFF,
        ("你离开了房产" + mMap->getName()).c_str());
}

PrisonMapArea::PrisonMapArea(Map* map) :
    SphereArea(map->mBoundSphereCenter,
        map->mBoundSphereRadius, map->mVirtualWorld, -1, -1),
    mMap(map)
{
}

void PrisonMapArea::onEnter(int playerid)
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return;
    if(!p->isPrisonTermExceeded())
    {
        SendClientMessage(playerid, 0xFFFFFFFF, "你被送进了监狱.");
    }
}

void PrisonMapArea::onLeave(int playerid)
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return;
    if(!p->isPrisonTermExceeded())
    {
        SendClientMessage(playerid, 0xFFFFFFFF, "不要越狱.");
        p->teleportTo("##prison");
    }
}

Map::Map() : StorableObject(Config::colNameMap),
    mType(LANDSCAPE), mActivated(true), mVirtualWorld(0)
    , mVariance(0.0), mPrice(0)
{
}

Map::Map(MapType type, int world, const mongo::OID& owner,
    const std::string& name) : Map::Map()
{
    mType = type; mOwner = owner; mVirtualWorld = world;
    mName = name;
    if(_createObject(BSON(
        "type"          << mType            <<
        "name"          << GBKToUTF8(mName) <<
        "owner"         << mOwner           <<
        "activated"     << mActivated       <<
        "world"         << mVirtualWorld
    )))
    {
        LOG(INFO) << "Map " << mName << " saved.";
    }
}

Map::Map(const std::string& name) : Map::Map()
{
    mName = name;
    _loadObject("name", GBKToUTF8(name));
}

Map::Map(const mongo::BSONObj& data) : Map::Map()
{
    MONGO_WRAPPER({
        mId = data["_id"].OID();
        if(_parseObject(data))
        {
            mValid = true;
        }
    });
}

std::string Map::getTypeStr() const
{
    switch(mType)
    {
        case LANDSCAPE:
        return "地形";
        case DEATHMATCH:
        return "死斗";
        case STUNT:
        return "特技";
        case PROPERTY:
        return "房产";
        case BUSINESS:
        return "生意";
        case PRISON:
        return "监狱";
        default:
        return "未知";
    }
}

bool Map::addObject(int model, float x, float y, float z,
    float rx, float ry, float rz, bool editable, int interior)
{
    std::unique_ptr<Object>
        obj(new Object(model, x, y, z, rx, ry, rz, editable, mId,
            mVirtualWorld, interior));
    if(obj->isValid())
    {
        mObjects.push_back(std::move(obj));
        return true;
    }
    return false;
}

bool Map::addVehicle(int model, float x, float y, float z,
    float angle, int interior, int respawndelay)
{
    std::unique_ptr<LandscapeVehicle>
        veh(new LandscapeVehicle(model, x, y, z, angle, mId,
            mVirtualWorld, interior, respawndelay));
    if(veh->isValid())
    {
        mVehicles.push_back(std::move(veh));
        return true;
    }
    return false;
}

bool Map::setWorld(int world)
{
    if(_updateField("$set", "world", world))
    {
        mVirtualWorld = world;
        LOG(INFO) << "Map " << mName << "'s world is set to " << world;
        return true;
    }
    return false;
}

bool Map::setOwner(const mongo::OID& owner)
{
    if(_updateField("$set", "owner", owner))
    {
        mOwner = owner;
        LOG(INFO) << "Map " << mName << "'s owner is set to " << owner;
        return true;
    }
    return false;
}

bool Map::setPrice(size_t price)
{
    if(_updateField("$set", "price", price))
    {
        mPrice = price;
        LOG(INFO) << "Map " << mName << "'s price is set to " << price;
        return true;
    }
    return false;
}

bool Map::setName(const std::string& name)
{
    if(_updateField("$set", "name", GBKToUTF8(name)))
    {
        mName = name;
        LOG(INFO) << "Map " << mName << "'s name is set to " << name;
        return true;
    }
    return false;
}

bool Map::setPassword(const std::string& password)
{
    if(_updateField("$set", "password", GBKToUTF8(password)))
    {
        mPassword = password;
        LOG(INFO) << "Map " << mName << "'s password is set to " << password;
        return true;
    }
    return false;
}

bool Map::sell(Player* player)
{
    if(mType != PROPERTY || mOwner == mongo::OID()) return false;
    auto oldOwner = mOwner;
    if(setOwner(mongo::OID()))
    {
        player->increaseMoney(mPrice);
        LOG(INFO) << "Map " << mName << " is on sale now.";
        EventLog("Property", "propertySold", BSON(
            "previousOwner" << oldOwner
        ));
        return true;
    }
    return false;
}

bool Map::buy(Player* player)
{
    if(mType != PROPERTY || mOwner != mongo::OID()) return false;
    if(player->increaseMoney(-mPrice) && setOwner(player->getId()))
    {
        EventLog("Property", "propertyBought", BSON(
            "newOwner" << player->getId()
        ));
        LOG(INFO) << "Map " << mName << " is bought.";
        return true;
    }
    return false;
}

bool Map::setEntrance(const std::string& name)
{
    if(_updateField("$set", "entrance", GBKToUTF8(name)))
    {
        mEntranceTeleportName = name;
        return true;
    }
    return false;
}

void Map::teleportToEntrance(int playerid) const
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p != nullptr)
    {
        p->teleportTo(mEntranceTeleportName);
    }
}

bool Map::_calculateBoundingSphere()
{
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

    std::vector<kanko::Vector3>     vertices;

    for(auto& i : mObjects)
    {
        vertices.push_back(kanko::Vector3(i->mX, i->mY, i->mZ));
    }
    for(auto& i : mVehicles)
    {
        vertices.push_back(kanko::Vector3(i->mX, i->mY, i->mZ));
    }

    kanko::Vector3                  center = vertices[0];
    kanko::Vector3                  diff;
    float                           radius = 0.0001f;
    float                           len, alpha, alphaSq, alphaSqReci;

    for(int i = 0; i < 2; ++i)
    {
        for(auto& pos : vertices)
        {
            diff    = pos - center;
            len     = diff.length();
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
        len     = diff.length();

        // TO-DO: Spatial Cluster Analysis
        if(len > 10000.0)
        {
            LOG(INFO) << kanko::FRONT_RED
                << "Map " << mName << " has an object which "
                "is far from the center " << pos
                << kanko::FRONT_DEFAULT;
            continue;
        }

        if(len > radius)
        {
            radius  = (radius + len) * 0.5f;
            center  = center + ((len - radius) / len * diff);
        }
    }

    /*** END ***/

    mBoundSphereCenter  = center;

    /**
     * Since I am considering objects as mass points, I should make their
     * bounding sphere a bit larger.
     */
    mBoundSphereRadius  = radius + 5.0;

    // calculate variance
    for(auto& pos : vertices)
    {
        float v = (pos - center).lengthSquared();
        mVariance += v;
    }

    mVariance /= vertices.size();

    if(mVariance > 10000.0)
        LOG(INFO) << kanko::FRONT_RED
            << "Map " << mName << " has a variance of " << mVariance
            << kanko::FRONT_DEFAULT;
    else
        LOG(INFO) << "Map " << mName << " has a variance of " << mVariance;

    return true;
}

bool Map::_calculateBoundingBox()
{
    if(mObjects.size() == 0)
    {
        LOG(WARNING) << "Calculate bouding rectangle of an empty "
            "point set.";
        return false;
    }

    for(auto& i : mObjects)
    {
        kanko::join(mBBox, kanko::Vector3(i->mX, i->mY, i->mZ));
    }

    for(auto& i : mVehicles)
    {
        kanko::join(mBBox, kanko::Vector3(i->mX, i->mY, i->mZ));
    }

    mBBox.e += kanko::Vector3(5.0);
    return true;
}

void Map::updateBounding()
{
    _calculateBoundingSphere();
    _calculateBoundingBox();
    switch(mType)
    {
        case PROPERTY:
        mBoundingArea.reset(new HouseMapArea(this));
        break;
        case PRISON:
        mBoundingArea.reset(new PrisonMapArea(this));
        break;
        default:
        LOG(WARNING) << "No proper bounding for map type " << mType;
        break;
    }
}

bool Map::_parseObject(const mongo::BSONObj& data)
{
    MONGO_WRAPPER({
        mType           = MapType(data["type"].numberInt());
        mName           = UTF8ToGBK(data["name"].str());
        mOwner          = data["owner"].OID();
        mActivated      = data["activated"].boolean();
        mVirtualWorld   = data["world"].numberInt();

        try
        {
            if(mType == PROPERTY)
            {
                mPrice      = data["price"].numberInt();
                mPassword   = UTF8ToGBK(data["password"].str());
                mEntranceTeleportName   = UTF8ToGBK(data["entrance"].str());
            }
        }
        catch(...)
        {
            setPrice(1000);
            setPassword("");
            setEntrance("");
        }

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
        updateBounding();
        LOG(INFO) << "Map " << mName << " loaded.";
        return true;
    });
    return false;
}

bool Map::deleteFromDatabase()
{
    if(!mValid)
    {
        LOG(ERROR) << "Cannot delete a map not saved.";
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->remove(
            Config::colNameMapObject,
            QUERY("map" << mId)
        );
        dbCheckError();
        getDBConn()->remove(
            Config::colNameMapVehicle,
            QUERY("map" << mId)
        );
        dbCheckError();
        getDBConn()->remove(
            Config::colNameMap,
            QUERY("_id" << mId)
        );
        dbCheckError();
        LOG(INFO) << "Map " << mName << " is removed.";
        mValid = false;
        return true;
    });
    return false;
}

}
