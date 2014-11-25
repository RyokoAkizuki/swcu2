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

#include "../Interface/Dialog.hpp"

namespace swcu {

class PlayerRegisterDialog : public InputDialog
{
public:
                    PlayerRegisterDialog(int playerid);
    virtual         ~PlayerRegisterDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerLoginDialog : public InputDialog
{
public:
                    PlayerLoginDialog(int playerid);
    virtual         ~PlayerLoginDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerEditProfileDialog : public MenuDialog
{
public:
                    PlayerEditProfileDialog(int playerid);
    virtual         ~PlayerEditProfileDialog() {}

    virtual bool    display();
            void    _buildItems();
};

class PlayerViewProfileDialog : public MessageDialog
{
public:
                    PlayerViewProfileDialog(int playerid, int targetplayer);
    virtual         ~PlayerViewProfileDialog() {}
};

class PlayerChangePasswordDialog : public InputDialog
{
public:
                    PlayerChangePasswordDialog(int playerid);
    virtual         ~PlayerChangePasswordDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerChangeLogNameDialog : public InputDialog
{
public:
                    PlayerChangeLogNameDialog(int playerid);
    virtual         ~PlayerChangeLogNameDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerChangeNicknameDialog : public InputDialog
{
public:
                    PlayerChangeNicknameDialog(int playerid);
    virtual         ~PlayerChangeNicknameDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

}
