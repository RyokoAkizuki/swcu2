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
#include "../Player/PlayerManager.hpp"
#include "GangZoneManager.hpp"

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
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p != nullptr)
    {
        p->_setCurrentGangZone(
            GangZoneManager::get().getGangZone(mZone->mInGameId)
        );
    }
}

void GangZoneBoxArea::onLeave(int playerid)
{
    SendClientMessage(playerid, 0xFFFFFFFF, CSTR(
        "你离开了" << mZone->getName() << "."
    ));
    auto p = PlayerManager::get().getPlayer(playerid);
    // If CurrentGangZone != mZone, it might be overwritten by other zones.
    if(p != nullptr && p->_getCurrentGangZone().get() == mZone)
    {
        p->_clearCurrentGangZone();
    }
}

GangZone::GangZone() : mInGameId(-1), mValid(false),
    mGangWarEndTime(0), mCrewDeath(0), mEnemyDeath(0), mInWar(false)
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

GangZone::GangZone(const std::string& name) : GangZone()
{
    if(_findAndLoadByName(name))
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
        if(mCrew != mCrewPtr->getId())
        {
            LOG(WARNING) << "mCrew != mCrewPtr.getId()";
        }
        return true;
    }
    return false;
}

void GangZone::updatePlayersHUD()
{
    auto c = mCrewPtr->getColor();
    c.setAlpha(static_cast<int8_t>(0x80));
    GangZoneShowForAll(mInGameId, c.getRGBA());
    if(isInWar())
    {
        GangZoneFlashForAll(mInGameId, 0xFF000080 /* RED */);
    }
    else
    {
        GangZoneStopFlashForAll(mInGameId);
    }
}

bool GangZone::startGangWar(const mongo::OID& enemyCrewId)
{
    if(mInWar)
    {
        LOG(WARNING) << "Previous gang war still in progress."
            " Can't start a new one.";
        return false;
    }
    // A gang war lasts 10 mins.
    mGangWarEndTime = time(0) + 10 * 60;
    // Reset counters.
    mCrewDeath = 0; mEnemyDeath = 0;
    mEnemyCrew = enemyCrewId;
    mInWar = true;
    updatePlayersHUD();
    return true;
}

void GangZone::onCrewDeath()
{
    if(!isInWar()) return;
    ++mCrewDeath;
    _broadcastWarProgress();
}

void GangZone::onEnemyDeath()
{
    if(!isInWar()) return;
    ++mEnemyDeath;
    _broadcastWarProgress();
}

void GangZone::_broadcastWarProgress()
{
    if(!isInWar()) return;
    SendClientMessageToAll(0xFFFFFFFF, CSTR(
        mName << " 上的地盘争夺战战况 - 攻 " << mCrewDeath
        << " : 守 " <<  mEnemyDeath
    ));
}

bool GangZone::stopGangWar()
{
    if(!mInWar)
    {
        return false;
    }
    if(mCrewDeath > mEnemyDeath)
    {
        setCrew(mEnemyCrew);
        SendClientMessageToAll(0xFFFFFFFF, CSTR(
            mName << " 被 " << mCrewPtr->getName() << " 占领."
        ));
    }
    else
    {
        SendClientMessageToAll(0xFFFFFFFF, CSTR(
            mCrewPtr->getName() << " 守住了 " << mName << "."
        ));
    }
    mGangWarEndTime = 0;
    mCrewDeath = 0; mEnemyDeath = 0;
    mEnemyCrew = mongo::OID();
    mInWar = false;
    updatePlayersHUD();
    return true;
}

void GangZone::updateWarStatus()
{
    if(isInWar() && isWarExpired())
    {
        stopGangWar();
    }
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

bool GangZone::_findAndLoadByName(const std::string& name)
{
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(
            Config::colNameGangZone,
            QUERY("name" << GBKToUTF8(name))
        );
        if(doc.isEmpty())
        {
            LOG(WARNING) << "Specified gang zone (" << name
                << ") not found. This is going to be an invalid object.";
            return false;
        }
        else
        {
            return _loadDocument(doc);
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
