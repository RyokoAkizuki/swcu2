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
#include "../Player/Player.hpp"

namespace swcu {

enum CrewHierarchy
{
    LEADER          = 1,
    COMMISSIONERS   = 2,
    LIEUTENANTS     = 3,
    REPRESENTATIVES = 4,
    MUSCLE          = 5,
    PENDING         = 0,
    NOT_A_MEMBER    = -1
};

const char* getCrewHierarchyStr(CrewHierarchy hier);

class Crew : public StorableObject
{
    friend class CrewViewMembersDialog;
    friend class CrewEditMemberDialog;

protected:
    std::string     mName;
    mongo::OID      mLeader;
    std::string     mDescription;
    int64_t         mReputation;
    int32_t         mLevel;
    RGBAColor       mColor;

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
            std::string getColoredName() const
            { return mColor.getEmbedCode() + mName + "{FFFFFF}"; }
            mongo::OID  getLeader() const       { return mLeader; }
            std::string getDescription() const  { return mDescription; }
            int64_t     getReputation() const   { return mReputation; }
            int32_t     getLevel() const        { return mLevel; }
            RGBAColor   getColor() const        { return mColor; }
            bool        isMember(const mongo::OID& profileId);
            CrewHierarchy getMemberHierarchy(const mongo::OID& profileId);

            bool        setName(const std::string& name);
            bool        setLeader(const mongo::OID& profileId);
            bool        setDescription(const std::string& des);
            bool        increaseReputation(int64_t deltaRep);
            bool        increaseLevel(int32_t deltaLevel);
            bool        setColor(RGBAColor color);

            bool        applyToJoin(const mongo::OID& profileid);
            bool        approveToJoin(const mongo::OID& profileid);
            bool        denyToJoin(const mongo::OID& profileid);
            bool        addMember(const mongo::OID& profileid,
                CrewHierarchy hierarchy);
            bool        removeMember(const mongo::OID& profileid);
            bool        setMemberHierarchy(const mongo::OID& profileid,
                CrewHierarchy hierarchy);

protected:
    virtual bool        _parseObject(const mongo::BSONObj& doc);
};

}
