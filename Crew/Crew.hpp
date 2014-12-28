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

class Crew
{
protected:
    mongo::OID      mId;
    std::string     mName;
    mongo::OID      mLeader;
    time_t          mCreateTime;
    std::string     mDescription;
    int64_t         mScore;
    int32_t         mLevel;
    // RGB
    int32_t         mColor;

    // Identity for SA-MP
    int32_t         mTeamId;

    bool            mValid;

protected:
                        Crew();

public:
    /**
     * Load a crew. If fails then create a crew.
     */
                        Crew(const std::string& name);
    /**
     * Load a crew.
     */
                        Crew(const mongo::OID& id);
    virtual             ~Crew() {}

            bool        isValid() const         { return mValid; }
            mongo::OID  getId() const           { return mId; }
            std::string getName() const         { return mName; }
            mongo::OID  getLeader() const       { return mLeader; }
            time_t      getCreateTime() const   { return mCreateTime; }
            std::string getDescription() const  { return mDescription; }
            int64_t     getScore() const        { return mScore; }
            int32_t     getLevel() const        { return mLevel; }
            int32_t     getColor() const        { return mColor; }

            int32_t     getSAMPTeamId() const   { return mTeamId; }

            bool        setName(const std::string& name);
            bool        setLeader(const mongo::OID& profileId);
            bool        setDescription(const std::string& des);
            bool        increaseScore(int64_t deltaScore);
            bool        increaseLevel(int32_t deltaLevel);
            bool        setColor(int32_t color);

protected:
            bool        _createCrew();
            bool        _findAndLoadDocumentByName();
            bool        _findAndLoadDocumentById();
            bool        _loadDocument(const mongo::BSONObj& doc);

    template<typename T>
            bool        _updateField(const char* operation,
                const char* fieldname, T value)
    {
        if(!isValid())
        {
            LOG(ERROR) << "Invalid crew.";
            return false;
        }
        MONGO_WRAPPER({
            getDBConn()->update(
                Config::colNameCrew,
                BSON("_id" << mId),
                BSON(operation << BSON(fieldname << value))
                );
            return dbCheckError();
        });
        return false;
    }
};

}
