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

#include <memory>
#include <stack>
#include <unordered_map>

#include "../Utility/Singleton.hpp"

#include "Dialog.hpp"

namespace swcu {

class DialogManager : public Singleton<DialogManager>
{
protected:
    typedef std::unique_ptr<Dialog>                 DialogPtr;
    typedef std::stack<DialogPtr>                   DialogStack;
    typedef std::unordered_map<int, DialogStack>    DialogStackMap;

    DialogStackMap                                  mPlayerDialogStacks;

public:
    virtual         ~DialogManager() {}

            void    clearPlayerStack(int playerid);

    template<typename DialogType, typename...Args>
            void    push(int playerid, Args...args)
    {
        DialogType*             playerDialogPtr
            = new DialogType(playerid, std::move(args)...);
        std::unique_ptr<Dialog> playerDialogPtrWrap(playerDialogPtr);

        mPlayerDialogStacks[playerid].push(std::move(playerDialogPtrWrap));
        playerDialogPtr->display();
    }

    template<typename DialogType>
            void    push(int playerid)
    {
        DialogType*             playerDialogPtr = new DialogType(playerid);
        std::unique_ptr<Dialog> playerDialogPtrWrap(playerDialogPtr);

        mPlayerDialogStacks[playerid].push(std::move(playerDialogPtrWrap));
        playerDialogPtr->display();
    }

            bool    handleCallback(int playerid, int dialogid,
        int response, int listitem, const std::string &inputtext);
};

}
