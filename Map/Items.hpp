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
    mongo::OID          mID, mMap;
    int                 mModel;
    float               mX, mY, mZ, mRX, mRY, mRZ;
    /**
     * Notice:
     * mWorld is not stored directly in a Object document. Instead, it's
     * an attribute of its parent map.
     */
    int                 mWorld, mInterior;
    bool                mEditable;

    int                 mInGameID;

    friend class Map;
    
public:
    /**
     * For loading an existing Object from database.
     */
                        Object(const mongo::BSONObj& data, int vworld);

    /**
     * For creating a new Object.
     */
                        Object(int model, float x, float y, float z,
        float rx, float ry, float rz, bool editable,
        const mongo::OID& map = mongo::OID(),
        int world = 0, int interior = -1);

    virtual             ~Object();

            mongo::OID  getMap() const
            { return mMap; }

            mongo::OID  getID() const
            { return mID; }

            int         getInGameID() const
            { return mInGameID; }

    /**
     * Save a newly created object to database.
     */
            bool        save();

    /**
     * This function is used when create a map which requires plenty of
     * insert operations in order to improve performance.
     * This will return an empty BSONObj when the Object holds an assigned
     * mID.
     */
            mongo::BSONObj _buildDocument();

    /**
     * Store a previously created object's info ( possibly has been changed
     * recently ) into database.
     */
            bool        update();

            bool        changePose(float x, float y, float z,
        float rx, float ry, float rz);

            bool        startEditing(int playerid);

            bool        hasEntryInDatabase() const
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

class LandscapeVehicle
{
    friend class Map;
    
protected:
    mongo::OID          mID, mMap;
    int                 mModel;
    float               mX, mY, mZ, mAngle;
    int                 mWorld, mInterior;
    int                 mRespawnDelay;
    int                 mInGameID;

public:
    /**
     * For loading an existing Object from database.
     */
                        LandscapeVehicle(
        const mongo::BSONObj& data, int vworld);

    /**
     * For creating a new Object.
     */
                        LandscapeVehicle(
        int model, float x, float y, float z,
        float angle, const mongo::OID& map = mongo::OID(),
        int world = 0, int interior = 0,
        int respawndelay = 60);

    virtual             ~LandscapeVehicle();

            mongo::OID  getID() const
            { return mID; }

            int         getInGameID() const
            { return mInGameID; }

            bool        save();
            mongo::BSONObj _buildDocument();

            mongo::OID  getMap() const
            { return mMap; }

            void        respawn();

            void        setPosition(float x, float y, float z,
        float angle, int interior);

            bool        hasEntryInDatabase()
            { return mID.isSet(); }
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
