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

#include <vector>
#include <memory>

#include "../Common/Common.hpp"
#include "../Area/Area.hpp"
#include "Items.hpp"

namespace swcu {

class HouseMapArea : public SphereArea
{
public:
                    HouseMapArea(float x, float y, float z, float radius,
        int virtualworld = -1, int interior = -1, int playerid = -1);
    virtual         ~HouseMapArea() {}

    virtual void    onEnter(int playerid);
    virtual void    onLeave(int playerid);
};

class PrisonMapArea : public SphereArea
{
public:
                    PrisonMapArea(float x, float y, float z, float radius,
        int virtualworld = -1, int interior = -1, int playerid = -1);
    virtual         ~PrisonMapArea() {}

    virtual void    onEnter(int playerid);
    virtual void    onLeave(int playerid);
};

enum MapType
{
    LANDSCAPE   = 0,
    DEATHMATCH  = 1,
    STUNT       = 2,
    HOUSE       = 3,
    PROPERTY    = 4,
    PRISON      = 5
};

class Map
{
protected:
    /**
     * If the id is not set, the map is not saved yet.
     */
    mongo::OID          mId;
    MapType             mType;
    std::string         mName;
    mongo::OID          mOwner;
    time_t              mAddTime;
    bool                mActivated;
    int                 mVirtualWorld;
    double              mArea;
    float               mBoundX, mBoundY, mBoundZ, mBoundRadius;

    bool                mSaved;

    std::unique_ptr<Area>                           mBoundingArea;

    std::vector<std::unique_ptr<Object>>            mObjects;
    std::vector<std::unique_ptr<LandscapeVehicle>>  mVehicles;

protected:
                        Map();

public:
    /**
     * Initialize a new map.
     */
                        Map(MapType type, int world,
        const mongo::OID& owner);

    /**
     * Find and load a map by its name.
     */
                        Map(const std::string& name);

    /**
     * Load map from provided data document.
     */
                        Map(const mongo::BSONObj& data);

    virtual             ~Map() {}

            mongo::OID  getId() const
            { return mId; }

            std::string getName() const
            { return mName; }

            bool        isActivated() const
            { return mActivated; }

            size_t      getObjectCount() const
            { return mObjects.size(); }

            size_t      getVehicleCount() const
            { return mVehicles.size(); }

            bool        hasEntryInDatabase()
            { return mSaved; }

            bool        addObject(int model, float x, float y, float z,
        float rx, float ry, float rz, bool editable, int interior);
            bool        addVehicle(int model, float x, float y, float z,
        float angle, int interior, int respawndelay);
            bool        setWorld(int world);
            bool        setOwner(const mongo::OID& owner);
            bool        reload();
            bool        save(const std::string& name);

    /**
     * Calculate a bounding sphere which covers all positions of objects.
     * Normally, this should only be done when a map's responsibility requires
     * a bounding sphere, such as a property map, which detects player
     * entrances to trigger property management functions, and a deathmatch
     * map, which uses a bounding sphere to ensure players be within the
     * battlefield. When a new object is added to map, the bounding sphere
     * will not be updated immediately by default in case of a waste of
     * computation when creating a new map. So you may call this manually.
     * @return  True if bouding sphere is successfully updated and false
     *          if the map doesn't have an entry in database or database
     *          operation failed.
     */
            bool        calculateBoundingSphere();

protected:
            bool        _loadMap(const mongo::BSONObj& data);
            void        _createBoundingSphereArea();
};

}
