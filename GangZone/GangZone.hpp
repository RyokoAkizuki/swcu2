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

#pragma once

#include "../Common/StorableObject.hpp"

#include "../Area/Area.hpp"

namespace swcu {

class Crew;
class GangZone;

class GangZoneBoxArea : public BoxArea
{
protected:
    GangZone*       mZone;

public:
                    GangZoneBoxArea(GangZone* zone);
    virtual         ~GangZoneBoxArea() {}

    virtual void    onEnter(int playerid);
    virtual void    onLeave(int playerid);
};

class GangZone : public StorableObject
{
    friend class GangZoneBoxArea;

protected:
    std::string     mName;
    mongo::OID      mCrew;
    float           mMinX, mMinY, mMinZ, mMaxX, mMaxY, mMaxZ;

    int32_t         mInGameId;

    std::shared_ptr<Crew>               mCrewPtr;
    std::unique_ptr<GangZoneBoxArea>    mArea;

    /**
     * Gang War.
     */
    time_t          mGangWarEndTime;
    uint64_t        mCrewDeath, mEnemyDeath;
    mongo::OID      mEnemyCrew;
    bool            mInWar;

protected:
                        GangZone();

public:
    /**
     * Create a gang zone.
     */
                        GangZone(
        const std::string& name,
        float minx, float miny, float minz,
        float maxx, float maxy, float maxz
    );

    /**
     * Load a gang zone.
     */
                        GangZone(const mongo::BSONObj& data);
                        GangZone(const std::string& name);

    virtual             ~GangZone();

            std::string getName() const         { return mName; }
            mongo::OID  getCrew() const         { return mCrew; }

            int32_t     getInGameId() const     { return mInGameId; }

            bool        setName(const std::string& name);
            bool        setCrew(const mongo::OID& crewId);

            bool        startGangWar(const mongo::OID& enemyCrewId);
            
            bool        isInWar() const
            { return mInWar; }

            bool        isWarExpired() const
            { return mGangWarEndTime < time(0); }

            mongo::OID  getEnemyCrew() const
            { return mEnemyCrew; }

            void        onCrewDeath();
            void        onEnemyDeath();
            bool        stopGangWar();
            void        updateWarStatus();
            void        updatePlayersHUD();

protected:
            void        _broadcastWarProgress();
    virtual bool        _parseObject(const mongo::BSONObj& data);
};

}
