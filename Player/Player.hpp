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

#include "../GangZone/GangZone.hpp"

namespace swcu {

class Crew;

enum PlayerFlags
{
    STATUS_MUTED        = 0b00001000,
    STATUS_FREEZED      = 0b00000100,
    STATUS_JAILED       = 0b00000010,
    STATUS_BANNED       = 0b00000001,
    NO_FLAGS            = 0b00000000
};

extern const char* PoliceRankString[10];

enum PoliceRank
{
    CHIEF_OF_POLICE     = 9,
    ASSISTANT_CHIEF     = 8,
    POLICE_DEPUTY_CHIEF = 7,
    POLICE_COMMANDER    = 6,
    POLICE_CAPTAIN      = 5,
    POLICE_LIEUTENANT   = 4,
    POLICE_SERGEANT     = 3,
    POLICE_DETECTIVE    = 2,
    POLICE_OFFICER      = 1,
    CIVILIAN            = 0
};

class Player : public StorableObject
{
protected:
    /**
     * Profile.
     */
    std::string         mLogName;
    std::string         mNickname;
    std::string         mPasswordHash;
    int                 mMoney;
    int                 mAdminLevel;
    int                 mFlags;
    int64_t             mGameTime;
    RGBAColor           mColor;

    mongo::OID          mCrew;
    std::shared_ptr<Crew>   mCrewPtr;

    /**
     * Police System
     */
    // Default is 0, CIVILIAN.
    PoliceRank          mPoliceRank;
    // Default is 0.
    int                 mWantedLevel;
    // Time spent in prison in total.
    int64_t             mTimeInPrison;
    // Time to be freed from prison.
    int64_t             mTimeToFree;
    /**
     * Gaming.
     */
    int                 mInGameId;
    int64_t             mLastSaved;
    bool                mLoggedIn;
    int                 mTextLabel;
    int                 mPrivateVehicle;

    std::shared_ptr<GangZone>   mCurrentGangZone;

    /**
     * Houses, Weapons, Vehicles, etc.
     */

public:
                        Player(int ingameid);
    virtual             ~Player();

    /**
     * ************ Profile ************
     */
            int64_t     getGameTime() const
            { return mGameTime; }

    /**
     * Create a profile if player is not registered.
     * @param  password Raw password field.
     * @return          True if player's profile is created.
     *                  False if: player is registered;
     *                  database failed.
     */
            bool        createProfile(const std::string& password);

    /**
     * Save current profile to database.
     * @return True if succeeded.
     *         False if database fails.
     */
            bool        saveProfile();

            bool        isLoggedIn() const
            { return mLoggedIn; }

    /**
     * Set player's logged in flag.
     * Note: You CAN'T set it to true if player isn't registered.
     * @param  logged Should player be set logged in or not.
     * @return        Nothing.
     */
            void        setLoggedIn(bool logged)
            { mLoggedIn = logged; }

    /**
     * Append flags to player.
     * @param  flags Flags to append.
     *               To append multiple flags,
     *               use as STATUS_MUTED | STATUS_FREEZED.
     * @return       True if succeeded. False if not.
     */
            bool        addFlags(PlayerFlags flags);

    /**
     * Removes certian flags from player.
     * @param  flags Flags to remove.
     * @return       True if succeeded. False if not.
     */
            bool        removeFlags(PlayerFlags flags);

    /**
     * Check if player has certian flags.
     * @param  flags Flags to confirm.
     * @return       True if all flags passed in matches.
     *               Flase if player don't have any one of flags passed in.
     */
            bool        hasFlags(PlayerFlags flags)
            { return (mFlags & flags) == flags; }

            std::string getLogName() const
            { return mLogName; }
            bool        setLogName(const std::string& name);

            std::string getNickname() const
            { return mNickname; }
            std::string getColoredNickname() const
            { return mColor.getEmbedCode() + mNickname + "{FFFFFF}"; }
            bool        setNickname(const std::string& name);

            int         getMoney() const
            { return mMoney; }
            bool        increaseMoney(int amount);

    /**
     * Verify inputed password by comparing its hash with correct hash.
     * @param  password Plaintext password.
     * @return          True if password is correct.
     *                  False if not.
     */
            bool        verifyPassword(const std::string& password) const;
            bool        changePassword(const std::string& password);

    /**
     * Get player's admin level which is used to determinate player's
     * permissions to certain operations.
     * @return An integer value of player's administration level.
     */
            int         getAdminLevel() const
            { return mAdminLevel; }

            bool        setAdminLevel(int level);

            PoliceRank  getPoliceRank() const
            { return mPoliceRank; }

            bool        setPoliceRank(PoliceRank rank);
            std::string getPoliceRankStr() const;

            int         getWantedLevel() const
            { return mWantedLevel; }

            bool        setWantedLevel(int level);

            RGBAColor   getColor() const
            { return mColor; }

            bool        setColor(RGBAColor color);

    /**
     * ************ Game Features ************
     */

            int         getInGameId() const
            { return mInGameId; }

    /**
     * prisinTerm is counted using seconds.
     */
            bool        putIntoPrison(time_t prisonTerm);
            bool        isPrisonTermExceeded() const;
            bool        freeFromPrison();
            bool        teleportToPrison();

            void        updatePlayerLabel();

    /**
     * If interior or world is -1, player's old settings of them will
     * be preserved.
     */
            void        teleportTo(float x, float y, float z, float facing,
                int world = -1, int interior = -1);
            void        teleportTo(int targetplayer);
            bool        teleportTo(const std::string& placeName);
            bool        createTeleport(const std::string& placeName);

            bool        createPrivateVehicle(int model);
            // bool        setPrivateVehicleLock(bool lock);
            // bool        isPrivateVehicleLocked() const;
            bool        dropPrivateVehicle();
            void        teleportPrivateVehicleToPlayer();

            int         getPrivateVehicleId() const
            { return mPrivateVehicle; }

    /**
     * CurrentGangZone status. Used by GangZoneBoxArea and
     */
            void        _setCurrentGangZone(std::shared_ptr<GangZone> zone)
            { mCurrentGangZone = zone; }

            void        _clearCurrentGangZone()
            { mCurrentGangZone.reset(); }

            bool        _isInGangZone() const
            { return mCurrentGangZone.get() != nullptr; }

            std::shared_ptr<GangZone>   _getCurrentGangZone() const
            { return mCurrentGangZone; }

    /**
     * Callbacks.
     */
            bool        onUpdate();
            bool        onSpawn();
            bool        onDeath();

    /**
     * ************ Crew System ************
     */
            bool        joinCrew(const mongo::OID& crewId);
            bool        quitCrew();

            mongo::OID  getCrew() const
            { return mCrew; }

            bool        isCrewMember() const
            { return mCrew.isSet(); }

protected:
    virtual bool        _parseObject(const mongo::BSONObj& data);
            void        _applyWantedLevel();
            bool        _validatePassword(const std::string& password);
};

}
