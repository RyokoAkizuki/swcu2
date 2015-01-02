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

#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>

#include "../Common/RGBAColor.hpp"
#include "../Event/Event.hpp"

#include "Crew.hpp"

namespace swcu {

const char* getCrewHierarchyStr(CrewHierarchy hier)
{
    switch(hier)
    {
        case LEADER:            return "帮会领袖";
        case COMMISSIONERS:     return "堂主    ";
        case LIEUTENANTS:       return "干部    ";
        case REPRESENTATIVES:   return "街头代表";
        case MUSCLE:            return "打手    ";
        case PENDING:           return "[待审核]";
        case NOT_A_MEMBER:      return "[非成员]";
        default:                return "???";
    }
    return "???";
}

Crew::Crew() : StorableObject(Config::colNameCrew),
    mReputation(0), mLevel(0)
{
}

Crew::Crew(const std::string& name) : Crew()
{
    mName = name;
    if(!_loadObject("name", GBKToUTF8(name)))
    {
        if(_createObject(BSON(
            "name"          << GBKToUTF8(mName)         <<
            "leader"        << mLeader                  <<
            "description"   << GBKToUTF8(mDescription)  <<
            "reputation"    << mReputation              <<
            "level"         << mLevel                   <<
            "color"         << mColor.getRGB()          <<
            "members"       << mongo::BSONObj()
        )))
        {
            LOG(INFO) << "Crew created: " << mName;
        }
    }
}

Crew::Crew(const mongo::OID& id) : Crew()
{
    mId = id;
    _loadObject();
}

bool Crew::setName(const std::string& name)
{
    if(_updateField("$set", "name", GBKToUTF8(name)))
    {
        mName = name;
        EventManager::get().sendEvent(onCrewNameChanged, this);
        return true;
    }
    return false;
}

bool Crew::setLeader(const mongo::OID& profileId)
{
    if(_updateField("$set", "leader", profileId))
    {
        mLeader = profileId;
        EventManager::get().sendEvent(onCrewLeaderChanged, this);
        return true;
    }
    return false;
}

bool Crew::setDescription(const std::string& des)
{
    if(_updateField("$set", "description", GBKToUTF8(des)))
    {
        mDescription = des;
        return true;
    }
    return false;
}

bool Crew::increaseReputation(int64_t deltaRep)
{
    if(_updateField("$inc", "reputation", deltaRep))
    {
        mReputation += deltaRep;
        return true;
    }
    return false;
}

bool Crew::increaseLevel(int32_t deltaLevel)
{
    if(_updateField("$inc", "level", deltaLevel))
    {
        mLevel += deltaLevel;
        return true;
    }
    return false;
}

bool Crew::setColor(RGBAColor color)
{
    if(_updateField("$set", "color", color.getRGB()))
    {
        mColor = color;
        EventManager::get().sendEvent(onCrewColorChanged, this);
        return true;
    }
    return false;
}

bool Crew::applyToJoin(const mongo::OID& profileid)
{
    if(isMember(profileid)) return false;
    std::string fieldname = "members." + profileid.str();
    if(_conditionedUpdate(
        BSON(fieldname << BSON("$exists" << false)),
        BSON("$set" << BSON(fieldname << PENDING))
    ))
    {
        EventManager::get().sendEvent(
            onCrewPlayerApplyToJoin, this, profileid);
        return true;
    }
    return false;
}

bool Crew::approveToJoin(const mongo::OID& profileid)
{
    std::string fieldname = "members." + profileid.str();
    if(_conditionedUpdate(
        BSON(fieldname << BSON("$exists" << true)),
        BSON("$set" << BSON(fieldname << MUSCLE))
    ))
    {
        EventManager::get().sendEvent(
            onCrewPlayerApprovedToJoin, this, profileid);
        EventManager::get().sendEvent(
            onCrewMemberAdded, this, profileid);
        return true;
    }
    return false;
}

bool Crew::denyToJoin(const mongo::OID& profileid)
{
    std::string fieldname = "members." + profileid.str();
    if(_conditionedUpdate(
        BSON(fieldname << BSON("$exists" << true)),
        BSON("$unset" << BSON(fieldname << true))
    ))
    {
        EventManager::get().sendEvent(
            onCrewPlayerDeniedToJoin, this, profileid);
        return true;
    }
    return false;
}

bool Crew::addMember(const mongo::OID& profileid, CrewHierarchy hierarchy)
{
    std::string fieldname = "members." + profileid.str();
    if(_conditionedUpdate(
        BSON(fieldname << BSON("$exists" << false)),
        BSON("$set" << BSON(fieldname << hierarchy))
    ))
    {
        EventManager::get().sendEvent(
            onCrewMemberAdded, this, profileid);
        return true;
    }
    return false;
}

bool Crew::removeMember(const mongo::OID& profileid)
{
    if(profileid == mLeader) return false;
    std::string fieldname = "members." + profileid.str();
    if(_conditionedUpdate(
        BSON(fieldname << BSON("$exists" << true)),
        BSON("$unset" << BSON(fieldname << true))
    ))
    {
        EventManager::get().sendEvent(
            onCrewMemberRemoved, this, profileid);
        return true;
    }
    return false;
}

bool Crew::setMemberHierarchy(const mongo::OID& profileid,
    CrewHierarchy hierarchy)
{
    std::string fieldname = "members." + profileid.str();
    if(_conditionedUpdate(
        BSON(fieldname << BSON("$exists" << true)),
        BSON("$set" << BSON(fieldname << hierarchy))
    ))
    {
        EventManager::get().sendEvent(
            onCrewMemberHierarchyChanged, this, profileid);
        return true;
    }
    return false;
}

bool Crew::isMember(const mongo::OID& profileId)
{
    MONGO_WRAPPER({
        if(profileId == mLeader) return true;
        return getDBConn()->count(mCollection, BSON(
            "_id" << mId << "members." + profileId.str() <<
            BSON("$gt" << PENDING))) > 0;
    });
    return false;
}

CrewHierarchy Crew::getMemberHierarchy(const mongo::OID& profileId)
{
    if(profileId == mLeader) return LEADER;
    std::string idstr = profileId.str();
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(mCollection, QUERY(
            "_id" << mId << "members." + idstr <<
            BSON("$exists" << true)));
        if(doc.isEmpty())
        {
            return NOT_A_MEMBER;
        }
        else
        {
            return CrewHierarchy(
                doc["members"].Obj()[idstr.c_str()].numberInt());
        }
    });
    return NOT_A_MEMBER;
}

bool Crew::_parseObject(const mongo::BSONObj& doc)
{
    MONGO_WRAPPER({
        mName           = UTF8ToGBK(doc["name"].str());
        mLeader         = doc["leader"].OID();
        mDescription    = UTF8ToGBK(doc["description"].str());
        mReputation     = doc["reputation"].numberLong();
        mLevel          = doc["level"].numberInt();
        mColor          = doc["color"].numberInt();
        return true;
    });
    return false;
}

}
