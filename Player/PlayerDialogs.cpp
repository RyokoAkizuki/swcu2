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
#include "../Multilang/Language.hpp"

#include "PlayerDialogs.hpp"

namespace swcu {

PlayerRegisterDialog::PlayerRegisterDialog(int playerid) :
    InputDialog(playerid,
        t(playerid, DLG_REG_TITLE),
        t(playerid, DLG_REG_MSG))
{
}

bool PlayerRegisterDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
{
    if(!response)
    {
        mMessage = t(mPlayerId, DLG_REG_MUST_CREATE);
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
        mMessage = t(p, DLG_REG_ERR);
        return false;
    }
}

PlayerLoginDialog::PlayerLoginDialog(int playerid) :
    InputDialog(playerid,
        t(playerid, DLG_LOG_TITLE),
        t(playerid, DLG_LOG_MSG))
{
}

bool PlayerLoginDialog::handleCallback(
     bool response, int listitem, const std::string &inputtext)
{
    if(!response)
    {
        mMessage = t(mPlayerId, DLG_LOG_MUST_LOGIN);
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
        mMessage = t(p, DLG_LOG_BAD_LOGIN);
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

bool PlayerEditProfileDialog::display()
{
    _buildItems();
    return MenuDialog::display();
}

void PlayerEditProfileDialog::_buildItems()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        return;
    }
    mItemList.clear();
    addItem(
        t(p, DLG_EDIT_PROF_LOG_NAME) + p->getLogName(),
        [this]() {
            DialogManager::get().push<PlayerChangeLogNameDialog>(mPlayerId);
            return true;
        });
    addItem(
        t(p, DLG_EDIT_PROF_PASSWORD),
        [this]() {
            DialogManager::get().push<PlayerChangePasswordDialog>(mPlayerId);
            return true;
        });
    addItem(
        t(p, DLG_EDIT_PROF_NICKNAME) + p->getNickname(),
        [this]() {
            DialogManager::get().push<PlayerChangeNicknameDialog>(mPlayerId);
            return true;
        });
}

PlayerViewProfileDialog::PlayerViewProfileDialog(
    int playerid, int targetplayer) :
    MessageDialog(playerid, "", "")
{
    auto tar = PlayerManager::get().getPlayer(targetplayer);
    auto p = PlayerManager::get().getPlayer(playerid);
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
                << tar->getNickname() << "\n"
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
    InputDialog(playerid,
        t(playerid, DLG_CHANGE_PASSWORD_TITLE),
        t(playerid, DLG_CHANGE_PASSWORD_MSG))
{
}

bool PlayerChangePasswordDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
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
        mMessage = t(p, DLG_CHANGE_PASSWORD_FAIL);
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
    InputDialog(playerid,
        t(playerid, DLG_CHANGE_LOGNAME_TITLE),
        t(playerid, DLG_CHANGE_LOGNAME_MSG))
{
}

bool PlayerChangeLogNameDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
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
        mMessage = t(p, DLG_CHANGE_LOGNAME_FAIL);
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
    InputDialog(playerid,
        t(playerid, DLG_CHANGE_NICKNAME_TITLE),
        t(playerid, DLG_CHANGE_NICKNAME_MSG))
{
}

bool PlayerChangeNicknameDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
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
        mMessage = t(p, DLG_CHANGE_NICKNAME_FAIL);
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            t(p, DLG_CHANGE_NICKNAME_SUCCESS));
        return true;
    }
}

}
