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
#include <boost/algorithm/string.hpp>

#include "../Streamer/Streamer.hpp"
#include "../Multilang/Language.hpp"
#include "../Map/Map.hpp"
#include "PlayerColors.hpp"

#include "Player.hpp"

namespace swcu {

const char* PoliceRankString[] =
{
    "平民[Civilian]",
    "警员[Police Officer]",
    "警探[Police Detective]",
    "警长[Police Sergeant]",
    "警督[Police Lieutenant]",
    "警监[Police Captain]",
    "指挥官[Police Commander]",
    "副总警监[Police Deputy Chief]",
    "助理总警监[Assistant Chief of Police]",
    "总警监[Chief of Police]"
};

Player::Player(int gameid) :
    mMoney(0), mAdminLevel(0), mFlags(PlayerFlags::NO_FLAGS),
    mJoinTime(0), mGameTime(0), mLanguage(0), mColor(getRandomColor()),
    mPoliceRank(CIVILIAN), mWantedLevel(0), mTimeInPrison(0),
    mTimeToFree(0),
    mRegistered(false),
    mInGameId(gameid), mTimeEnteredServer(time(0)), mLoggedIn(false),
    mTextLabel(0)
{
    mLogName = getPlayerNameFixed(mInGameId);
    LOG(INFO) << "Loading player " << mLogName << "'s profile.";
    SetPlayerColor(mInGameId, mColor);
    loadProfile();
}

Player::~Player()
{
    if(mTextLabel != 0) DestroyDynamic3DTextLabel(mTextLabel);
    saveProfile();
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
    auto datetime               = mongo::jsTime();
    MONGO_WRAPPER({
        getDBConn()->insert(
            Config::colNamePlayer,
            BSON(
                "_id"           << tId                  <<
                "logname"       << GBKToUTF8(mLogName)  <<
                "password"      << tPasswordHash        <<
                "lang"          << mLanguage            <<
                "color"         << mColor               <<
                "gametime"      << 0                    <<
                "adminlevel"    << mAdminLevel          <<
                "flags"         << mFlags               <<
                // A fix.
                "nickname"      << GBKToUTF8(mLogName)  <<
                "money"         << mMoney               <<
                "jointime"      << datetime             <<
                "policerank"    << mPoliceRank          <<
                "wantedlevel"   << mWantedLevel         <<
                "timeinprison"  << mTimeInPrison        <<
                "timetofree"    << mTimeToFree
                )
            );
        if(dbCheckError())
        {
            mRegistered         = true;
            mId                 = tId;
            mIdStr              = mId.str();
            mPasswordHash       = tPasswordHash;
            mJoinTime           = datetime.toTimeT();
            LOG(INFO) << "Player " << mLogName << "'s profile is created.";
            return true;
        }
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
        if(dbCheckError())
        {
            mTimeEnteredServer = now;
            LOG(INFO) << "Player " << mLogName << "'s profile is saved.";
            return true;
        }
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
            mRegistered = true;
            _loadProfile(doc);
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
        updatePlayerLabel();
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
        updatePlayerLabel();
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
        updatePlayerLabel();
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

bool Player::setPoliceRank(PoliceRank rank)
{
    if(rank > 9 || rank < 0)
    {
        LOG(WARNING) << "Police rank should range from 0~9.";
        return false;
    }
    if(_updateField("$set", "policerank", rank))
    {
        LOG(INFO) << "Player " << mLogName << "'s police rank is set to "
            << rank << ".";
        mPoliceRank = rank;
        updatePlayerLabel();
        return true;
    }
    return false;
}

bool Player::setWantedLevel(int level)
{
    if(level > 6 || level < 0)
    {
        LOG(WARNING) << "Wanted level should range from 0~6.";
        return false;
    }
    if(_updateField("$set", "wantedlevel", level))
    {
        LOG(INFO) << "Player " << mLogName << "'s wandted level is set to "
            << level << ".";
        mWantedLevel = level;
        updatePlayerLabel();
        _applyWantedLevel();
        return true;
    }
    return false;
}

bool Player::setColor(int color)
{
    if(_updateField("$set", "color", color))
    {
        LOG(INFO) << "Player " << mLogName << "'s color is set to "
            << color << ".";
        mColor = color;
        SetPlayerColor(mInGameId, mColor);
        return true;
    }
    return false;
}

bool Player::putIntoPrison(time_t prisonTerm)
{
    if(hasFlags(STATUS_JAILED)) return false;
    if(!teleportToPrison())
    {
        return false;
    }
    int64_t tofree = time(0) + prisonTerm;
    if(_updateField("$set", "timetofree", tofree) &&
        _updateField("$inc", "timeinprison", static_cast<int64_t>(prisonTerm)))
    {
        LOG(INFO) << "Player " << mLogName << " now have a prison term of "
            << prisonTerm << " seconds.";
        mTimeToFree = tofree;
        mTimeInPrison += tofree;
        setWantedLevel(0);
        addFlags(STATUS_JAILED);
        return true;
    }
    return false;
}

bool Player::isPrisonTermExceeded() const
{
    return time(0) > mTimeToFree;
}

bool Player::freeFromPrison()
{
    if(!hasFlags(STATUS_JAILED)) return false;
    if(!_updateField("$set", "timetofree", 0)) return false;
    mTimeToFree = 0;
    removeFlags(STATUS_JAILED);
    ForceClassSelection(mInGameId);
    SetPlayerHealth(mInGameId, 0.0);
    SetPlayerVirtualWorld(mInGameId, WORLD_MAIN);
    updatePlayerLabel();
    LOG(INFO) << "Player " << mLogName << " is freed from prison.";
    return true;
}

void Player::updatePlayerLabel()
{
    if(mTextLabel != 0)
    {
        DestroyDynamic3DTextLabel(mTextLabel);
    }
    std::stringstream label;
    // Nickname
    label << mNickname << "{FFFFFF}\n";
    // Admin Level
    if(mAdminLevel > 0)
    {
        label << "Level " << mAdminLevel << "\n";
    }
    // Police Rank
    if(static_cast<size_t>(mPoliceRank) >= sizeof(PoliceRankString))
    {
        LOG(WARNING) <<
        "static_cast<size_t>(mPoliceRank) >= sizeof(PoliceRankString)";
    }
    else
    {
        if(mPoliceRank > CIVILIAN)
        {
            label << "{33FFFF}" << PoliceRankString[mPoliceRank]
                << "{FFFFFF}\n";
        }
    }
    // Wanted Level
    if(mWantedLevel > 0)
    {
        label << "{FFFF00}通缉等级 " << mWantedLevel << "{FFFFFF}\n";
    }
    // Login Name and ID
    label << "(" << mLogName << ")(" << mInGameId << ")\n";
    mTextLabel = CreateDynamic3DTextLabel(label.str(), 0xFFFFFFFF,
        0.0, 0.0, 0.2, 200.0, mInGameId);
    LOG(INFO) << "Player " << mLogName << "'s label created with id " <<
        mTextLabel;
}

void Player::teleportTo(float x, float y, float z, float facing,
    int world, int interior)
{
    Streamer_UpdateEx(mInGameId, x, y, z, world, interior);

                        SetPlayerPos(mInGameId, x, y, z);
                        SetPlayerFacingAngle(mInGameId, facing);
    if(world != -1)     SetPlayerInterior(mInGameId, interior);
    if(interior != -1)  SetPlayerVirtualWorld(mInGameId, world);

    if(IsPlayerInAnyVehicle(mInGameId)
        && GetPlayerVehicleSeat(mInGameId) == 0 /* driver */)
    {
        int vid =           GetPlayerVehicleID(mInGameId);

                            SetVehiclePos(vid, x, y, z);
                            SetVehicleZAngle(vid, facing);
        if(world != -1)     SetVehicleVirtualWorld(vid, world);
        if(interior != -1)  LinkVehicleToInterior(vid, interior);

        int max = GetMaxPlayers();
        for(int i = 0; i < max; ++i)
        {
            if(GetPlayerVehicleID(i) == vid)
            {
                int seat =          GetPlayerVehicleSeat(i);

                                    SetPlayerPos(i, x, y, z);
                if(world != -1)     SetPlayerInterior(i, interior);
                if(interior != -1)  SetPlayerVirtualWorld(i, world);

                                    PutPlayerInVehicle(i, vid, seat);
            }
        }
    }
}

void Player::teleportTo(int targetplayer)
{
    if(!IsPlayerConnected(targetplayer))
    {
        SendClientMessage(mInGameId, 0xFFFFFFFF, t(this, PLAYER_NOT_FOUND));
        return;
    }
    float x, y, z, facing;
    GetPlayerPos(targetplayer, &x, &y, &z);
    GetPlayerFacingAngle(targetplayer, &facing);
    teleportTo(x, y, z + 0.3, facing, GetPlayerVirtualWorld(targetplayer),
        GetPlayerInterior(targetplayer));
}

bool Player::teleportTo(const std::string& placeName)
{
    using namespace boost::algorithm;
    std::string trimmedName = placeName;
    trim(trimmedName);
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(
            Config::colNameTeleport,
            QUERY("name" << GBKToUTF8(trimmedName))
        );
        if(doc.isEmpty())
        {
            LOG(WARNING) << "Teleport " << trimmedName << " can't be found.";
            SendClientMessage(mInGameId, 0xFFFFFFFF,
                t(this, TELEPORT_NOT_FOUND));
            return false;
        }
        else
        {
            float x; float y; float z; float facing;
            int world; int interior;
            x           = doc["x"].numberDouble();
            y           = doc["y"].numberDouble();
            z           = doc["z"].numberDouble();
            facing      = doc["facing"].numberDouble();
            world       = doc["world"].numberInt();
            interior    = doc["interior"].numberInt();
            teleportTo(x, y, z, facing, world, interior);
            getDBConn()->update(
                Config::colNameTeleport,
                BSON("_id" << doc["_id"].OID()),
                BSON("$inc" << BSON("use" << 1))
            );
            dbCheckError();
            LOG(INFO) << "Player " << mLogName << " teleported to "
                << placeName;
            return true;
        }
    });
    SendClientMessage(mInGameId, 0xFFFFFFFF,
        t(this, TELEPORT_INVALID));
    return false;
}

bool Player::createTeleport(const std::string& placeName)
{
    if(!mRegistered)
    {
        LOG(ERROR) << "Can't create teleport because player " << mLogName <<
        " hasn't registered.";
        return false;
    }
    using namespace boost::algorithm;
    std::string trimmedName = placeName;
    trim(trimmedName);
    float x, y, z, facing;
    GetPlayerPos(mInGameId, &x, &y, &z);
    GetPlayerFacingAngle(mInGameId, &facing);
    int world       = GetPlayerVirtualWorld(mInGameId);
    int interior    = GetPlayerInterior(mInGameId);
    MONGO_WRAPPER({
        getDBConn()->insert(
            Config::colNameTeleport,
            BSON(
                "_id"           << mongo::OID::gen()    <<
                "name"          << GBKToUTF8(trimmedName)<<
                "x"             << x                    <<
                "y"             << y                    <<
                "z"             << z                    <<
                "facing"        << facing               <<
                "world"         << world                <<
                "interior"      << interior             <<
                "creator"       << mId                  <<
                "createtime"    << mongo::DATENOW       <<
                "use"           << 0
            )
        );
        if(dbCheckError())
        {
            SendClientMessage(mInGameId, 0xFFFFFFFF,
                t(this, TELEPORT_CREATE_SUCCESS));
            LOG(INFO) << "Teleport " << trimmedName << " is created.";
            return true;
        }
    });
    LOG(ERROR) << "Failed to create teleport.";
    SendClientMessage(mInGameId, 0xFFFFFFFF,
        t(this, TELEPORT_CREATE_FAILED));
    return false;
}

void Player::_loadProfile(const mongo::BSONObj& doc)
{
    MONGO_WRAPPER({
        mId             = doc["_id"].OID();
        mIdStr          = mId.str();
        mNickname       = UTF8ToGBK(doc["nickname"].str());
        if(mNickname.size() == 0) setNickname(mLogName);
        mPasswordHash   = doc["password"].str();
        mAdminLevel     = doc["adminlevel"].numberInt();
        mMoney          = doc["money"].numberInt();
        mFlags          = doc["flags"].numberInt();
        mJoinTime       = doc["jointime"].date().toTimeT();
        mGameTime       = doc["gametime"].numberLong();
        mLanguage       = doc["lang"].numberInt();
        mColor          = doc["color"].numberInt();
        if(mColor == 0) setColor(getRandomColor());
        mPoliceRank     = PoliceRank(doc["policerank"].numberInt());
        if(mPoliceRank > 9) mPoliceRank = CHIEF_OF_POLICE;
        if(mPoliceRank < 0) mPoliceRank = CIVILIAN;
        mWantedLevel    = doc["wantedlevel"].numberInt();
        if(mWantedLevel > 9) mWantedLevel = 9;
        if(mWantedLevel < 0) mWantedLevel = 0;
        mTimeInPrison   = doc["timeinprison"].numberLong();
        mTimeToFree     = doc["timetofree"].numberLong();
    });
    _applyWantedLevel();
    updatePlayerLabel();
    if(!isPrisonTermExceeded()) teleportToPrison();
}

void Player::_applyWantedLevel()
{
    SetPlayerWantedLevel(mInGameId, mWantedLevel);
    SetPlayerColor(mInGameId, (mWantedLevel > 0) ? 0xFF0000FF : mColor);
}

bool Player::teleportToPrison()
{
    return teleportTo("##prison");
}

bool Player::_validatePassword(const std::string& password)
{
    return password.size() >= 6;
}

}
