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

#include <sampgdk/a_players.h>

#include "Player.hpp"

namespace swcu {

Player::Player(int gameid) :
    mMoney(0), mAdminLevel(0), mFlags(PlayerFlags::NO_FLAGS),
    mJoinTime(0), mGameTime(0), mRegistered(false),
    mInGameId(gameid), mTimeEnteredServer(time(0)), mLoggedIn(false)
    ,mLanguage(0)
{
    mLogName = getPlayerNameFixed(mInGameId);
    LOG(INFO) << "Loading player " << mLogName << "'s profile.";
    loadProfile();
}

bool Player::createProfile(const std::string& password)
{
    if(mRegistered)
    {
        LOG(ERROR) << "Can't create profile for " << mLogName <<
        " because there has one.";
        return false;
    }
    if(!_validatePassword(password))
    {
        LOG(ERROR) << "Entered password doesn't meet requirements.";
        return false;
    }
    mongo::OID tId              = mongo::OID::gen();
    std::string tPasswordHash   = sha1(password);
    MONGO_WRAPPER({
        getDBConn()->insert(
            Config::colNamePlayer,
            BSON(
                "_id"           << tId                  <<
                "logname"       << GBKToUTF8(mLogName)  <<
                "password"      << tPasswordHash        <<
                "lang"          << mLanguage            <<
                "gametime"      << 0                    <<
                "adminlevel"    << mAdminLevel          <<
                "flags"         << mFlags               <<
                "nickname"      << GBKToUTF8(mNickname) <<
                "money"         << mMoney               <<
                "jointime"      << mongo::DATENOW
                )
            );
        auto err = getDBConn()->getLastError();
        if (err.size())
        {
            LOG(ERROR) << "Profile create failed: " << err;
            return false;
        }
        mRegistered             = true;
        mId                     = tId;
        mIdStr                  = mId.str();
        mPasswordHash           = tPasswordHash;
        LOG(INFO) << "Player " << mLogName << "'s profile is created.";
        return true;
    });
    return false;
}

bool Player::saveProfile()
{
    if(!mRegistered)
    {
        LOG(ERROR) << "Can't save a profile of an unregistered player.";
        return false;
    }
    /**
     * Here only increases player's gaming time counter because other
     * fields should be updated inmmediately after their modifiy operations.
     */
    time_t now = time(0);
    MONGO_WRAPPER({
        getDBConn()->update(
            Config::colNamePlayer,
            QUERY("_id" << mId),
            BSON("$inc" << BSON("gametime" << now - mTimeEnteredServer))
        );
        auto err = getDBConn()->getLastError();
        if (err.size())
        {
            LOG(ERROR) << "Profile save failed: " << err;
            return false;
        }
        mTimeEnteredServer = now;
        LOG(INFO) << "Player " << mLogName << "'s profile is saved.";
        return true;
    });
    return false;
}

bool Player::loadProfile()
{
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(
            Config::colNamePlayer,
            /**
             * WARNING: DO NOT use mId field here since it won't be inited
             * in constructor.
             */
            QUERY("logname" << GBKToUTF8(mLogName))
        );
        if(doc.isEmpty())
        {
            mRegistered = false;
            LOG(WARNING) << "Player " << mLogName << 
                "'s profile can't be found.";
            return false;
        }
        else
        {
            _loadProfile(doc);
            mRegistered = true;
            LOG(INFO) << "Player " << mLogName << "'s profile is loaded.";
            return true;
        }
    });
    return false;
}

bool Player::addFlags(PlayerFlags flags)
{
    int tFlags = mFlags | flags;
    if(_updateField("$set", "flags", tFlags))
    {
        mFlags = tFlags;
        LOG(INFO) << "Appended flags " << flags
            << " to player " << mLogName;
        return true;
    }
    return false;
}

bool Player::removeFlags(PlayerFlags flags)
{
    int tFlags = mFlags ^ flags;
    if(_updateField("$set", "flags", tFlags))
    {
        mFlags = tFlags;
        LOG(INFO) << "Removed flags " << flags <<
            " from player " << mLogName;
        return true;
    }
    return false;
}

bool Player::setLogName(const std::string& name)
{
    if(name.size() < 3)
    {
        return false;
    }
    if(SetPlayerName(mInGameId, name.c_str()) != 1)
    {
        return false;
    }
    if(_updateField("$set", "logname", GBKToUTF8(name)))
    {
        LOG(INFO) << "Player " << mLogName << "'s logname is set to "
            << name;
        mLogName = name;
        return true;
    }
    return false;
}

bool Player::setNickname(const std::string& name)
{
    if(name.size() < 3)
    {
        return false;
    }
    if(_updateField("$set", "nickname", GBKToUTF8(name)))
    {
        LOG(INFO) << "Player " << mLogName << "'s nickname is set to "
            << name;
        mNickname = name;
        return true;
    }
    return false;
}

bool Player::increaseMoney(int amount)
{
    if(_updateField("$inc", "money", amount))
    {
        LOG(INFO) << "Gived " << mLogName << " $" << amount << ".";
        mMoney += amount;
        return true;
    }
    return false;
}

bool Player::verifyPassword(const std::string& password) const
{
    return sha1(password) == mPasswordHash;
}

bool Player::changePassword(const std::string& password)
{
    if(!_validatePassword(password))
    {
        LOG(ERROR) << "Entered password doesn't meet requirements.";
        return false;
    }
    std::string tPasswordHash = sha1(password);
    if(_updateField("$set", "password", tPasswordHash))
    {
        LOG(INFO) << "Player " << mLogName << "'s password is changed.";
        mPasswordHash = tPasswordHash;
        return true;
    }
    return false;
}

bool Player::setAdminLevel(int level)
{
    if(_updateField("$set", "adminlevel", level))
    {
        LOG(INFO) << "Player " << mLogName << "'s admin level is set to "
            << level << ".";
        mAdminLevel = level;
        return true;
    }
    return false;
}

bool Player::setLanguage(int lang)
{
    if(!isRegistered())
    {
        mLanguage = lang;
        return true;
    }
    if(_updateField("$set", "lang", lang))
    {
        LOG(INFO) << "Player " << mLogName << "'s language is set to "
            << lang << ".";
        mLanguage = lang;
        return true;
    }
    return false;
}

void Player::_loadProfile(const mongo::BSONObj& doc)
{
    MONGO_WRAPPER({
        mId             = doc["_id"].OID();
        mIdStr          = mId.str();
        mNickname       = UTF8ToGBK(doc["nickname"].str());
        mPasswordHash   = doc["password"].str();
        mAdminLevel     = doc["adminlevel"].numberInt();
        mMoney          = doc["money"].numberInt();
        mFlags          = doc["flags"].numberInt();
        mJoinTime       = doc["jointime"].numberLong();
        mGameTime       = doc["gametime"].numberLong();
        mLanguage       = doc["lang"].numberInt();
    });
}

bool Player::_validatePassword(const std::string& password)
{
    return password.size() >= 6;
}

}
