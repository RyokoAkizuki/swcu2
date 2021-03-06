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

    virtual bool    build() = 0;
    virtual void    clear() = 0;

            int     getPlayerId() const
            { return mPlayerId; }

            std::string getTitle() const
            { return mTitle; }

            void    setTitle(const std::string& title)
            { mTitle = title; }
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
                        const std::string &title
                    ) : Dialog(playerid, title) {}

    virtual         ~InfoDialog() {}

    virtual void    setMessage(const std::string& msg)
    {
        mMessage = msg;
    }

    virtual void    clear()
    {
        mMessage.clear();
    }

    virtual bool    display()
    {
        clear();
        if(!build()) return false;

        if(mMessage.empty())
        {
            mMessage = "无内容.";
        }

        const char *btn1, *btn2;
        switch(Type)
        {
        case DIALOG_TYPE_MESSAGE:
            btn1 = "确定";
            btn2 = "";
            break;
        case DIALOG_TYPE_CONFIRM:
            btn1 = "是";
            btn2 = "否";
            break;
        case DIALOG_TYPE_INPUT:
        case DIALOG_TYPE_INPUT_MASKED:
            btn1 = "确定";
            btn2 = "返回";
            break;
        default:
            btn1 = "<未定义>";
            btn2 = "<未定义>";
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
                        const std::string &title
                    ) :
        __MessageDialog(playerid, title) {}

    virtual         ~MessageDialog() {}

    virtual bool    handleCallback(
        bool /* response */, int /* listitem */,
        const std::string& /* inputtext */)
    {
        return true;
    }
};

class MenuDialog : public Dialog
{
protected:
    typedef std::function<void()>   Functor;

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

    virtual void    clear();

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

    virtual void    clear()
    {
        mItemList.clear();
        mSelected = -1;
    }

    /**
     * Only the latest item claiming it's active will be considered active.
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
        if(index + 1<= mItemList.size())
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
        clear();
        if(!build()) return false;

        std::stringstream serial;
        for(size_t i = 0; i < mItemList.size(); ++i)
        {
            serial << ((i == mSelected) ? "* " : "  ")
                << mItemList[i].title << "\n";
        }
        std::string message = serial.str();
        if(message.empty())
        {
            message = "无内容.";
        }
        return ShowPlayerDialog(mPlayerId, 0, DIALOG_STYLE_LIST,
            mTitle.c_str(), message.c_str(), "确定", "返回");
    }

    virtual bool    handleCallback(
        bool response, int listitem, const std::string& /*inputtext*/)
    {
        if(!response)
        {
            return true;
        }
        if(static_cast<size_t>(listitem) + 1 > mItemList.size())
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

template<typename KeyType>
class ItemListDialog : public RadioListDialog<KeyType>
{
public:
                    ItemListDialog(
                        int playerid,
                        const std::string &title
                    ) : RadioListDialog<KeyType>(playerid, title) {}
    virtual         ~ItemListDialog() {}

    virtual bool    handleCallback(
        bool response, int listitem, const std::string& /*inputtext*/)
    {
        if(!response)
        {
            return true;
        }
        if(static_cast<size_t>(listitem) + 1 > this->mItemList.size())
        {
            LOG(ERROR) << "listitem > mItemList.size()";
            return false;
        }
        bool canclose = this->process(this->mItemList[listitem].key);
        return canclose;
    }
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

    virtual void    clear();

            void    addItem(const std::string &title,
                const Functor &checker, const Functor &toggler);
    virtual bool    display();
    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext);
};

}
