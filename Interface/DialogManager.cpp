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

#include "../Common/Common.hpp"

#include "DialogManager.hpp"

namespace swcu {

void DialogManager::clearPlayerStack(int playerid)
{
    mPlayerDialogStacks.erase(playerid);
}

bool DialogManager::handleCallback(int playerid, int /* dialogid */,
    int response, int listitem, const std::string &inputtext)
{
    auto iter = mPlayerDialogStacks.find(playerid);
    if(iter == mPlayerDialogStacks.end())
    {
        LOG(ERROR) << "A dialog callback is called while there is no "
            "such a dialog or the player trigger it is disconnected.";
        return false;
    }
    size_t size = iter->second.size();
    if(size == 0)
    {
        LOG(ERROR) << "A dialog callback is called while the player's "
            "dialog stack is empty";
        return false;
    }
    auto top = iter->second.top().get();
    bool canpop = top->handleCallback(response != 0, listitem, inputtext);
    /**
     * If oldSize != newSize after invoking handleCallback,
     * there must be a new dialog pushed into the stack,
     * therefore we should not pop the new top.
     */
    if(canpop && size == iter->second.size())
    {
        iter->second.pop();
        if(iter->second.size() == 0)
        {
            return true;
        }
    }
    top = iter->second.top().get();
    top->display();
    return true;
}

}
