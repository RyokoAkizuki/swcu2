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

#include "PlayerDialogs.hpp"

namespace swcu {

PlayerRegisterDialog::PlayerRegisterDialog(int playerid) :
    InputDialog(playerid,
        "Welcome to SWCU",
        "Please enter a password to create your profile.\n"
        "The password must be longer than 6 characters.")
{
}

bool PlayerRegisterDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
{
    if(!response)
    {
        mMessage = "You must have a profile to join the game.";
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
            "You have a profile and don't have to create a new one.");
        return true;
    }
    if(p->createProfile(inputtext))
    {
        p->setLoggedIn(true);
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            "Your profile has been created.");
        return true;
    }
    else
    {
        mMessage = "Your password is illegal "
            "or a database error is occurred.\nPlease retry.";
        return false;
    }
}

PlayerLoginDialog::PlayerLoginDialog(int playerid) :
    InputDialog(playerid,
        "Back to SWCU",
        "Please enter your password.")
{
}

bool PlayerLoginDialog::handleCallback(
     bool response, int listitem, const std::string &inputtext)
{
    if(!response)
    {
        mMessage = "You must login to join the game.";
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
            "You've been logged in. You don't have to login twice.");
        return true;
    }
    if(p->verifyPassword(inputtext))
    {
        p->setLoggedIn(true);
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "You've logged in.");
        return true;
    }
    else
    {
        mMessage = "Your password is incorrect. Please retry.";
        return false;
    }
}

PlayerEditProfileDialog::PlayerEditProfileDialog(int playerid) :
    MenuDialog(playerid, "My Profile")
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
        SendClientMessage(playerid, 0xFFFFFFFF, "You haven't logged in.");
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
        "My Login Name: " + p->getLogName(),
        [this]() {
            DialogManager::get().push<PlayerChangeLogNameDialog>(mPlayerId);
            return true;
        });
    addItem(
        "Change Password",
        [this]() {
            DialogManager::get().push<PlayerChangePasswordDialog>(mPlayerId);
            return true;
        });
    addItem(
        "My Nickname: " + p->getNickname(),
        [this]() {
            DialogManager::get().push<PlayerChangeNicknameDialog>(mPlayerId);
            return true;
        });
}

PlayerViewProfileDialog::PlayerViewProfileDialog(
    int playerid, int targetplayer) :
    MessageDialog(playerid, "", "")
{
    auto t = PlayerManager::get().getPlayer(targetplayer);
    if(t == nullptr)
    {
        mTitle = "Player's Profile";
        mMessage = "Player not found.";
    }
    else
    {
        mTitle = t->getLogName(); mTitle += "'s Profile";
        if(t->isLoggedIn())
        {
            std::stringstream msg;
            msg << "ID: "               << t->getIdStr()
                << "\nLogin Name: "     << t->getLogName()
                << "\nNickname: "       << t->getNickname()
                << "\nJoin Time: "      << t->getJoinTime()
                << "\nPlaying Time: "   << t->getGameTime()
                << "\nAdmin Level: "    << t->getAdminLevel();
            mMessage = msg.str();
        }
        else
        {
            mMessage = "Player is not logged in.";
        }
    }
}

PlayerChangePasswordDialog::PlayerChangePasswordDialog(int playerid) :
    InputDialog(playerid,
        "Change Password",
        "Please enter a new password below "
        "containing more than 6 characters.")
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
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "You haven't logged in.");
        return true;
    }
    if(!p->changePassword(inputtext))
    {
        mMessage = "Your password is illegal "
            "or a database error is occurred.\nPlease retry.";
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            "Your password is changed.");
        return true;
    }
}

PlayerChangeLogNameDialog::PlayerChangeLogNameDialog(int playerid) :
    InputDialog(playerid,
        "Change Login Name",
        "Please enter the new name you desired.")
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
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "You haven't logged in.");
        return true;
    }
    if(!p->setLogName(inputtext))
    {
        mMessage = "The name you used contains illegal characters\n"
            "or a database error is occurred.\nPlease retry.";
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            "Your login name is changed.");
        return true;
    }
}

PlayerChangeNicknameDialog::PlayerChangeNicknameDialog(int playerid) :
    InputDialog(playerid,
        "Change Nickname",
        "Please enter the new name you desired.")
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
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "You haven't logged in.");
        return true;
    }
    if(!p->setNickname(inputtext))
    {
        mMessage = "The name you used contains illegal characters\n"
            "or a database error is occurred.\nPlease retry.";
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            "Your nickname is changed.");
        return true;
    }
}

}
