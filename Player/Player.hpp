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

enum PlayerFlags
{
    STATUS_MUTED        = 0b00001000,
    STATUS_FREEZED      = 0b00000100,
    STATUS_JAILED       = 0b00000010,
    STATUS_BANNED       = 0b00000001,
    NO_FLAGS            = 0b00000000
};

class Player
{
protected:
    /**
     * Profile.
     */
    mongo::OID          mId;
    std::string         mIdStr;
    std::string         mLogName;
    std::string         mNickname;
    std::string         mPasswordHash;
    int                 mMoney;
    int                 mAdminLevel;
    int                 mFlags;
    int64_t             mJoinTime;
    int64_t             mGameTime;
    int                 mLanguage;

    bool                mRegistered;

    /**
     * Gaming.
     */
    int                 mInGameId;
    int64_t             mTimeEnteredServer;
    bool                mLoggedIn;

    /**
     * Houses, Weapons, Vehicles, etc.
     */

public:
                        Player(int gameid);
    virtual             ~Player();

    /**
     * ************ Profile ************
     */

            mongo::OID  getId() const
            { return mId;}

            std::string getIdStr() const
            { return mIdStr; }

            int64_t     getJoinTime() const
            { return mJoinTime; }

            int64_t     getGameTime() const
            { return mGameTime; }

    /**
     * As literal meaning.
     * @return True if player is registered.
     *         False if player don't have a profile
     *         or database failed while initial loading phase.
     */
            bool        isRegistered() const
            { return mRegistered; }

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
    /**
     * Load player profile from database.
     * @return True if succeeded.
     *         False if player didn't registered or database fails.
     */
            bool        loadProfile();

            bool        isLoggedIn() const
            { return mLoggedIn; }

    /**
     * Set player's logged in flag.
     * Note: You CAN'T set it to true if player isn't registered.
     * @param  logged Should player be set logged in or not.
     * @return        Nothing.
     */
            void        setLoggedIn(bool logged)
            { mLoggedIn = logged && mRegistered; }

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

            int         getLanguage() const
            { return mLanguage; }

            bool        setLanguage(int lang);

protected:

            void        _loadProfile(const mongo::BSONObj& doc);

    template<typename T>
            bool        _updateField(const char* operation,
                const char* fieldname, T value)
    {
        if(!mRegistered)
        {
            LOG(ERROR) << "Player is unregistered."
                " Can't perform _updateField.";
            return false;
        }
        MONGO_WRAPPER({
            getDBConn()->update(
                Config::colNamePlayer,
                BSON("_id" << mId),
                BSON(operation << BSON(fieldname << value))
                );
            auto err = getDBConn()->getLastError();
            if (err.size())
            {
                LOG(ERROR) << "Failed to " << operation << " field "
                    << fieldname << " to " << value;
                return false;
            }
            return true;
        });
        return false;
    }

            bool        _validatePassword(const std::string& password);
};

}
