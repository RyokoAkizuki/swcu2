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

    virtual bool    handleCallback(
        bool response, int listitem, const std::string &inputtext)
    {
        return true;
    }

    virtual bool    display()
    {
        const char *btn1, *btn2;
        switch(Type)
        {
        case DIALOG_TYPE_MESSAGE:
            btn1 = "OK";
            btn2 = "";
            break;
        case DIALOG_TYPE_CONFIRM:
            btn1 = "Yes";
            btn2 = "No";
            break;
        case DIALOG_TYPE_INPUT:
        case DIALOG_TYPE_INPUT_MASKED:
            btn1 = "OK";
            btn2 = "Cancel";
            break;
        default:
            btn1 = "undef";
            btn2 = "undef";
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

typedef InfoDialog<DIALOG_TYPE_MESSAGE, false, false>   MessageDialog;
typedef InfoDialog<DIALOG_TYPE_CONFIRM, false, false>   ConfirmDialog;
typedef InfoDialog<DIALOG_TYPE_INPUT, true, false>      InputDialog;
typedef InfoDialog<DIALOG_TYPE_INPUT_MASKED, true, true>MaskedInputDialog;

}
