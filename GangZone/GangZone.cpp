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

#include "../Crew/CrewManager.hpp"
#include "../Crew/Crew.hpp"

#include "GangZone.hpp"

namespace swcu {

GangZoneBoxArea::GangZoneBoxArea(GangZone* zone) :
    BoxArea(zone->mMinX, zone->mMinY, zone->mMinZ,
        zone->mMaxX, zone->mMaxY, zone->mMaxZ),
    mZone(zone)
{
}

void GangZoneBoxArea::onEnter(int playerid)
{
    if(mZone->mCrewPtr->isValid())
    {
        SendClientMessage(playerid, 0xFFFFFFFF, CSTR(
            "你进入了" << mZone->getName() << ", 这片地盘归" <<
            mZone->mCrewPtr->getName() << "所有."
        ));
    }
    else
    {
        SendClientMessage(playerid, 0xFFFFFFFF, CSTR(
            "你进入了" << mZone->getName() << ", 这片地盘还没有人占领."
        ));
    }
}

void GangZoneBoxArea::onLeave(int playerid)
{
    SendClientMessage(playerid, 0xFFFFFFFF, CSTR(
        "你离开了" << mZone->getName() << "."
    ));
}

GangZone::GangZone() : mInGameId(-1), mValid(false)
{
}

GangZone::GangZone(
    const std::string& name,
    float minx, float miny, float minz,
    float maxx, float maxy, float maxz
) : GangZone()
{
    mName = name;
    mMinX = minx; mMinY = miny; mMinZ = minz;
    mMaxX = maxx; mMaxY = maxy; mMaxZ = maxz;
    if(_createGangZone())
    {
        mValid = true;
    }
    mCrewPtr = CrewManager::get().getCrew(mCrew);
}

GangZone::GangZone(const mongo::BSONObj& data) : GangZone()
{
    if(_loadDocument(data))
    {
        mValid = true;
    }
    mCrewPtr = CrewManager::get().getCrew(mCrew);
}

bool GangZone::setName(const std::string& name)
{
    if(_updateField("$set", "name", GBKToUTF8(name)))
    {
        mName = name;
        return true;
    }
    return false;
}

bool GangZone::setCrew(const mongo::OID& crewId)
{
    if(_updateField("$set", "crew", crewId))
    {
        mCrew = crewId;
        mCrewPtr = CrewManager::get().getCrew(mCrew);
        return true;
    }
    return false;
}

void GangZone::showForAll()
{
    GangZoneShowForAll(mInGameId, (mCrewPtr->getColor() << 8) + 0x44);
}

void GangZone::flashForAll()
{
    GangZoneFlashForAll(mInGameId, (mCrewPtr->getColor() << 8) + 0xFF);
}

bool GangZone::_createGangZone()
{
    MONGO_WRAPPER({
        mongo::OID id           = mongo::OID::gen();
        getDBConn()->insert(
            Config::colNameGangZone,
            BSON(
                "_id"   << id               <<
                "name"  << GBKToUTF8(mName) <<
                "crew"  << mCrew            <<
                "minx"  << mMinX            <<
                "miny"  << mMinY            <<
                "minz"  << mMinZ            <<
                "maxx"  << mMaxX            <<
                "maxy"  << mMaxY            <<
                "maxz"  << mMaxZ
                )
            );
        if(dbCheckError())
        {
            mId         = id;
            mInGameId   = GangZoneCreate(mMinX, mMinY, mMaxX, mMaxY);
            mArea.reset(new GangZoneBoxArea(this));
            LOG(INFO) << "Gang zone created: " << mName;
            return true;
        }
    });
    return false;
}

bool GangZone::_loadDocument(const mongo::BSONObj& doc)
{
    MONGO_WRAPPER({
        mId         = doc["_id"].OID();
        mName       = UTF8ToGBK(doc["name"].str());
        mCrew       = doc["crew"].OID();
        mMinX       = doc["minx"].numberDouble();
        mMinY       = doc["miny"].numberDouble();
        mMinZ       = doc["minz"].numberDouble();
        mMaxX       = doc["maxx"].numberDouble();
        mMaxY       = doc["maxy"].numberDouble();
        mMaxZ       = doc["maxz"].numberDouble();
        mInGameId   = GangZoneCreate(mMinX, mMinY, mMaxX, mMaxY);
        mArea.reset(new GangZoneBoxArea(this));
        return true;
    });
    return false;
}

GangZone::~GangZone()
{
    if(mInGameId != -1) GangZoneDestroy(mInGameId);
}

}
