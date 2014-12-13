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
protected:
    enum State
    {
        INIT, MUST_REG, ERROR
    } mState;

public:
                    PlayerRegisterDialog(int playerid);
    virtual         ~PlayerRegisterDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerLoginDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, MUST_LOGIN, BAD_LOGIN
    } mState;

public:
                    PlayerLoginDialog(int playerid);
    virtual         ~PlayerLoginDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerEditProfileDialog : public MenuDialog
{
public:
                    PlayerEditProfileDialog(int playerid);
    virtual         ~PlayerEditProfileDialog() {}

    virtual void    build();
};

class PlayerViewProfileDialog : public MessageDialog
{
protected:
    int             mTargetPlayer;

public:
                    PlayerViewProfileDialog(int playerid, int targetplayer);
    virtual         ~PlayerViewProfileDialog() {}

    virtual void    build();
};

class PlayerChangePasswordDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, FAIL
    } mState;

public:
                    PlayerChangePasswordDialog(int playerid);
    virtual         ~PlayerChangePasswordDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerChangeLogNameDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, FAIL
    } mState;

public:
                    PlayerChangeLogNameDialog(int playerid);
    virtual         ~PlayerChangeLogNameDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerChangeNicknameDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, FAIL
    } mState;

public:
                    PlayerChangeNicknameDialog(int playerid);
    virtual         ~PlayerChangeNicknameDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

}
