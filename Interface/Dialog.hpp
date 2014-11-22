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

#include <sampgdk/a_samp.h>
#include <stack>
#include <sstream>
#include <functional>
#include <vector>

namespace swcu {

class Dialog
{
protected:
    int             mPlayerId;
    int             mId;

    static int      sLastId;

public:
                    Dialog(int playerid);
    virtual         ~Dialog() {}

            int     getIdentity();
    virtual bool    display() = 0;
    virtual bool    handleCallback(int playerid, int dialogid,
        int response, int listitem, const std::string &inputtext) = 0;
};

class MessageDialog : public Dialog
{
protected:
    std::string     mTitle, mMessage, mBtn1, mBtn2;
    bool            mInput, mMasked;

public:
                    MessageDialog(
        int playerid, const std::string &title,
        const std::string &msg, const std::string &btn1,
        const std::string &btn2, bool acceptInput = false,
        bool maskedInput = false);
    virtual         ~MessageDialog() {}
    virtual bool    display();
    virtual bool    handleCallback(int playerid, int dialogid,
        int response, int listitem, const std::string &inputtext);
};

class ListDialog : public Dialog
{
protected:
    std::string     mTitle, mBtn1, mBtn2;

    typedef         std::function<bool(bool)>               ItemCallback;
    typedef         std::pair<std::string, ItemCallback>    Item;
    typedef         std::vector<Item>                       ItemList;

    ItemList        mItemList;

public:
                    ListDialog(
        int playerid, const std::string &title,
        const std::string &btn1, const std::string &btn2);
    virtual         ~ListDialog() {}
    virtual void    addItem(const std::string &title, ItemCallback cb);
    virtual bool    display();
    virtual bool    handleCallback(int playerid, int dialogid,
        int response, int listitem, const std::string &inputtext);
};

}
