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
    addItem(
        t(p, DLG_EDIT_PROF_LOG_NAME) + p->getLogName(),
        [this]() {
            DialogManager::get().push<PlayerChangeLogNameDialog>(mPlayerId);
        });
    addItem(
        t(p, DLG_EDIT_PROF_PASSWORD),
        [this]() {
            DialogManager::get().push<PlayerChangePasswordDialog>(mPlayerId);
        });
    addItem(
        t(p, DLG_EDIT_PROF_NICKNAME) + p->getNickname() + "{FFFFFF}",
        [this]() {
            DialogManager::get().push<PlayerChangeNicknameDialog>(mPlayerId);
        });
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

}
