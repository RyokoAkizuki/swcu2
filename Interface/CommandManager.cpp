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

#include "CommandManager.hpp"

namespace swcu {

void CommandManager::registerCommand(const std::string& cmdname,
    CommandHandler handler)
{
    auto iter = mCommands.find(cmdname);
    if(iter != mCommands.end())
    {
        LOG(WARNING) << "Commadn handler overwritten: " << cmdname;
        iter->second = handler;
    }
    else
    {
        mCommands.insert(std::make_pair(cmdname, handler));
    }
}

bool CommandManager::handleCallback(int playerid, const char *cmdtext)
{
    if(strlen(cmdtext) < 2) return false;
    try
    {
        std::stringstream   cmdsrc(cmdtext + 1);
        std::string         cmdname;
        cmdsrc >> cmdname;
        auto iter = mCommands.find(cmdname);
        if(iter == mCommands.end())
        {
            return false;
        }
        else
        {
            return iter->second(playerid, cmdsrc);
        }
    }
    catch(std::exception &e)
    {
        LOG(ERROR) << e.what();
    }
    catch(...)
    {
        LOG(ERROR) << "Unknown error occurred.";
    }
    return false;
}

}
