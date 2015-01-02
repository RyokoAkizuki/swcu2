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

#include <sampgdk/a_players.h>
#include <boost/algorithm/string.hpp>

#include "../Streamer/Streamer.hpp"
#include "../Map/Map.hpp"
#include "../Crew/CrewManager.hpp"
#include "../Crew/Crew.hpp"

#include "Player.hpp"

namespace swcu {

const char* PoliceRankString[] =
{
    "平民",
    "警员",
    "警探",
    "警长",
    "警督",
    "警监",
    "指挥官",
    "副总警监",
    "助理总警监",
    "总警监"
};

Player::Player(int gameid) :
    StorableObject(Config::colNamePlayer),
    mMoney(0), mAdminLevel(0), mFlags(PlayerFlags::NO_FLAGS),
    mGameTime(0),
    mPoliceRank(CIVILIAN), mWantedLevel(0), mTimeInPrison(0),
    mTimeToFree(0),
    mInGameId(gameid), mLastSaved(time(0)), mLoggedIn(false),
    mTextLabel(0), mPrivateVehicle(INVALID_VEHICLE_ID)
{
    mLogName = getPlayerNameFixed(mInGameId);
    mNickname = mLogName;
    SetPlayerColor(mInGameId, mColor.getRGBA());
    LOG(INFO) << "Loading player " << mLogName << "'s profile.";
    _loadObject("logname", GBKToUTF8(mLogName));
}

Player::Player(const mongo::OID& id) :
    StorableObject(Config::colNamePlayer, id),
    mMoney(0), mAdminLevel(0), mFlags(PlayerFlags::NO_FLAGS),
    mGameTime(0),
    mPoliceRank(CIVILIAN), mWantedLevel(0), mTimeInPrison(0),
    mTimeToFree(0),
    mInGameId(-1), mLastSaved(time(0)), mLoggedIn(false),
    mTextLabel(0), mPrivateVehicle(INVALID_VEHICLE_ID)
{
    _loadObject();
}

Player::~Player()
{
    if(mTextLabel != 0) DestroyDynamic3DTextLabel(mTextLabel);
    saveProfile();
}

bool Player::createProfile(const std::string& password)
{
    if(!_validatePassword(password))
    {
        return false;
    }
    std::string tPasswordHash   = sha1(password);
    if(_createObject(BSON(
        "logname"       << GBKToUTF8(mLogName)  <<
        "password"      << tPasswordHash        <<
        "color"         << mColor.getRGB()      <<
        "crew"          << mCrew                <<
        "gametime"      << 0                    <<
        "adminlevel"    << mAdminLevel          <<
        "flags"         << mFlags               <<
        "nickname"      << GBKToUTF8(mNickname) <<
        "money"         << mMoney               <<
        "policerank"    << mPoliceRank          <<
        "wantedlevel"   << mWantedLevel         <<
        "timeinprison"  << mTimeInPrison        <<
        "timetofree"    << mTimeToFree
    )))
    {
        mPasswordHash   = tPasswordHash;
        EventLog("player", "createProfile", BSON(
            "profile"   << mId                  <<
            "logname"   << mLogName             <<
            "ip"        << getPlayerIP(mInGameId)<<
            "gpci"      << getGPCI(mInGameId)
        ));
        LOG(INFO) << "Player " << mLogName << "'s profile is created.";
        return true;
    }
    return false;
}

bool Player::saveProfile()
{
    time_t now = time(0);
    if(_updateObject(BSON(
        "$inc" << BSON(
            "gametime" << now - mLastSaved
        )
    )))
    {
        mLastSaved = now;
        return true;
    }
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
        EventLog("player", "increaseMoney", BSON(
            "profile"   << mId      <<
            "amount"    << amount
        ));
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
        EventLog("player", "changePassword", BSON(
            "profile"   << mId
        ));
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
        EventLog("player", "setAdminLevel", BSON(
            "profile"   << mId          <<
            "levelprev" << mAdminLevel  <<
            "level"     << level
        ));
        SendClientMessage(mInGameId, 0xFFFFFFFF, CSTR(
            "你的管理员等级被设置为 " << level
        ));
        mAdminLevel = level;
        updatePlayerLabel();
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
        EventLog("player", "setPoliceRank", BSON(
            "profile"   << mId          <<
            "rankprev"  << mPoliceRank  <<
            "rank"      << rank
        ));
        SendClientMessage(mInGameId, 0xFFFFFFFF, CSTR(
            "你的警衔被设置为 " << rank
        ));
        mPoliceRank = rank;
        updatePlayerLabel();
        return true;
    }
    return false;
}

std::string Player::getPoliceRankStr() const
{
    if(static_cast<size_t>(mPoliceRank) >= sizeof(PoliceRankString))
    {
        LOG(WARNING) <<
        "static_cast<size_t>(mPoliceRank) >= sizeof(PoliceRankString)";
        return "";
    }
    return PoliceRankString[mPoliceRank];
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
        SendClientMessage(mInGameId, 0xFFFFFFFF, CSTR(
            "你的通缉等级被设置为 " << level
        ));
        updatePlayerLabel();
        _applyWantedLevel();
        return true;
    }
    return false;
}

bool Player::setColor(RGBAColor color)
{
    if(_updateField("$set", "color", color.getRGB()))
    {
        LOG(INFO) << "Player " << mLogName << "'s color is set to "
            << color.getRGB() << ".";
        mColor = color;
        SetPlayerColor(mInGameId, mColor.getRGBA());
        updatePlayerLabel();
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
        SendClientMessage(mInGameId, 0xFFFFFFFF, CSTR(
            "你被关入监狱" << prisonTerm / 60 << "分钟."
        ));
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
    label << getColoredNickname() << "\n";
    // Admin Level
    if(mAdminLevel > 0)
    {
        label << "Level " << mAdminLevel << "\n";
    }
    // Police Rank
    if(mPoliceRank > CIVILIAN)
    {
        label << "{33FFFF}" << getPoliceRankStr() << "{FFFFFF}\n";
    }
    // Wanted Level
    if(mWantedLevel > 0)
    {
        label << "{FFFF00}通缉等级 " << mWantedLevel << "{FFFFFF}\n";
    }
    // Crew
    if(isCrewMember())
    {
        label << CrewManager::get().getCrew(mCrew)->getColoredName() << "\n";
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
        SendClientMessage(mInGameId, 0xFFFFFFFF, "未找到该玩家.");
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
    std::string trimmedName = placeName;
    boost::algorithm::trim(trimmedName);
    MONGO_WRAPPER({
        auto doc = getDBConn()->findOne(
            Config::colNameTeleport,
            QUERY("name" << GBKToUTF8(trimmedName))
        );
        if(doc.isEmpty())
        {
            LOG(WARNING) << "Teleport " << trimmedName << " can't be found.";
            SendClientMessage(mInGameId, 0xFFFFFFFF, "传送点不存在.");
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
                BSON("_id" << doc["_id"]),
                BSON("$inc" << BSON("use" << 1))
            );
            dbCheckError();
            LOG(INFO) << "Player " << mLogName << " teleported to "
                << placeName;
            return true;
        }
    });
    SendClientMessage(mInGameId, 0xFFFFFFFF, "传送点无效.");
    return false;
}

bool Player::createTeleport(const std::string& placeName)
{
    std::string trimmedName = placeName;
    boost::algorithm::trim(trimmedName);
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
            SendClientMessage(mInGameId, 0xFFFFFFFF, "传送点创建成功.");
            LOG(INFO) << "Teleport " << trimmedName << " is created.";
            return true;
        }
    });
    LOG(ERROR) << "Failed to create teleport.";
    SendClientMessage(mInGameId, 0xFFFFFFFF,
        "传送点创建失败. 这个名字可能已经被使用了.");
    return false;
}

bool Player::createPrivateVehicle(int model)
{
    dropPrivateVehicle();
    mPrivateVehicle = CreateVehicle(model, 0.0, 0.0, 0.0, 0.0,
        rand() % 256, rand() % 256, 60);
    teleportPrivateVehicleToPlayer();
    return mPrivateVehicle != INVALID_VEHICLE_ID;
}

bool Player::dropPrivateVehicle()
{
    if(mPrivateVehicle != INVALID_VEHICLE_ID)
    {
        DestroyVehicle(mPrivateVehicle);
        return true;
    }
    return false;
}

void Player::teleportPrivateVehicleToPlayer()
{
    if(mPrivateVehicle == INVALID_VEHICLE_ID) return;

    float   x, y, z, a;
    GetPlayerPos(mInGameId, &x, &y, &z);
    GetPlayerFacingAngle(mInGameId, &a);
    int     interior    = GetPlayerInterior(mInGameId);
    int     world       = GetPlayerVirtualWorld(mInGameId);

    SetVehiclePos(mPrivateVehicle, x, y, z);
    SetVehicleZAngle(mPrivateVehicle, a);
    SetVehicleVirtualWorld(mPrivateVehicle, world);
    LinkVehicleToInterior(mPrivateVehicle, interior);
    PutPlayerInVehicle(mInGameId, mPrivateVehicle, 0 /* driver */);
}

bool Player::_setCrew(const mongo::OID& crewId)
{
    if(_updateField("$set", "crew", crewId))
    {
        mCrew = crewId;
        if(crewId.isSet())
        {
            LOG(INFO) << "Player " << mLogName << " joined a crew.";
        }
        else
        {
            LOG(INFO) << "Player " << mLogName << " quited his crew.";
        }
        updatePlayerLabel();
        return true;
    }
    return false;
}

bool Player::_parseObject(const mongo::BSONObj& doc)
{
    MONGO_WRAPPER({
        mLogName        = UTF8ToGBK(doc["logname"].str());
        mNickname       = UTF8ToGBK(doc["nickname"].str());
        if(mNickname.size() == 0) setNickname(mLogName);
        mPasswordHash   = doc["password"].str();
        mAdminLevel     = doc["adminlevel"].numberInt();
        mMoney          = doc["money"].numberInt();
        mFlags          = doc["flags"].numberInt();
        mGameTime       = doc["gametime"].numberLong();
        mColor.assignRGB(doc["color"].numberInt());
        if(mColor.getRGB() == 0) setColor(RGBAColor());
        mCrew           = doc["crew"].OID();
        mPoliceRank     = PoliceRank(doc["policerank"].numberInt());
        if(mPoliceRank > 9) mPoliceRank = CHIEF_OF_POLICE;
        if(mPoliceRank < 0) mPoliceRank = CIVILIAN;
        mWantedLevel    = doc["wantedlevel"].numberInt();
        if(mWantedLevel > 6) mWantedLevel = 6;
        if(mWantedLevel < 0) mWantedLevel = 0;
        mTimeInPrison   = doc["timeinprison"].numberLong();
        mTimeToFree     = doc["timetofree"].numberLong();
        _applyWantedLevel();
        updatePlayerLabel();
        if(!isPrisonTermExceeded()) teleportToPrison();
        return true;
    });
    return false;
}

void Player::_applyWantedLevel()
{
    SetPlayerWantedLevel(mInGameId, mWantedLevel);
    SetPlayerColor(mInGameId,
        (mWantedLevel > 0) ? 0xFF0000FF : mColor.getRGBA());
}

bool Player::teleportToPrison()
{
    return teleportTo("##prison");
}

bool Player::_validatePassword(const std::string& password)
{
    return password.size() >= 6;
}

bool Player::onUpdate()
{
    if(hasFlags(swcu::STATUS_JAILED))
    {
        if(isPrisonTermExceeded())
        {
            freeFromPrison();
        }
    }
    if(hasFlags(swcu::STATUS_FREEZED))
    {
        return false;
    }
    return true;
}

bool Player::onSpawn()
{
    if(!isPrisonTermExceeded())
    {
        teleportToPrison();
    }
    return true;
}

bool Player::onDeath()
{
    return true;
}

void Player::handleEvent(const Event& evt)
{
    switch(std::get<0>(evt))
    {
        case onCrewLeaderChanged:
        {
            auto crew = boost::any_cast<Crew*>(std::get<1>(evt));
            if(crew->getLeader() == mId)
            {
                _setCrew(boost::any_cast<Crew*>(std::get<1>(evt))->getId());
            }
            break;
        }
        case onCrewPlayerApplyToJoin:
        {
            auto crew = boost::any_cast<Crew*>(std::get<1>(evt));
            if(crew->getLeader() == mId)
            {
                SendClientMessage(mInGameId, 0xFFFFFFFF,
                    "有玩家申请加入你的帮派, 请及时处理");
            }
            break;
        }
        case onCrewPlayerApprovedToJoin:
        case onCrewMemberAdded:
        {
            auto profile = boost::any_cast<mongo::OID>(std::get<2>(evt));
            if(profile == mId)
            {
                _setCrew(boost::any_cast<Crew*>(std::get<1>(evt))->getId());
            }
            break;
        }
        case onCrewPlayerDeniedToJoin:
        case onCrewMemberRemoved:
        {
            auto profile = boost::any_cast<mongo::OID>(std::get<2>(evt));
            if(profile == mId)
            {
                _setCrew(mongo::OID());
            }
            break;
        }
        case onCrewColorChanged:
        case onCrewNameChanged:
        {
            auto crew = boost::any_cast<Crew*>(std::get<1>(evt));
            if(crew->getId() == mCrew)
            {
                updatePlayerLabel();
            }
            break;
        }
        case onCrewMemberHierarchyChanged:
        {
            auto profile = boost::any_cast<mongo::OID>(std::get<2>(evt));
            if(profile == mId)
            {
                updatePlayerLabel();
            }
            break;
        }
        default:
            break;
    }
}

}
