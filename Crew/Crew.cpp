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

#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>

#include "../Common/RGBAColor.hpp"

#include "Crew.hpp"

namespace swcu {

Crew::Crew() : mCreateTime(0), mScore(0), mLevel(0),
    mColor(0xFFFFFF), mTeamId(NO_TEAM), mValid(false)
{
}

Crew::Crew(const std::string& name) : Crew()
{
    mName = name;
    if(_findAndLoadDocumentByName() || _createCrew())
    {
        mValid = true;
    }
}

Crew::Crew(const mongo::OID& id) : Crew()
{
    mId = id;
    if(_findAndLoadDocumentById())
    {
        mValid = true;
    }
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

bool Crew::setColor(int32_t color)
{
    if(_updateField("$set", "color", color))
    {
        mColor = color;
        return true;
    }
    return false;
}

bool Crew::_createCrew()
{
    MONGO_WRAPPER({
        mongo::OID id           = mongo::OID::gen();
        auto datetime           = mongo::jsTime();
        int32_t color           = RGBAColor().getRGB();
        getDBConn()->insert(
            Config::colNameCrew,
            BSON(
                "_id"           << id                       <<
                "name"          << GBKToUTF8(mName)         <<
                "leader"        << mLeader                  <<
                "createtime"    << datetime                 <<
                "description"   << GBKToUTF8(mDescription)  <<
                "score"         << mScore                   <<
                "level"         << mLevel                   <<
                "color"         << color
                )
            );
        if(dbCheckError())
        {
            mId                 = id;
            mCreateTime         = datetime.toTimeT();
            mColor              = color;
            LOG(INFO) << "Crew created: " << mName;
            return true;
        }
    });
    return false;
}

bool Crew::_findAndLoadDocumentByName()
{
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(
            Config::colNameCrew,
            QUERY("name" << GBKToUTF8(mName))
        );
        if(doc.isEmpty())
        {
            LOG(WARNING) << "Specified crew (" << mName
                << ") not found. This is going to be an invalid object.";
            return false;
        }
        else
        {
            return _loadDocument(doc);
        }
    });
    return false;
}

bool Crew::_findAndLoadDocumentById()
{
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(
            Config::colNameCrew,
            QUERY("_id" << mId)
        );
        if(doc.isEmpty())
        {
            LOG(WARNING) << "Specified crew (" << mId.str()
                << ") not found. This is going to be an invalid object.";
            return false;
        }
        else
        {
            return _loadDocument(doc);
        }
    });
    return false;
}

bool Crew::_loadDocument(const mongo::BSONObj& doc)
{
    MONGO_WRAPPER({
        mId             = doc["_id"].OID();
        mName           = UTF8ToGBK(doc["name"].str());
        mLeader         = doc["leader"].OID();
        mCreateTime     = doc["createtime"].date().toTimeT();
        mDescription    = UTF8ToGBK(doc["description"].str());
        mScore          = doc["score"].numberLong();
        mLevel          = doc["level"].numberInt();
        mColor          = doc["color"].numberInt();
        return true;
    });
    return false;
}

}
