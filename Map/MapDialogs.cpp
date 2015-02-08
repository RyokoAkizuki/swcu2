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

#include "../Interface/DialogManager.hpp"
#include "../Player/PlayerManager.hpp"
#include "MapManager.hpp"

#include "MapDialogs.hpp"

namespace swcu {

MapManagerDialog::MapManagerDialog(int playerid) :
    MenuDialog(playerid, "地图管理")
{
}

void MapManagerDialog::build()
{
    int playerid = mPlayerId;
    addItem("查看已载入的地图", [playerid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "已载入的地图",
            [playerid](const std::shared_ptr<Map>& map) {
                DialogManager::get().push<MapEditDialog>(playerid, map);
            }
        );
    });
    addItem("重新载入所有地图",
        std::bind(&MapManager::loadAllMaps, std::ref(MapManager::get()))
    );
}

MapViewDialog::MapViewDialog(int playerid, const std::string& title,
        Callback callback, Filter filter) :
    ItemListDialog<std::shared_ptr<Map>>(playerid, title)
    , mCallback(callback), mFilter(filter)
{
}

void MapViewDialog::build()
{
    MapManager& mgr = MapManager::get();
    for(auto& iter : mgr.mLoadedMaps)
    {
        if(mFilter(iter.second))
        {
            addItem(
                iter.second,
                STR(iter.second->getTypeStr() << " " << iter.first)
            );
        }
    }
}

bool MapViewDialog::process(std::shared_ptr<Map> key)
{
    mCallback(key);
    return true;
}

MapEditDialog::MapEditDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    MenuDialog(playerid, "地图管理"), mMap(map)
{
}

void MapEditDialog::build()
{
    // addItem("更新包围体积", std::bind(mMap, &Map::updateBounding));
}

PropertyDialog::PropertyDialog(int playerid) :
    MenuDialog(playerid, "房产")
{
}

void PropertyDialog::build()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr || !p->isLoggedIn())
    {
        return;
    }
    int playerid = mPlayerId;
    mongo::OID pid = p->getId();
    addItem("查看我的房屋", [playerid, pid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "查看我的房屋",
            [playerid](const std::shared_ptr<Map>& map) {
                DialogManager::get().push<PropertyEditDialog>(
                    playerid, map
                );
            },
            [pid](const std::shared_ptr<Map>& m) {
                return m->getType() == PROPERTY &&
                    m->getOwner() == pid;
            }
        );
    });
    addItem("购买房屋", [playerid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "购买房屋",
            [playerid](const std::shared_ptr<Map>& map) {
                DialogManager::get().push<PropertyEditDialog>(
                    playerid, map
                );
            },
            [](const std::shared_ptr<Map>& m) {
                return m->getType() == PROPERTY &&
                    m->getOwner() == mongo::OID();
            }
        );
    });
}

PropertyEditDialog::PropertyEditDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    MenuDialog(playerid, "房产 " + map->getName()), mMap(map)
{
}

void PropertyEditDialog::build()
{
    if(mMap->getType() != PROPERTY) return;
    std::shared_ptr<Map> map = mMap;
    int playerid = mPlayerId;
    auto player = PlayerManager::get().getPlayer(playerid);
    if(player == nullptr || !player->isLoggedIn())
    {
        return;
    }
    if(mMap->getOwner() == player->getId())
    {
        addItem("名称: " + mMap->getName(), [=]() {
           DialogManager::get().push<PropertySetNameDialog>(
                playerid, map);
        });
        addItem(STR("价格: " << mMap->getPrice()), [](){});
        addItem("将传送点设置在脚下", [=]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            std::string name = mongo::OID::gen().str();
            if(p->createTeleport(name) && map->setEntrance(name))
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "传送点设置成功");
            }
            else
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "传送点设置失败");
            }
        });
        addItem("传送到门口", [=]() {
            map->teleportToEntrance(playerid);
        });
        addItem("更改密码", [=]() {
            DialogManager::get().push<PropertySetPasswordDialog>(
                playerid, map);
        });
        addItem("出售", [=]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            if(map->sell(p))
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "出售成功");
            }
            else
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "出售失败");
            }
        });
    }
    else
    {
        addItem("名称: " + mMap->getName(), [](){});
        addItem(STR("价格: " << mMap->getPrice()), [](){});
        if(mMap->getOwner() != mongo::OID()) return;
        addItem("购买", [=]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            if(map->buy(p))
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "购买成功");
            }
            else
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "购买失败");
            }
        });
    }
}

PropertySetNameDialog::PropertySetNameDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    InputDialog(playerid, "更改房产名称"),
    mState(INIT), mMap(map)
{
}

void PropertySetNameDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("请输入一个新的名称.");
        break;
        case FAIL:
        setMessage("名称不符合要求, 或者服务器发生了内部错误.\n"
            "请重试. 如果反复出现错误请麻烦联系管理员.");
        break;
    }
}

bool PropertySetNameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr || !p->isLoggedIn())
    {
        return true;
    }
    if(p->getId() != mMap->getOwner())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "你不是房主.");
        return true;
    }
    if(inputtext.size() == 0 || !mMap->setName(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "房产名称更改成功.");
        return true;
    }
}

PropertySetPasswordDialog::PropertySetPasswordDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    InputDialog(playerid, "更改房产密码"),
    mState(INIT), mMap(map)
{
}

void PropertySetPasswordDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("请输入密码.");
        break;
        case FAIL:
        setMessage("密码不符合要求, 或者服务器发生了内部错误.\n"
            "请重试. 如果反复出现错误请麻烦联系管理员.");
        break;
    }
}

bool PropertySetPasswordDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr || !p->isLoggedIn())
    {
        return true;
    }
    if(p->getId() != mMap->getOwner())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "你不是房主.");
        return true;
    }
    if(!mMap->setPassword(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "房产密码更改成功.");
        return true;
    }
}

}
