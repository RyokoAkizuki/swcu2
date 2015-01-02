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

#include <functional>

#include "../Interface/Dialog.hpp"

namespace swcu {

class CrewControlPanelDialog : public MenuDialog
{
public:
                    CrewControlPanelDialog(int playerid);
    virtual         ~CrewControlPanelDialog() {}

    virtual void    build();
};

class CrewViewMembersDialog : public ItemListDialog<std::string>
{
protected:
    mongo::OID      mCrew;     

public:
                    CrewViewMembersDialog(
        int playerid, const mongo::OID& crew);
    virtual         ~CrewViewMembersDialog() {}

    virtual void    build();
    virtual bool    process(std::string key);
};

class CrewEditMemberDialog : public MenuDialog
{
protected:
    mongo::OID      mCrew, mMember;     

public:
                    CrewEditMemberDialog(int playerid,
        const mongo::OID& crew, const mongo::OID& member);
    virtual         ~CrewEditMemberDialog() {}

    virtual void    build();
};

class CrewChangeNameDialog : public InputDialog
{
protected:
    mongo::OID      mCrew; 

public:
                    CrewChangeNameDialog(int playerid,
        const mongo::OID& crew);
    virtual         ~CrewChangeNameDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class CreateCrewDialog : public InputDialog
{
public:
                    CreateCrewDialog(int playerid);
    virtual         ~CreateCrewDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class CrewFindByNameDialog : public InputDialog
{
    typedef std::function<bool(const mongo::OID&)> CallbackType;

protected:
    CallbackType    mCallback;

public:
                    CrewFindByNameDialog(int playerid, CallbackType cb);
    virtual         ~CrewFindByNameDialog() {}

    virtual void    build();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

class _CrewFindByNameResultDialog : public ItemListDialog<std::string>
{
    typedef std::function<bool(const mongo::OID&)> CallbackType;

protected:
    std::string     mKeyWord;
    CallbackType    mCallback;

public:
                    _CrewFindByNameResultDialog(
        int playerid, const std::string& keyword, CallbackType cb);
    virtual         ~_CrewFindByNameResultDialog() {}

    virtual void    build();
    virtual bool    process(std::string key);
};

}
