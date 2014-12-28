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

#include "../Common/Common.hpp"

namespace swcu {

class Crew;

class GangZone
{
protected:
    mongo::OID      mId;
    std::string     mName;
    mongo::OID      mCrew;
    float           mMinX, mMinY, mMinZ, mMaxX, mMaxY, mMaxZ;

    int32_t         mInGameId;

    bool            mValid;

    std::shared_ptr<Crew>   mCrewPtr;

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

    virtual             ~GangZone();

            bool        isValid() const         { return mValid; }
            mongo::OID  getId() const           { return mId; }
            std::string getName() const         { return mName; }
            mongo::OID  getCrew() const         { return mCrew; }

            int32_t     getInGameId() const     { return mInGameId; }

            bool        setName(const std::string& name);
            bool        setCrew(const mongo::OID& crewId);

            void        showForAll();
            void        flashForAll();

protected:
            bool        _createGangZone();
            bool        _loadDocument(const mongo::BSONObj& doc);

    template<typename T>
            bool        _updateField(const char* operation,
                const char* fieldname, T value)
    {
        if(!isValid())
        {
            LOG(ERROR) << "Invalid gang zone.";
            return false;
        }
        MONGO_WRAPPER({
            getDBConn()->update(
                Config::colNameGangZone,
                BSON("_id" << mId),
                BSON(operation << BSON(fieldname << value))
                );
            return dbCheckError();
        });
        return false;
    }
};

}
