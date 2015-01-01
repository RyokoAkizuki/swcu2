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

#include "Crew.hpp"

namespace swcu {

Crew::Crew() : StorableObject(Config::colNameCrew),
    mScore(0), mLevel(0), mTeamId(NO_TEAM)
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
            "score"         << mScore                   <<
            "level"         << mLevel                   <<
            "color"         << mColor.getRGB()
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
        return true;
    }
    return false;
}

bool Crew::setLeader(const mongo::OID& profileId)
{
    if(_updateField("$set", "leader", profileId))
    {
        mLeader = profileId;
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

bool Crew::increaseScore(int64_t deltaScore)
{
    if(_updateField("$inc", "score", deltaScore))
    {
        mScore += deltaScore;
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
        return true;
    }
    return false;
}

bool Crew::_parseObject(const mongo::BSONObj& doc)
{
    MONGO_WRAPPER({
        mName           = UTF8ToGBK(doc["name"].str());
        mLeader         = doc["leader"].OID();
        mDescription    = UTF8ToGBK(doc["description"].str());
        mScore          = doc["score"].numberLong();
        mLevel          = doc["level"].numberInt();
        mColor          = doc["color"].numberInt();
        return true;
    });
    return false;
}

}
