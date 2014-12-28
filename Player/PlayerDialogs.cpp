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
#include <sstream>

#include "PlayerManager.hpp"
#include "../Interface/DialogManager.hpp"
#include "../Map/MapDialogs.hpp"
#include "../Multilang/Language.hpp"
#include "../Weapon/WeaponShopDialog.hpp"
#include "../GangZone/GangZoneManager.hpp"

#include "PlayerDialogs.hpp"

namespace swcu {

PlayerRegisterDialog::PlayerRegisterDialog(int playerid) :
    InputDialog(playerid, t(playerid, DLG_REG_TITLE)), mState(INIT)
{
}

void PlayerRegisterDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage(t(mPlayerId, DLG_REG_MSG));
        break;
        case MUST_REG:
        setMessage(t(mPlayerId, DLG_REG_MUST_CREATE));
        break;
        case ERROR:
        setMessage(t(mPlayerId, DLG_REG_ERR));
        break;
    }
}
bool PlayerRegisterDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        mState = MUST_REG;
        return false;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "Player not found.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(p->isRegistered())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_REG_ALREADY_REGISTERED));
        return true;
    }
    if(p->createProfile(inputtext))
    {
        p->setLoggedIn(true);
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_REG_SUCCESS));
        return true;
    }
    else
    {
        mState = ERROR;
        return false;
    }
}

PlayerLoginDialog::PlayerLoginDialog(int playerid) :
    InputDialog(playerid, t(playerid, DLG_LOG_TITLE)), mState(INIT)
{
}

void PlayerLoginDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage(t(mPlayerId, DLG_LOG_MSG));
        break;
        case MUST_LOGIN:
        setMessage(t(mPlayerId, DLG_LOG_MUST_LOGIN));
        break;
        case BAD_LOGIN:
        setMessage(t(mPlayerId, DLG_LOG_BAD_LOGIN));
        break;
    }
}

bool PlayerLoginDialog::handleCallback(
     bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        mState = MUST_LOGIN;
        return false;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "Player not found.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_LOG_LOG_TWICE));
        return true;
    }
    if(p->verifyPassword(inputtext))
    {
        p->setLoggedIn(true);
        SendClientMessage(mPlayerId, 0xFFFFFFFF, t(p, DLG_LOG_SUCCESS));
        return true;
    }
    else
    {
        mState = BAD_LOGIN;
        return false;
    }
}

PlayerEditProfileDialog::PlayerEditProfileDialog(int playerid) :
    MenuDialog(playerid, t(playerid, DLG_EDIT_PROF_TITLE))
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p == nullptr)
    {
        SendClientMessage(playerid, 0xFFFFFFFF, "Player not found.");
        LOG(ERROR) << "Player not found.";
        return;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(playerid, 0xFFFFFFFF, t(p, NOT_LOGGED_IN));
        return;
    }
}

void PlayerEditProfileDialog::build()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        return;
    }
    mItemList.clear();
    int playerid = mPlayerId;
    addItem(
        t(p, DLG_EDIT_PROF_LOG_NAME) + p->getLogName(),
        [playerid]() {
            DialogManager::get().push<PlayerChangeLogNameDialog>(playerid);
        });
    addItem(
        t(p, DLG_EDIT_PROF_PASSWORD),
        [playerid]() {
            DialogManager::get().push<PlayerChangePasswordDialog>(playerid);
        });
    addItem(
        t(p, DLG_EDIT_PROF_NICKNAME) + p->getNickname() + "{FFFFFF}",
        [playerid]() {
            DialogManager::get().push<PlayerChangeNicknameDialog>(playerid);
        });
    {
        std::stringstream al;
        al << t(p, DLG_EDIT_PROF_ADMINLEVEL) << p->getAdminLevel();
        addItem(al.str(), []() {});
    }
    addItem(t(p, DLG_EDIT_PROF_POLICERANK) + p->getPoliceRankStr(), [](){});
}

PlayerViewProfileDialog::PlayerViewProfileDialog(
    int playerid, int targetplayer) :
    MessageDialog(playerid, ""), mTargetPlayer(targetplayer)
{
}

void PlayerViewProfileDialog::build()
{
    auto tar = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(tar == nullptr || p == nullptr)
    {
        mTitle = "Player's Profile";
        mMessage = "Player not found.";
    }
    else
    {
        mTitle = tar->getLogName();
        mTitle += t(p, DLG_VIEW_PROF_TITLE_POSTFIX);
        if(tar->isLoggedIn())
        {
            std::stringstream msg;
            msg << t(p, DLG_VIEW_PROF_ID)
                << tar->getIdStr() << "\n"
                << t(p, DLG_VIEW_PROF_LOGNAME)
                << tar->getLogName() << "\n"
                << t(p, DLG_VIEW_PROF_NICKNAME)
                // In case of colored text, reset color at the end.
                << tar->getNickname() << "{FFFFFF}\n"
                << t(p, DLG_VIEW_PROF_JOINTIME)
                << tar->getJoinTime() << "\n"
                << t(p, DLG_VIEW_PROF_GAMETIME)
                << tar->getGameTime() << "\n"
                << t(p, DLG_VIEW_PROF_ADMINLEVEL)
                << tar->getAdminLevel();
            mMessage = msg.str();
        }
        else
        {
            mMessage = t(p, DLG_VIEW_PROF_TARGET_NOT_LOGGED_IN);
        }
    }
}

PlayerChangePasswordDialog::PlayerChangePasswordDialog(int playerid) :
    InputDialog(playerid, t(playerid, DLG_CHANGE_PASSWORD_TITLE)),
    mState(INIT)
{
}

void PlayerChangePasswordDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage(t(mPlayerId, DLG_CHANGE_PASSWORD_MSG));
        break;
        case FAIL:
        setMessage(t(mPlayerId, DLG_CHANGE_PASSWORD_FAIL));
        break;
    }
}

bool PlayerChangePasswordDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "Player not found.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, t(p, NOT_LOGGED_IN));
        return true;
    }
    if(!p->changePassword(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_CHANGE_PASSWORD_SUCCESS));
        return true;
    }
}

PlayerChangeLogNameDialog::PlayerChangeLogNameDialog(int playerid) :
    InputDialog(playerid, t(playerid, DLG_CHANGE_LOGNAME_TITLE)),
    mState(INIT)
{
}

void PlayerChangeLogNameDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage(t(mPlayerId, DLG_CHANGE_LOGNAME_MSG));
        break;
        case FAIL:
        setMessage(t(mPlayerId, DLG_CHANGE_LOGNAME_FAIL));
        break;
    }
}

bool PlayerChangeLogNameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "Player not found.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, t(p, NOT_LOGGED_IN));
        return true;
    }
    if(!p->setLogName(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_CHANGE_LOGNAME_SUCCESS));
        return true;
    }
}

PlayerChangeNicknameDialog::PlayerChangeNicknameDialog(int playerid) :
    InputDialog(playerid, t(playerid, DLG_CHANGE_NICKNAME_TITLE)),
    mState(INIT)
{
}

void PlayerChangeNicknameDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage(t(mPlayerId, DLG_CHANGE_NICKNAME_MSG));
        break;
        case FAIL:
        setMessage(t(mPlayerId, DLG_CHANGE_NICKNAME_FAIL));
        break;
    }
}

bool PlayerChangeNicknameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "Player not found.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, t(p, NOT_LOGGED_IN));
        return true;
    }
    if(!p->setNickname(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_CHANGE_NICKNAME_SUCCESS));
        return true;
    }
}

PlayerControlDialog::PlayerControlDialog(int playerid, int targetplayer)
    : MenuDialog(playerid, t(playerid, DLG_PLAYER_CTL_TITLE)),
    mTargetPlayer(targetplayer)
{
}

void PlayerControlDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return;
    }
    setTitle(target->getLogName());

    int playerid = mPlayerId, targetid = mTargetPlayer;

    // General Functions
    // * Private Message
    addItem(t(p, DLG_PLAYER_CTL_SENDMSG), [playerid, targetid]() {
        DialogManager::get().push<PlayerSendMessageDialog>(
            playerid, targetid
        );
    });
    // * View Profile
    addItem(t(p, DLG_PLAYER_CTL_VIEWPROFILE), [playerid, targetid]() {
        DialogManager::get().push<PlayerViewProfileDialog>(
            playerid, targetid
        );
    });
    // * Spectate

    // Police Rank POLICE_OFFICER
    int rank = p->getPoliceRank();

    if(rank > CIVILIAN && target->getWantedLevel() > 0)
    {
        float x, y, z;
        GetPlayerPos(targetid, &x, &y, &z);
        float d = GetPlayerDistanceFromPoint(playerid, x, y, z);
        if(d < 10.0)
        {
            addItem(t(p, DLG_PLAYER_CTL_ARREST), [playerid, targetid]() {
                DialogManager::get().push<ArrestDialog>(playerid, targetid);
            });
        }
    }

    if(rank >= POLICE_OFFICER && rank < POLICE_DEPUTY_CHIEF)
    {
        if(!target->hasFlags(STATUS_JAILED))
        {
            addItem(t(p, DLG_PLAYER_CTL_SETWANTED), [playerid, targetid]() {
                DialogManager::get().push<PlayerSetWantedLevelDialog>(
                    playerid, targetid, 1, 2
                );
            });
        }
    }

    // Police Rank POLICE_DEPUTY_CHIEF
    if(rank >= POLICE_DEPUTY_CHIEF && rank < CHIEF_OF_POLICE)
    {
        if(!target->hasFlags(STATUS_JAILED))
        {
            addItem(t(p, DLG_PLAYER_CTL_SETWANTED), [playerid, targetid]() {
                DialogManager::get().push<PlayerSetWantedLevelDialog>(
                    playerid, targetid, 0, 4
                );
            });
        }
    }

    // Police Rank CHIEF_OF_POLICE
    if(rank == CHIEF_OF_POLICE)
    {
        if(!target->hasFlags(STATUS_JAILED))
        {
            addItem(t(p, DLG_PLAYER_CTL_SETWANTED), [playerid, targetid]() {
                DialogManager::get().push<PlayerSetWantedLevelDialog>(
                    playerid, targetid, 0, 6
                );
            });
        }
    }

    // Police Rank >= POLICE_DEPUTY_CHIEF
    if(rank >= POLICE_DEPUTY_CHIEF)
    {
        if(target->hasFlags(STATUS_JAILED))
        {
            addItem(t(p, DLG_PLAYER_CTL_FREE), [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->freeFromPrison();
            });
        }
    }

    // Admin Level 1
    if(p->getAdminLevel() >= 1)
    {
        // * Go To
        addItem(t(p, DLG_PLAYER_CTL_GOTO), [playerid, targetid]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p != nullptr) p->teleportTo(targetid);
        });
        // * Get There
        addItem(t(p, DLG_PLAYER_CTL_GETTHERE), [playerid, targetid]() {
            auto target = PlayerManager::get().getPlayer(targetid);
            if(target != nullptr) target->teleportTo(playerid);
        });
        // * Mute / Unmute
        if(target->hasFlags(STATUS_MUTED))
        {
            addItem(t(p, DLG_PLAYER_CTL_UNMUTE), [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->removeFlags(STATUS_MUTED);
            });
        }
        else
        {
            addItem(t(p, DLG_PLAYER_CTL_MUTE), [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->addFlags(STATUS_MUTED);
            });
        }
        // * Eject
        if(IsPlayerInAnyVehicle(targetid))
        {
            addItem(t(p, DLG_PLAYER_CTL_EJECT),
                std::bind(&RemovePlayerFromVehicle, targetid)
            );
        }
    }

    // Admin Level 2
    if(p->getAdminLevel() >= 2)
    {
        // * Freeze / Unfreeze
        if(target->hasFlags(STATUS_FREEZED))
        {
            addItem(t(p, DLG_PLAYER_CTL_UNFREEZE), [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->removeFlags(STATUS_FREEZED);
                TogglePlayerControllable(targetid, 1);
            });
        }
        else
        {
            addItem(t(p, DLG_PLAYER_CTL_FREEZE), [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->addFlags(STATUS_FREEZED);
                TogglePlayerControllable(targetid, 0);
            });
        }
        // * Kill
        addItem(t(p, DLG_PLAYER_CTL_KILL),
            std::bind(&SetPlayerHealth, targetid, 0.0)
        );
        // * Force Respawn
        addItem(t(p, DLG_PLAYER_CTL_FORCERESPAWN), [targetid]() {
            ForceClassSelection(targetid);
            SetPlayerHealth(targetid, 0.0);
        });
        // * Kick
        addItem(t(p, DLG_PLAYER_CTL_KICK),
            std::bind(&Kick, targetid)
        );
    }

    // Admin Level 3
    if(p->getAdminLevel() >= 3)
    {
        // * Explode
        addItem(t(p, DLG_PLAYER_CTL_EXPLODE), [targetid]() {
            float x, y, z;
            GetPlayerPos(targetid, &x, &y, &z);
            CreateExplosion(x, y, z, 0, 5.0);
        });
        // * Ban
        addItem(t(p, DLG_PLAYER_CTL_BAN), [targetid]() {
            auto target = PlayerManager::get().getPlayer(targetid);
            if(target != nullptr) target->addFlags(STATUS_BANNED);
            Kick(targetid);
        });
        // * Set Admin Level
        addItem(t(p, DLG_PLAYER_CTL_SET_ADMIN),
        [playerid, targetid]() {
            DialogManager::get().push<PlayerSetAdminLevelDialog>(
                playerid, targetid
            );
        });
        // * Set Police Rank
        addItem(t(p, DLG_PLAYER_CTL_SET_POLICE),
        [playerid, targetid]() {
            DialogManager::get().push<PlayerSetPoliceRankDialog>(
                playerid, targetid
            );
        });
    }
}

PlayerSendMessageDialog::PlayerSendMessageDialog(int playerid, int target) :
    InputDialog(playerid, t(playerid, DLG_PM_TITLE)), mTargetPlayer(target)
{
}

void PlayerSendMessageDialog::build()
{
    setMessage(t(mPlayerId, DLG_PM_MSG));
}

bool PlayerSendMessageDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return true;
    }
    if(!IsPlayerConnected(mTargetPlayer))
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, PLAYER_NOT_FOUND));
        return true;
    }
    std::stringstream info;
    info << t(p, DLG_PM_SENT) << target->getNickname()
        << "{FFFFFF}(" << mTargetPlayer << "): " << inputtext;
    SendClientMessage(mPlayerId, 0xFFFFFFFF, info.str().c_str());
    std::stringstream msg;
    msg << t(target, DLG_PM_RECEIVE) << p->getNickname()
        << "{FFFFFF}(" << mPlayerId << "): " << inputtext;
    SendClientMessage(mTargetPlayer, 0xFFFFFFFF, msg.str().c_str());
    LOG(INFO) << "Private message sent from " << p->getLogName()
        << " to " << target->getLogName() << ": " << inputtext;
    return false;
}

PlayerSetAdminLevelDialog::PlayerSetAdminLevelDialog(int playerid,
    int target) : RadioListDialog<int>(playerid,
    t(playerid, DLG_SET_ADMIN_LEVEL_TITLE)), mTargetPlayer(target)
{
}

void PlayerSetAdminLevelDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return;
    }
    addItem(0, t(p, ADMIN_LEVEL_0), target->getAdminLevel() == 0);
    addItem(1, t(p, ADMIN_LEVEL_1), target->getAdminLevel() == 1);
    addItem(2, t(p, ADMIN_LEVEL_2), target->getAdminLevel() == 2);
    addItem(3, t(p, ADMIN_LEVEL_3), target->getAdminLevel() == 3);
}

bool PlayerSetAdminLevelDialog::process(int adminlevel)
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    if(target != nullptr)
    {
        return target->setAdminLevel(adminlevel);
    }
    return false;
}

PlayerSetPoliceRankDialog::PlayerSetPoliceRankDialog(int playerid,
    int target) : RadioListDialog<PoliceRank>(playerid,
    t(playerid, DLG_SET_POLICE_RANK_TITLE)), mTargetPlayer(target)
{
}

void PlayerSetPoliceRankDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    if(target == nullptr)
    {
        return;
    }
    addItem(CIVILIAN, PoliceRankString[CIVILIAN],
        target->getPoliceRank() == CIVILIAN);
    addItem(POLICE_OFFICER, PoliceRankString[POLICE_OFFICER],
        target->getPoliceRank() == POLICE_OFFICER);
    addItem(POLICE_DEPUTY_CHIEF, PoliceRankString[POLICE_DEPUTY_CHIEF],
        target->getPoliceRank() == POLICE_DEPUTY_CHIEF);
    addItem(CHIEF_OF_POLICE, PoliceRankString[CHIEF_OF_POLICE],
        target->getPoliceRank() == CHIEF_OF_POLICE);
}

bool PlayerSetPoliceRankDialog::process(PoliceRank rank)
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    if(target != nullptr)
    {
        return target->setPoliceRank(rank);
    }
    return false;
}

PlayerControlPanelDialog::PlayerControlPanelDialog(int playerid) :
    MenuDialog(playerid, t(playerid, DLG_CTLPANEL_TITLE))
{
}

void PlayerControlPanelDialog::build()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        return;
    }
    int playerid = mPlayerId;
    // General Functions
    // * Weapon Shop
    addItem(t(p, DLG_CTLPANEL_WEAPONSHOP), [playerid]() {
        DialogManager::get().push<WeaponShopDialog>(playerid);
    });
    // * Create Teleport
    addItem(t(p, DLG_CTLPANEL_CREATE_TP), [playerid]() {
        DialogManager::get().push<CreateTeleportDialog>(playerid);
    });
    // * Profile
    addItem(t(p, DLG_CTLPANEL_PROFILE), [playerid]() {
        DialogManager::get().push<PlayerEditProfileDialog>(playerid);
    });
    // * Languages
    addItem(t(p, DLG_CTLPANEL_LANG), [playerid]() {
        DialogManager::get().push<PlayerSelectLanguageDialog>(playerid);
    });
    // Admin Level 3
    // * Map Manager
    if(p->getAdminLevel() >= 3)
    {
        addItem(t(p, DLG_CTLPANEL_MAP_MGR), [playerid]() {
            DialogManager::get().push<MapManagerDialog>(playerid);
        });
        addItem(t(p, DLG_CTLPANEL_RELOAD_ZONES), []() {
            GangZoneManager::get().loadAll();
            GangZoneManager::get().showAll();
        });
    }
    // Police System
    if(p->getWantedLevel() > 0)
    {
        addItem(t(p, DLG_CTLPANEL_SURRENDER), [playerid]() {
            DialogManager::get().push<ArrestSurrenderDialog>(playerid);
        });
    }
}

PlayerSetWantedLevelDialog::PlayerSetWantedLevelDialog(int playerid,
    int target, int minlevel, int maxlevel) :
    RadioListDialog<int>(playerid, t(playerid, DLG_SET_WANDTED_TITLE)),
    mTargetPlayer(target), mMin(minlevel < 0 ? 0 : minlevel),
    mMax(maxlevel > 6 ? 6 : maxlevel)
{
}

void PlayerSetWantedLevelDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return;
    }
    if(0 >= mMin && 0 <= mMax)
        addItem(0, t(p, WANDTED_LEVEL_0), target->getWantedLevel() == 0);
    if(1 >= mMin && 1 <= mMax)
        addItem(1, t(p, WANDTED_LEVEL_1), target->getWantedLevel() == 1);
    if(2 >= mMin && 2 <= mMax)
        addItem(2, t(p, WANDTED_LEVEL_2), target->getWantedLevel() == 2);
    if(3 >= mMin && 3 <= mMax)
        addItem(3, t(p, WANDTED_LEVEL_3), target->getWantedLevel() == 3);
    if(4 >= mMin && 4 <= mMax)
        addItem(4, t(p, WANDTED_LEVEL_4), target->getWantedLevel() == 4);
    if(5 >= mMin && 5 <= mMax)
        addItem(5, t(p, WANDTED_LEVEL_5), target->getWantedLevel() == 5);
    if(6 >= mMin && 6 <= mMax)
        addItem(6, t(p, WANDTED_LEVEL_6), target->getWantedLevel() == 6);
}

bool PlayerSetWantedLevelDialog::process(int level)
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return false;
    }
    // You can't down set wanted level if it's beyond your authority.
    if(target->getWantedLevel() > level && target->getWantedLevel() > mMax)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_SET_WANTED_BEYOND_AUTH));
        return false;
    }
    target->setWantedLevel(level);
    if(level > 0)
    {
        DialogManager::get().push<ArrestSurrenderDialog>(mTargetPlayer);
    }
    return true;
}

PlayerSelectLanguageDialog::PlayerSelectLanguageDialog(int playerid) :
    RadioListDialog<Languages>(playerid, t(playerid, DLG_SELECT_LANG_TITLE))
{
}

void PlayerSelectLanguageDialog::build()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        return;
    }
    addItem(ZH_CN, "ÖÐÎÄ", p->getLanguage() == ZH_CN);
    addItem(EN_US, "English", p->getLanguage() == EN_US);
}

bool PlayerSelectLanguageDialog::process(Languages language)
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        return false;
    }
    return p->setLanguage(language);
}

ArrestSurrenderDialog::ArrestSurrenderDialog(int playerid) :
    ConfirmDialog(playerid, t(playerid, DLG_SURRENDER_TITLE))
{
}

void ArrestSurrenderDialog::build()
{
    setMessage(t(mPlayerId, DLG_SURRENDER_MSG));
}

bool ArrestSurrenderDialog::handleCallback(
    bool response, int /* listitem */, const std::string& /* inputtext */)
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr) return true;
    if(response && p->getWantedLevel() > 0)
    {
        p->putIntoPrison(60 * 2);
    }
    return true;
}

ArrestDialog::ArrestDialog(int playerid, int target) :
    MenuDialog(playerid, t(playerid, DLG_ARREST_TITLE)),
    mTarget(target)
{
}

void ArrestDialog::build()
{
    int targetid = mTarget;
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr) return;
    addItem(t(p, DLG_ARREST_5MIN), [targetid]() {
        auto target = PlayerManager::get().getPlayer(targetid);
        if(target != nullptr && target->getWantedLevel() > 0)
            target->putIntoPrison(60 * 5);
    });
    addItem(t(p, DLG_ARREST_10MIN), [targetid]() {
        auto target = PlayerManager::get().getPlayer(targetid);
        if(target != nullptr && target->getWantedLevel() > 0)
            target->putIntoPrison(60 * 10);
    });
    addItem(t(p, DLG_ARREST_15MIN), [targetid]() {
        auto target = PlayerManager::get().getPlayer(targetid);
        if(target != nullptr && target->getWantedLevel() > 0)
            target->putIntoPrison(60 * 15);
    });
}

bool ArrestDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
{
    MenuDialog::handleCallback(response, listitem, inputtext);
    return true;
}

CreateTeleportDialog::CreateTeleportDialog(int playerid) :
    InputDialog(playerid, t(playerid, DLG_CREATE_TP_TITLE))
{
}

void CreateTeleportDialog::build()
{
    setMessage(t(mPlayerId, DLG_CREATE_TP_MESSAGE));
}

bool CreateTeleportDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response) return true;
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr) return false;
    return p->createTeleport(inputtext);
}

}
