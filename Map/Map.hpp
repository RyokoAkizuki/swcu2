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

#include <vector>
#include <memory>
#include <kanko/Common/Vector3.hpp>
#include <kanko/Objects/BBox.hpp>

#include "../Common/StorableObject.hpp"
#include "../Area/Area.hpp"
#include "Items.hpp"

namespace swcu {

enum VirtualWorld
{
    WORLD_MAIN      = 0,
    WORLD_PRISON    = 1,
    WORLD_STUNT     = 2,
    WORLD_MAX       = 2147483647
};

class Map;

class HouseMapArea : public SphereArea
{
protected:
    Map*            mMap;

public:
                    HouseMapArea(Map* map);
    virtual         ~HouseMapArea() {}
    virtual void    onEnter(int playerid);
    virtual void    onLeave(int playerid);
};

class PrisonMapArea : public SphereArea
{
protected:
    Map*            mMap;

public:
                    PrisonMapArea(Map* map);
    virtual         ~PrisonMapArea() {}
    virtual void    onEnter(int playerid);
    virtual void    onLeave(int playerid);
};

enum MapType
{
    LANDSCAPE   = 0,
    DEATHMATCH  = 1,
    STUNT       = 2,
    PROPERTY    = 3,
    BUSINESS    = 4,
    PRISON      = 5
};

class Map : public StorableObject
{
    friend class HouseMapArea;
    friend class PrisonMapArea;

protected:
    /**
     * If the id is not set, the map is not saved yet.
     */
    MapType             mType;
    std::string         mName;
    mongo::OID          mOwner;
    bool                mActivated;
    int                 mVirtualWorld;
    kanko::Vector3      mBoundSphereCenter;
    float               mBoundSphereRadius;
    kanko::BBox         mBBox;
    float               mVariance;

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
        const mongo::OID& owner, const std::string& name);
    /**
     * Find and load a map by its name.
     */
                        Map(const std::string& name);
    /**
     * Load map from provided data document.
     */
                        Map(const mongo::BSONObj& data);
    virtual             ~Map() {}
            std::string getName() const         { return mName; }
            std::string getTypeStr() const;
            bool        isActivated() const     { return mActivated; }
            size_t      getObjectCount() const  { return mObjects.size(); }
            size_t      getVehicleCount() const { return mVehicles.size(); }
            bool        addObject(int model, float x, float y, float z,
        float rx, float ry, float rz, bool editable, int interior);
            bool        addVehicle(int model, float x, float y, float z,
        float angle, int interior, int respawndelay);
            bool        setWorld(int world);
            bool        setOwner(const mongo::OID& owner);
            mongo::OID  getOwner() const
            { return mOwner; }
            std::string getJSON() const;
            bool        deleteFromDatabase();
            bool        updateBounding();

protected:
    virtual bool        _parseObject(const mongo::BSONObj& data);
            bool        _calculateBoundingSphere();
            bool        _calculateBoundingBox();
};

}
