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

#include <sampgdk/a_vehicles.h>

#include "../Streamer/Streamer.hpp"

#include "Items.hpp"

namespace swcu {

bool Object::_parseObject(const mongo::BSONObj& data)
{
    MONGO_WRAPPER({
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
        return true;
    });
    return false;
}

bool Object::_createDynamicObject()
{
    mInGameID   = CreateDynamicObject(
        mModel, mX, mY, mZ, mRX, mRY, mRZ, mWorld, mInterior, -1, 300.0
    );
    if(mInGameID == 0)
    {
        LOG(ERROR) << "Error occurred while creating an object.";
        return false;
    }
    return true;
}

Object::Object(const mongo::BSONObj& data, int vworld) :
    StorableObject(Config::colNameMapObject), mInGameID(0)
{
    mWorld          = vworld;
    if(_parseObject(data) && _createDynamicObject())
    {
        mValid      = true;
    }
}

Object::Object(
    int model, float x, float y, float z,
    float rx, float ry, float rz, bool editable,
    const mongo::OID& map, int world, int interior
) : StorableObject(Config::colNameMapObject),
    mMap(map), mModel(model), mX(x), mY(y), mZ(z),
    mRX(rx), mRY(ry), mRZ(rz),
    mWorld(world), mInterior(interior), mEditable(editable)
{
    if(_createObject(_buildDocument()))
    {
        _createDynamicObject();
    }
}

Object::~Object()
{
    if(mInGameID != 0) DestroyDynamicObject(mInGameID);
}

mongo::BSONObj Object::_buildDocument()
{
    return BSON(
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

bool Object::changePose(float x, float y, float z,
    float rx, float ry, float rz)
{
    if(_updateObject(BSON("$set" << BSON(
        "x"     << x <<
        "y"     << y <<
        "z"     << z <<
        "rx"    << rx <<
        "ry"    << ry <<
        "rz"    << rz
    ))))
    {
        SetDynamicObjectPos(mInGameID, x, y, z);
        SetDynamicObjectRot(mInGameID, rx, ry, rz);
        mX = x; mY = y; mZ = z;
        mRX = rx; mRY = ry; mRZ = rz;
        return true;
    }
    return false;
}

bool Object::startEditing(int playerid)
{
    if(!mEditable) return false;
    LOG(INFO) << "Player " << playerid << " starts to edit object "
        << mId.str();
    return EditDynamicObject(playerid, mInGameID);
}

bool LandscapeVehicle::_parseObject(const mongo::BSONObj& data)
{
    MONGO_WRAPPER({
        mMap            = data["map"].OID();
        mModel          = data["model"].numberInt();
        mX              = data["x"].numberDouble();
        mY              = data["y"].numberDouble();
        mZ              = data["z"].numberDouble();
        mAngle          = data["rotate"].numberDouble();
        mInterior       = data["interior"].numberInt();
        mRespawnDelay   = data["respawndelay"].numberInt();
        return true;
    });
    return false;
}

bool LandscapeVehicle::_createVehicle()
{
    mInGameID = CreateVehicle(mModel, mX, mY, mZ, mAngle,
        rand() % 256, rand() % 256, mRespawnDelay);
    if(mInGameID == INVALID_VEHICLE_ID)
    {
        LOG(ERROR) << "Error occurred while creating a vehicle.";
        return false;
    }
    return true;
}

LandscapeVehicle::LandscapeVehicle(const mongo::BSONObj& data, int vworld)
    : StorableObject(Config::colNameMapVehicle),
    mInGameID(INVALID_VEHICLE_ID)
{
    mWorld          = vworld;
    if(_parseObject(data) && _createVehicle())
    {
        mValid      = true;
    }
}

LandscapeVehicle::LandscapeVehicle(
    int model, float x, float y, float z,
    float angle, const mongo::OID& map,
    int world, int interior,
    int respawndelay
) : StorableObject(Config::colNameMapVehicle),
    mMap(map), mModel(model), mX(x), mY(y), mZ(z), mAngle(angle),
    mWorld(world), mInterior(interior), mRespawnDelay(respawndelay)
{
    if(_createObject(_buildDocument()))
    {
        _createVehicle();
    }
}

LandscapeVehicle::~LandscapeVehicle()
{
    DestroyVehicle(mInGameID);
}

mongo::BSONObj LandscapeVehicle::_buildDocument()
{
    return BSON(
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

void LandscapeVehicle::respawn()
{
    SetVehicleToRespawn(mInGameID);
    SetVehicleVirtualWorld(mInGameID, mWorld);
    LinkVehicleToInterior(mInGameID, mInterior);
}

}
