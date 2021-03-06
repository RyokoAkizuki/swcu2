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

#include <sstream>

#include "Dialog.hpp"

namespace swcu {

MenuDialog::MenuDialog(int playerid, const std::string &title) :
    Dialog(playerid, title)
{
}

void MenuDialog::clear()
{
    mItemList.clear();
}

void MenuDialog::addItem(const std::string &title, const Functor& callback)
{
    mItemList.push_back({title, callback});
}

bool MenuDialog::display()
{
    clear();
    if(!build()) return false;

    std::stringstream serial;
    for(auto &i : mItemList)
    {
        serial << "  " << i.title << "\n";
    }
    std::string message = serial.str();
    if(message.empty())
    {
        message = "无内容.";
    }
    return ShowPlayerDialog(mPlayerId, 0, DIALOG_STYLE_LIST, mTitle.c_str(),
        message.c_str(), "确定", "返回");
}

bool MenuDialog::handleCallback(bool response, int listitem,
    const std::string& /* inputtext */)
{
    if(!response)
    {
        return true;
    }
    if(static_cast<size_t>(listitem + 1) > mItemList.size())
    {
        LOG(ERROR) << "listitem > mItemList.size()";
        return false;
    }
    mItemList[listitem].callback();
    return false;
}

CheckListDialog::CheckListDialog(int playerid, const std::string &title) :
    Dialog(playerid, title)
{
}

void CheckListDialog::clear()
{
    mItemList.clear();
}

void CheckListDialog::addItem(const std::string &title,
    const Functor &checker, const Functor &toggler)
{
    mItemList.push_back({title, checker, toggler});
}

bool CheckListDialog::display()
{
    clear();
    if(!build()) return false;
    
    std::stringstream serial;
    for(auto &i : mItemList)
    {
        serial << (i.statusChecker() ? "+ " : "  ") << i.title << "\n";
    }
    std::string message = serial.str();
    if(message.empty())
    {
        message = "无内容.";
    }
    return ShowPlayerDialog(mPlayerId, 0, DIALOG_STYLE_LIST, mTitle.c_str(),
        message.c_str(), "切换", "返回");
}

bool CheckListDialog::handleCallback(bool response, int listitem,
    const std::string& /* inputtext */)
{
    if(!response)
    {
        return true;
    }
    if(static_cast<size_t>(listitem + 1) > mItemList.size())
    {
        LOG(ERROR) << "listitem > mItemList.size()";
        return false;
    }
    mItemList[listitem].statusToggler();
    return false;
}

}
