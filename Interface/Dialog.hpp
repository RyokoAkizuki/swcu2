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

#include <functional>
#include <vector>
#include <sampgdk/a_samp.h>

#include "../Common/Common.hpp"
#include "../Multilang/Language.hpp"
 
namespace swcu {

class Dialog
{
protected:
    int             mPlayerId;
    std::string     mTitle;

public:
                    Dialog(
                        int playerid,
                        const std::string &title
                    ) : mPlayerId(playerid), mTitle(title) {}
    virtual         ~Dialog() {}

    virtual bool    display() = 0;
    /**
     * The return value indicates whether the action is completed
     * and the dialog can pop from the stack.
     */
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext) = 0;
};

enum DialogType
{
    DIALOG_TYPE_MESSAGE,
    DIALOG_TYPE_CONFIRM,
    DIALOG_TYPE_INPUT,
    DIALOG_TYPE_INPUT_MASKED,
    DIALOG_TYPE_LIST,
    DIALOG_TYPE_RADIOLIST,
    DIALOG_TYPE_CHECKLIST
};

template<DialogType Type, bool AcceptInput, bool MaskedInput>
class InfoDialog : public Dialog
{
protected:
    std::string     mMessage;

public:
                    InfoDialog(
                        int playerid,
                        const std::string &title,
                        const std::string &msg
                    ) :
        Dialog(playerid, title), mMessage(msg) {}

    virtual         ~InfoDialog() {}

    virtual bool    display()
    {
        const char *btn1, *btn2;
        switch(Type)
        {
        case DIALOG_TYPE_MESSAGE:
            btn1 = t(mPlayerId, OK);
            btn2 = "";
            break;
        case DIALOG_TYPE_CONFIRM:
            btn1 = t(mPlayerId, YES);
            btn2 = t(mPlayerId, NO);
            break;
        case DIALOG_TYPE_INPUT:
        case DIALOG_TYPE_INPUT_MASKED:
            btn1 = t(mPlayerId, OK);
            btn2 = t(mPlayerId, BACK);
            break;
        default:
            btn1 = t(mPlayerId, UNDEF);
            btn2 = t(mPlayerId, UNDEF);
        }
        int style;
        if(AcceptInput == true)
        {
            if(MaskedInput == true)
            {
                style = DIALOG_STYLE_PASSWORD;
            }
            else
            {
                style = DIALOG_STYLE_INPUT;
            }
        }
        else
        {
            style = DIALOG_STYLE_MSGBOX;
        }
        return ShowPlayerDialog(mPlayerId, 0, style, mTitle.c_str(),
            mMessage.c_str(), btn1, btn2) != 0;
    }
};

typedef InfoDialog<DIALOG_TYPE_MESSAGE, false, false>   __MessageDialog;
typedef InfoDialog<DIALOG_TYPE_CONFIRM, false, false>   ConfirmDialog;
typedef InfoDialog<DIALOG_TYPE_INPUT, true, false>      InputDialog;
typedef InfoDialog<DIALOG_TYPE_INPUT_MASKED, true, true>MaskedInputDialog;

class MessageDialog : public __MessageDialog
{
public:
                    MessageDialog(
                        int playerid,
                        const std::string &title,
                        const std::string &msg
                    ) :
        __MessageDialog(playerid, title, msg) {}

    virtual         ~MessageDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext)
    {
        return true;
    }
};

class MenuDialog : public Dialog
{
protected:
    typedef std::function<bool()>   Functor;

    struct Item
    {
        std::string                 title;
        Functor                     callback;
    };

    std::vector<Item>               mItemList;

public:
                    MenuDialog(
                        int playerid,
                        const std::string &title
                    );

    virtual         ~MenuDialog() {}

            void    addItem(const std::string &title,
                const Functor& callback);
    virtual bool    display();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

template<typename KeyType>
class RadioListDialog : public Dialog
{
protected:
    struct Item
    {
        KeyType                     key;
        std::string                 title;
    };

    std::vector<Item>               mItemList;
    size_t                          mSelected;

public:
                    RadioListDialog(
                        int playerid,
                        const std::string &title
                    ) : Dialog(playerid, title), mSelected(-1) {}

    virtual         ~RadioListDialog() {}

    /**
     * The "active" parameter will overwrite the old selected flag.
     */
            void    addItem(KeyType key, const std::string &title,
        bool active = false)
    {
        mItemList.push_back({key, title});
        if(active)
        {
            mSelected = mItemList.size() - 1;
        }
    }

            void    setActiveItem(size_t index)
    {
        if(index <= mItemList.size() - 1)
        {
            mSelected = index;
        }
        else
        {
            LOG(ERROR) << "setActiveItem: index > mItemList.size()";
        }
    }

    virtual bool    display()
    {
        std::stringstream serial;
        for(size_t i = 0; i < mItemList.size(); ++i)
        {
            serial << ((i == mSelected) ? "* " : "  ")
                << mItemList[i].title << "\n";
        }
        return ShowPlayerDialog(mPlayerId, 0, DIALOG_STYLE_LIST,
            mTitle.c_str(), serial.str().c_str(), 
            t(mPlayerId, OK), t(mPlayerId, BACK));
    }

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext)
    {
        if(!response)
        {
            return true;
        }
        if(listitem > mItemList.size() - 1)
        {
            LOG(ERROR) << "listitem > mItemList.size()";
            return false;
        }
        bool changeStatus = process(mItemList[listitem].key);
        if(changeStatus)
        {
            mSelected = listitem;
        }
        return false;
    }

    /**
     * Return a bool that is indicating whether the mSelected can be
     * changed as well as whether the action is successfully performed.
     */
    virtual bool    process(KeyType key) = 0;
};

class CheckListDialog : public Dialog
{
protected:
    typedef std::function<bool()>   Functor;

    struct Item
    {
        std::string                 title;
        Functor                     statusChecker;
        Functor                     statusToggler;
    };

    std::vector<Item>               mItemList;

public:
                    CheckListDialog(
                        int playerid,
                        const std::string &title
                    );

    virtual         ~CheckListDialog() {}

            void    addItem(const std::string &title,
                const Functor &checker, const Functor &toggler);
    virtual bool    display();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

}
