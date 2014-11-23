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

class TestMessageDialog : public MessageDialog
{
public:
                    TestMessageDialog(int playerid) :
        InfoDialog(
            playerid,
            "Welcome to SWCU SA-MP server!",
            "This is a test message."
            )
    {}

    virtual         ~TestMessageDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, ":3");
        return true;
    }
};

class TestRadioDialog : public RadioListDialog<std::string>
{
public:
                    TestRadioDialog(int playerid) :
        RadioListDialog(
            playerid,
            "Radio List Dialog Test"
            )
    {
        addItem("a", "Morning");
        addItem("b", "Afternoon");
        addItem("c", "Night", true);
        addItem("d", "Midnight");
    }

    virtual bool    process(std::string key)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, key.c_str());
        return (key != "d");
    }
};

class TestCheckDialog : public CheckListDialog
{
private:
    bool    mGodMod, mFly, mUnlimitedAmmo, mSuperRun;
public:
                    TestCheckDialog(int playerid) :
        CheckListDialog(
            playerid,
            "Check List Dialog Test"
            ),
        mGodMod(false), mFly(true), mUnlimitedAmmo(false), mSuperRun(true)
    {
        addItem(
            "God Mod",
            [this]() { return mGodMod; },
            [this]() { return false; }
        );
        addItem(
            "Fly",
            [this]() { return mFly; },
            [this]() { mFly = !mFly; return true; }
        );
        addItem(
            "Unlimited Ammo",
            [this]() { return mUnlimitedAmmo; },
            [this]() { mUnlimitedAmmo = !mUnlimitedAmmo; return true; }
        );
        addItem(
            "Super Run",
            [this]() { return mSuperRun; },
            [this]() { mSuperRun = !mSuperRun; return true; }
        );
    }
};

}
