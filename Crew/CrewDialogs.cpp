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

#include <memory>

#include "../Interface/DialogManager.hpp"
#include "../Player/PlayerManager.hpp"
#include "Crew.hpp"

#include "CrewDialogs.hpp"

namespace swcu {

CreateCrewDialog::CreateCrewDialog(int playerid) :
    InputDialog(playerid, "创建帮派")
{
}

void CreateCrewDialog::build()
{
    setMessage("请给你的帮派命名");
}

bool CreateCrewDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
{
    if(!response) return true;
    if(inputtext.length() == 0)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "帮派名称不能为空");
        return false;
    }
    std::unique_ptr<Crew> crew(new Crew(inputtext));
    if(crew->isValid())
    {
        auto p = PlayerManager::get().getPlayer(mPlayerId);
        if(p != nullptr && p->joinCrew(crew->getId()))
        {
            SendClientMessage(mPlayerId, 0xFFFFFFFF, "帮派创建成功");
            return true;
        }
    }
    SendClientMessage(mPlayerId, 0xFFFFFFFF, "帮派创建失败");
    return false;
}

CrewFindByNameDialog::CrewFindByNameDialog(int playerid, CallbackType cb) :
    InputDialog(playerid, "查找帮派"), mCallback(cb)
{
}

void CrewFindByNameDialog::build()
{
    setMessage("请输入帮派的名称");
}

bool CrewFindByNameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(response)
    {
        DialogManager::get().push<_CrewFindByNameResultDialog>(
            mPlayerId, inputtext, mCallback);
    }
    return true;
}

_CrewFindByNameResultDialog::_CrewFindByNameResultDialog(
    int playerid, const std::string& keyword, CallbackType cb) :
    ItemListDialog<std::string>(playerid, "查找帮派"),
    mKeyWord(keyword), mCallback(cb)
{
}

void _CrewFindByNameResultDialog::build()
{
    MONGO_WRAPPER({
        auto cur = getDBConn()->query(
            Config::colNameCrew,
            QUERY("name" << BSON("$regex" << mKeyWord))
        );
        while(cur->more())
        {
            auto doc = cur->next();
            addItem(doc["_id"].OID().str(), UTF8ToGBK(doc["name"].str()));
        }
    });
}

bool _CrewFindByNameResultDialog::process(std::string key)
{
    return mCallback(mongo::OID(key));
}

}
