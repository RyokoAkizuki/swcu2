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

#include "../Common/Common.hpp"

namespace swcu {

class Object
{
protected:
    int                 mModel;
    float               mX, mY, mZ, mRX, mRY, mRZ;
    int                 mWorld, mInterior;
    mongo::OID          mID, mParentMap, mOwner;
    int                 mInGameID;

public:
                        Object(
        int model, float x, float y, float z,
        float rx, float ry, float rz, int world = -1, int interior = -1,
        const mongo::OID& id = mongo::OID(),
        const mongo::OID& parentMap = mongo::OID(),
        const mongo::OID& owner = mongo::OID());

    virtual             ~Object() {}

            mongo::OID  getParentMap() const
            { return mParentMap; }

            mongo::OID  getID() const
            { return mID; }

            int         getInGameID() const
            { return mInGameID; }

            bool        save();

            mongo::OID  getOwner() const
            { return mOwner; }

            bool        setOwner(const mongo::OID& owner);

            void        changePose(float x, float y, float z,
        float rx, float ry, float rz);

            bool        startEditing(int playerid);

            bool        hasEntryInDatabase()
            { return mID.isSet(); }

    /**
     * Refetch object's position data including coordinates,
     * virtual world and interior.
     * Should be called when OnPlayerEditDynamicObject or
     * OnPlayerEditDynamicObject occured.
     * @return Nothing.
     */
            void        updatePosition();
};

class FakeObject : public Object
{
public:
                        FakeObject() :
        Object(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) {}
    virtual             ~FakeObject() {}
};

class Vehicle
{
protected:
    int                 mModel;
    float               mX, mY, mZ, mAngle;
    int                 mWorld, mInterior;
    int                 mColor1, mColor2;
    int                 mRespawnDelay;
    int                 mPaintjob;
    std::vector<int>    mComponents;
    mongo::OID          mID, mParentMap, mOwner;
    int                 mInGameID;

public:
                        Vehicle(
        int model, float x, float y, float z,
        float angle, int color1 = 0, int color2 = 1,
        int world = -1, int interior = -1,
        int respawndelay = 60,
        int paintjob = 3 /* Remove all paintjobs */,
        std::vector<int> components = {},
        const mongo::OID& id = mongo::OID(),
        const mongo::OID& parentMap = mongo::OID(),
        const mongo::OID& owner = mongo::OID());

    virtual             ~Vehicle() {}

            mongo::OID  getID() const
            { return mID; }

            int         getInGameID() const
            { return mInGameID; }

            bool        save();

            mongo::OID  getParentMap() const
            { return mParentMap; }

            mongo::OID  getOwner() const
            { return mOwner; }

            bool        setOwner(const mongo::OID& owner);

            void        addComponent(int component);
            void        removeComponent(int component);

            void        setPaintjob(int paintjob);
            void        removePaintjob();

            void        respawn();

            void        setPosition(float x, float y, float z,
        float angle, int world, int interior);

            void        updatePosition();

            bool        hasEntryInDatabase()
            { return mID.isSet(); }
};

class FakeVehicle : public Vehicle
{
public:
                        FakeVehicle() :
        Vehicle(0, 0.0, 0.0, 0.0, 0.0, 0.0) {}
    virtual             ~FakeVehicle() {}
};

/*
class MapIcon
{
protected:
    int                 mType, mColor;
    int                 mWorld, mInterior;
    float               mX, mY, mZ;
    mongo::OID          mID, mParentMap;
    int                 mInGameID;

public:
                        MapIcon(
        float x, float y, float z, int type, int color,
        int world = -1, int interior = -1,
        const mongo::OID& id = mongo::OID(),
        const mongo::OID& parentMap = mongo::OID());

    virtual             ~MapIcon() {}

            mongo::OID  getID() const
            { return mID; }

            int         getInGameID() const
            { return mInGameID; }

            bool        save();

            mongo::OID  getParentMap() const
            { return mParentMap; }
};
*/

}
