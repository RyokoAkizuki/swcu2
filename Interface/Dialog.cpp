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

#include "Dialog.hpp"

namespace swcu {

int Dialog::sLastId = 0;

Dialog::Dialog(int playerid) : mPlayerId(playerid)
{
    /**
     * May cause conflicts which are almostly impossible to happen.
     */
    mId = sLastId;
    ++sLastId;
    sLastId %= 32767;
}

int Dialog::getIdentity()
{
    return mId;
}

MessageDialog::MessageDialog(
    int playerid, const std::string &title,
    const std::string &msg, const std::string &btn1,
    const std::string &btn2, bool acceptInput,
    bool maskedInput) :
    Dialog(playerid), mTitle(title), mMessage(msg),
    mBtn1(btn1), mBtn2(btn2), mInput(acceptInput),
    mMasked(maskedInput)
{
}

bool MessageDialog::display()
{
    return ShowPlayerDialog(mPlayerId, getIdentity(),
        mInput ? (mMasked ? DIALOG_STYLE_PASSWORD : DIALOG_STYLE_INPUT)
            :  DIALOG_STYLE_MSGBOX,
        mTitle.c_str(), mMessage.c_str(),
        mBtn1.c_str(), mBtn2.c_str()) != 0;
}

bool MessageDialog::handleCallback(int playerid, int dialogid,
    int response, int listitem, const std::string &inputtext)
{
    return true;
}

ListDialog::ListDialog(
    int playerid, const std::string &title,
    const std::string &btn1, const std::string &btn2) :
    Dialog(playerid), mTitle(title),
    mBtn1(btn1), mBtn2(btn2)
{
}

void ListDialog::addItem(const std::string &title, ItemCallback cb)
{
    mItemList.push_back(std::make_pair(title, cb));
}

bool ListDialog::display()
{
    std::stringstream info;
    for(auto &i : mItemList)
    {
        info << i.first << "\n";
    }
    return ShowPlayerDialog(mPlayerId, getIdentity(),
        DIALOG_STYLE_LIST,
        mTitle.c_str(), info.str().c_str(),
        mBtn1.c_str(), mBtn2.c_str()) != 0;
}

bool ListDialog::handleCallback(int playerid, int dialogid,
    int response, int listitem, const std::string &inputtext)
{
    if(listitem > mItemList.size())
    {
        return false;
    }
    return mItemList[listitem].second(response == 1);
}

}
