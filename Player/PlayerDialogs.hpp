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
 
#include "../Interface/Dialog.hpp"

namespace swcu {

class PlayerRegisterDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, MUST_REG, ERROR, FINISH
    } mState;

public:
                    PlayerRegisterDialog(int playerid);
    virtual         ~PlayerRegisterDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerLoginDialog : public InputDialog
{
protected:
    enum State
    {
        INIT, MUST_LOGIN, BAD_LOGIN, FINISH
    } mState;

public:
                    PlayerLoginDialog(int playerid);
    virtual         ~PlayerLoginDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerEditProfileDialog : public MenuDialog
{
public:
                    PlayerEditProfileDialog(int playerid);
    virtual         ~PlayerEditProfileDialog() {}

    virtual bool    build();
};

class PlayerViewProfileDialog : public MessageDialog
{
protected:
    int             mTargetPlayer;

public:
                    PlayerViewProfileDialog(int playerid, int targetplayer);
    virtual         ~PlayerViewProfileDialog() {}

    virtual bool    build();
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

    virtual bool    build();
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

    virtual bool    build();
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

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerControlDialog : public MenuDialog
{
protected:
    int             mTargetPlayer;

public:
                    PlayerControlDialog(int playerid, int targetplayer);
    virtual         ~PlayerControlDialog() {}

    virtual bool    build();
};

class PlayerSendMessageDialog : public InputDialog
{
protected:
    int             mTargetPlayer;

public:
                    PlayerSendMessageDialog(int playerid, int target);
    virtual         ~PlayerSendMessageDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class PlayerSetAdminLevelDialog : public RadioListDialog<int>
{
protected:
    int             mTargetPlayer;

public:
                    PlayerSetAdminLevelDialog(int playerid, int target);
    virtual         ~PlayerSetAdminLevelDialog() {}

    virtual bool    build();
    virtual bool    process(int adminlevel);
};

class PlayerSetPoliceRankDialog : public RadioListDialog<PoliceRank>
{
protected:
    int             mTargetPlayer;

public:
                    PlayerSetPoliceRankDialog(int playerid, int target);
    virtual         ~PlayerSetPoliceRankDialog() {}

    virtual bool    build();
    virtual bool    process(PoliceRank rank);
};

class PlayerControlPanelDialog : public MenuDialog
{
public:
                    PlayerControlPanelDialog(int playerid);
    virtual         ~PlayerControlPanelDialog() {}

    virtual bool    build();
};

class PlayerSetWantedLevelDialog : public RadioListDialog<int>
{
protected:
    int             mTargetPlayer, mMin, mMax;

public:
                    PlayerSetWantedLevelDialog(int playerid, int target,
                        int minlevel, int maxlevel);
    virtual         ~PlayerSetWantedLevelDialog() {}

    virtual bool    build();
    virtual bool    process(int level);
};

class ArrestSurrenderDialog : public ConfirmDialog
{
public:
                    ArrestSurrenderDialog(int playerid);
    virtual         ~ArrestSurrenderDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class ArrestDialog : public MenuDialog
{
protected:
    int             mTarget;

public:
                    ArrestDialog(int playerid, int target);
    virtual         ~ArrestDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class CreateTeleportDialog : public InputDialog
{
public:
                    CreateTeleportDialog(int playerid);
    virtual         ~CreateTeleportDialog() {}

    virtual bool    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

}
