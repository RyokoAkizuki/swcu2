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
#include <algorithm>

#include "../Streamer/Streamer.hpp"

#include "Items.hpp"

namespace swcu {

Object::Object(
    int model, float x, float y, float z,
    float rx, float ry, float rz, int world, int interior,
    const mongo::OID& id,
    const mongo::OID& parentMap,
    const mongo::OID& owner
) : mModel(model), mX(x), mY(y), mZ(z), mRX(rx), mRY(ry), mRZ(rz),
    mWorld(world), mInterior(interior), mID(id), mParentMap(parentMap),
    mOwner(owner)
{
    mInGameID = CreateDynamicObject(
        mModel, mX, mY, mZ, mRX, mRY, mRZ, mWorld, mInterior
    );
}

bool Object::save()
{
    MONGO_WRAPPER({
        mongo::OID id = mID.isSet() ? mID : mongo::OID::gen();
        getDBConn()->update(
            Config::colNameMapObject,
            QUERY("_id" << id),
            BSON("$set" << BSON(
                "_id"       << id <<
                "model"     << mModel <<
                "x"         << mX <<
                "y"         << mY <<
                "z"         << mZ <<
                "rx"        << mRX <<
                "ry"        << mRY <<
                "rz"        << mRZ <<
                "world"     << mWorld <<
                "interior"  << mInterior <<
                "parentmap" << mParentMap <<
                "owner"     << mOwner
            )),
            true /* upsert */
        );
        mID = id;
        return true;
    });
    return false;
}

bool Object::setOwner(const mongo::OID& owner)
{
    if(!hasEntryInDatabase())
    {
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->update(
            Config::colNameMapObject,
            QUERY("_id" << mID),
            BSON("$set" << BSON(
                "owner" << owner
            ))
        );
        mOwner = owner;
        return true;
    });
    return false;
}

void Object::changePose(float x, float y, float z,
    float rx, float ry, float rz)
{
    SetDynamicObjectPos(mInGameID, x, y, z);
    SetDynamicObjectRot(mInGameID, rx, ry, rz);
    mX = x; mY = y; mZ = z;
    mRX = rx; mRY = ry; mRZ = rz;
}

bool Object::startEditing(int playerid)
{
    return EditDynamicObject(playerid, mInGameID);
}

void Object::updatePosition()
{
    GetDynamicObjectPos(mInGameID, mX, mY, mZ);
    GetDynamicObjectRot(mInGameID, mRX, mRY, mRZ);
}

Vehicle::Vehicle(
    int model, float x, float y, float z,
    float angle, int color1, int color2,
    int world, int interior,
    int respawndelay,
    int paintjob,
    std::vector<int> components,
    const mongo::OID& id,
    const mongo::OID& parentMap,
    const mongo::OID& owner
) : mModel(model), mX(x), mY(y), mZ(z), mAngle(angle),
    mColor1(color1), mColor2(color2), mRespawnDelay(respawndelay),
    mPaintjob(paintjob), mComponents(components), mID(id),
    mParentMap(parentMap), mOwner(owner)
{
    mInGameID = CreateVehicle(mModel, mX, mY, mZ, mAngle,
        mColor1, mColor2, mRespawnDelay);
    respawn();
}

bool Vehicle::save()
{
    MONGO_WRAPPER({
        mongo::OID id = mID.isSet() ? mID : mongo::OID::gen();
        getDBConn()->update(
            Config::colNameMapVehicle,
            QUERY("_id" << id),
            BSON("$set" << BSON(
                "_id"           << id <<
                "model"         << mModel <<
                "x"             << mX <<
                "y"             << mY <<
                "z"             << mZ <<
                "rotate"        << mAngle <<
                "color1"        << mColor1 <<
                "color2"        << mColor2 <<
                "world"         << mWorld <<
                "interior"      << mInterior <<
                "respawndelay"  << mRespawnDelay <<
                "paintjob"      << mPaintjob <<
                "components"    << mComponents <<
                "parentmap"     << mParentMap <<
                "owner"         << mOwner
            )),
            true /* upsert */
        );
        mID = id;
        return true;
    });
    return false;
}

bool Vehicle::setOwner(const mongo::OID& owner)
{
    if(!hasEntryInDatabase())
    {
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->update(
            Config::colNameMapVehicle,
            QUERY("_id" << mID),
            BSON("$set" << BSON(
                "owner" << owner
            ))
        );
        mOwner = owner;
        return true;
    });
    return false;
}

void Vehicle::addComponent(int component)
{
    AddVehicleComponent(mInGameID, component);
    mComponents.push_back(component);
}

void Vehicle::removeComponent(int component)
{
    RemoveVehicleComponent(mInGameID, component);
    mComponents.erase(std::remove_if(
        mComponents.begin(), mComponents.end(),
        [component](int com) { return com == component; }
    ));
}

void Vehicle::setPaintjob(int paintjob)
{
    ChangeVehiclePaintjob(mInGameID, paintjob);
}

void Vehicle::removePaintjob()
{
    ChangeVehiclePaintjob(mInGameID, 3);
}

void Vehicle::respawn()
{
    SetVehicleToRespawn(mInGameID);
    SetVehicleVirtualWorld(mInGameID, mWorld);
    LinkVehicleToInterior(mInGameID, mInterior);
    ChangeVehiclePaintjob(mInGameID, mPaintjob);
    for(auto i : mComponents)
    {
        AddVehicleComponent(mInGameID, i);
    }
}

void Vehicle::setPosition(float x, float y, float z,
    float angle, int world, int interior)
{
    SetVehiclePos(mInGameID, x, y, z);
    SetVehicleZAngle(mInGameID, angle);
    SetVehicleVirtualWorld(mInGameID, mWorld);
    LinkVehicleToInterior(mInGameID, mInterior);
    mX = x; mY = y; mZ = z;
    mAngle = angle; mWorld = world; mInterior = interior;
}

void Vehicle::updatePosition()
{
    mWorld = GetVehicleVirtualWorld(mInGameID);
    GetVehiclePos(mInGameID, &mX, &mY, &mZ);
    GetVehicleZAngle(mInGameID, &mAngle);
}

}
