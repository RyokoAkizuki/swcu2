/*
 * Copyright 2015 Yukino Hayakawa<tennencoll@gmail.com>
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

#include "StorableObject.hpp"

namespace swcu {

StorableObject::StorableObject(
    const std::string& collection,
    const mongo::OID& oid
) : mCollection(collection), mId(oid), mValid(false)
{
}

StorableObject::StorableObject(const std::string& collection) :
    mCollection(collection), mValid(false)
{
}

bool StorableObject::_createObject(const mongo::BSONObj& data)
{
    if(isValid())
    {
        LOG(ERROR) << "Are you going to overwrite it?";
        return false;
    }
    MONGO_WRAPPER({
        mongo::OID id = mongo::OID::gen();
        mongo::BSONObjBuilder b;
        b.append("_id", id).appendElements(data);
        getDBConn()->insert(mCollection, b.obj());
        if(dbCheckError())
        {
            mId     = id;
            mValid  = true;
            return true;
        }
    });
    return false;
}

bool StorableObject::_updateObject(const mongo::BSONObj& data)
{
    if(!isValid())
    {
        LOG(ERROR) << "You won't find this document.";
        return false;
    }
    MONGO_WRAPPER({
        getDBConn()->update(mCollection, QUERY("_id" << mId), data);
        return dbCheckError();
    });
    return false;
}

EventLog::EventLog(const std::string& subsystem, const std::string& event,
    const mongo::BSONObj& data) : StorableObject(Config::colNameEventLog)
{
    _createObject(BSON(
        "subsystem" << subsystem            <<
        "event"     << event                <<
        "time"      << mongo::DATENOW       <<
        "data"      << data
    ));
}
}
