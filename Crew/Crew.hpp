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

#include "../Common/StorableObject.hpp"
#include "../Common/RGBAColor.hpp"

namespace swcu {

class Crew : public StorableObject
{
protected:
    std::string     mName;
    mongo::OID      mLeader;
    std::string     mDescription;
    int64_t         mScore;
    int32_t         mLevel;
    RGBAColor       mColor;

    // Identity for SA-MP
    int32_t         mTeamId;

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

            std::string getName() const         { return mName; }
            mongo::OID  getLeader() const       { return mLeader; }
            std::string getDescription() const  { return mDescription; }
            int64_t     getScore() const        { return mScore; }
            int32_t     getLevel() const        { return mLevel; }
            RGBAColor   getColor() const        { return mColor; }

            int32_t     getSAMPTeamId() const   { return mTeamId; }

            bool        setName(const std::string& name);
            bool        setLeader(const mongo::OID& profileId);
            bool        setDescription(const std::string& des);
            bool        increaseScore(int64_t deltaScore);
            bool        increaseLevel(int32_t deltaLevel);
            bool        setColor(RGBAColor color);

protected:
    virtual bool        _parseObject(const mongo::BSONObj& doc);
            bool        _createCrew();
};

}
