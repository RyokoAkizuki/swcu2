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

#include <sampgdk/a_vehicles.h>

#include "../Streamer/Streamer.hpp"

#include "Items.hpp"

namespace swcu {

Object::Object(const mongo::BSONObj& data, int vworld)
{
    mWorld          = vworld;
    MONGO_WRAPPER({
        mID         = data["_id"].OID();
        mMap        = data["map"].OID();
        mModel      = data["model"].numberInt();
        mX          = data["x"].numberDouble();
        mY          = data["y"].numberDouble();
        mZ          = data["z"].numberDouble();
        mRX         = data["rx"].numberDouble();
        mRY         = data["ry"].numberDouble();
        mRZ         = data["rz"].numberDouble();
        mInterior   = data["interior"].numberInt();
        mEditable   = data["editable"].boolean();
        mInGameID = CreateDynamicObject(
            mModel, mX, mY, mZ, mRX, mRY, mRZ, mWorld, mInterior
        );
        if(mInGameID == 0)
        {
            LOG(ERROR) << "Error occurred while creating an object.";
        }
        return;
    });
    LOG(ERROR) << "Error occurred while loading an object.";
}

Object::Object(
    int model, float x, float y, float z,
    float rx, float ry, float rz, bool editable,
    const mongo::OID& map, int world, int interior
) : mMap(map), mModel(model), mX(x), mY(y), mZ(z),
    mRX(rx), mRY(ry), mRZ(rz),
    mWorld(world), mInterior(interior), mEditable(editable)
{
    mInGameID = CreateDynamicObject(
        mModel, mX, mY, mZ, mRX, mRY, mRZ, mWorld, mInterior
    );
    if(mInGameID == 0)
    {
        LOG(ERROR) << "Error occurred while creating object.";
    }
}

Object::~Object()
{
    DestroyDynamicObject(mInGameID);
}

bool Object::save()
{
    if(hasEntryInDatabase())
    {
        LOG(ERROR) << "Object " << mID.str() << " already has an entry in "
            "database.";
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->insert(
            Config::colNameMapObject,
            _buildDocument()
        );
        return dbCheckError();
    });
    return false;
}

mongo::BSONObj Object::_buildDocument()
{
    if(hasEntryInDatabase())
    {
        return mongo::BSONObj();
    }
    else
    {
        mID = mongo::OID::gen();
        return BSON(
            "_id"       << mID <<
            "map"       << mMap <<
            "model"     << mModel <<
            "x"         << mX <<
            "y"         << mY <<
            "z"         << mZ <<
            "rx"        << mRX <<
            "ry"        << mRY <<
            "rz"        << mRZ <<
            "interior"  << mInterior <<
            "editable"  << mEditable
        );
    }
}

bool Object::update()
{
    if(!hasEntryInDatabase())
    {
        LOG(ERROR) << "Object doesn't have an entry in database.";
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->update(
            Config::colNameMapObject,
            QUERY("_id" << mID),
            BSON("$set" << BSON(
                "x"     << mX <<
                "y"     << mY <<
                "z"     << mZ <<
                "rx"    << mRX <<
                "ry"    << mRY <<
                "rz"    << mRZ
            ))
        );
        return dbCheckError();
    });
    return false;
}

bool Object::changePose(float x, float y, float z,
    float rx, float ry, float rz)
{
    SetDynamicObjectPos(mInGameID, x, y, z);
    SetDynamicObjectRot(mInGameID, rx, ry, rz);
    mX = x; mY = y; mZ = z;
    mRX = rx; mRY = ry; mRZ = rz;
    return update();
}

bool Object::startEditing(int playerid)
{
    LOG(INFO) << "Player " << playerid << " starts to edit object "
        << mID.str();
    return EditDynamicObject(playerid, mInGameID);
}

void Object::updatePosition()
{
    GetDynamicObjectPos(mInGameID, mX, mY, mZ);
    GetDynamicObjectRot(mInGameID, mRX, mRY, mRZ);
    update();
}

LandscapeVehicle::LandscapeVehicle(
        const mongo::BSONObj& data, int vworld)
{
    mWorld              = vworld;
    MONGO_WRAPPER({
        mID             = data["_id"].OID();
        mMap            = data["map"].OID();
        mModel          = data["model"].numberInt();
        mX              = data["x"].numberDouble();
        mY              = data["y"].numberDouble();
        mZ              = data["z"].numberDouble();
        mAngle          = data["rotate"].numberDouble();
        mInterior       = data["interior"].numberInt();
        mRespawnDelay   = data["respawndelay"].numberInt();
        mInGameID = CreateVehicle(mModel, mX, mY, mZ, mAngle,
            // TO-DO: Random color or whatever.
            1, 1, mRespawnDelay);
        LOG(INFO) << "Vehicle loaded from database.";
        return;
    });
    LOG(ERROR) << "Error occured while loading a vehicle.";
}

LandscapeVehicle::LandscapeVehicle(
    int model, float x, float y, float z,
    float angle, const mongo::OID& map,
    int world, int interior,
    int respawndelay
) : mMap(map), mModel(model), mX(x), mY(y), mZ(z), mAngle(angle),
    mWorld(world), mInterior(interior), mRespawnDelay(respawndelay)
{
    mInGameID = CreateVehicle(mModel, mX, mY, mZ, mAngle,
        // TO-DO: Random color or whatever.
        1, 1, mRespawnDelay);
    respawn();
}

LandscapeVehicle::~LandscapeVehicle()
{
    DestroyVehicle(mInGameID);
}

bool LandscapeVehicle::save()
{
    if(hasEntryInDatabase())
    {
        LOG(ERROR) << "Vehicle " << mID.str() <<
            " already has an entry in database.";
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->insert(
            Config::colNameMapVehicle,
            _buildDocument()
        );
        return dbCheckError();
    });
    return false;
}

mongo::BSONObj LandscapeVehicle::_buildDocument()
{
    if(hasEntryInDatabase())
    {
        return mongo::BSONObj();
    }
    else
    {
        mID = mongo::OID::gen();
        return BSON(
            "_id"           << mID <<
            "map"           << mMap <<
            "model"         << mModel <<
            "x"             << mX <<
            "y"             << mY <<
            "z"             << mZ <<
            "rotate"        << mAngle <<
            "interior"      << mInterior <<
            "respawndelay"  << mRespawnDelay
        );
    }
}

void LandscapeVehicle::respawn()
{
    SetVehicleToRespawn(mInGameID);
    SetVehicleVirtualWorld(mInGameID, mWorld);
    LinkVehicleToInterior(mInGameID, mInterior);
}

void LandscapeVehicle::setPosition(float x, float y, float z,
    float angle, int interior)
{
    SetVehiclePos(mInGameID, x, y, z);
    SetVehicleZAngle(mInGameID, angle);
    LinkVehicleToInterior(mInGameID, mInterior);
    mX = x; mY = y; mZ = z;
    mAngle = angle; mInterior = interior;
}

}
