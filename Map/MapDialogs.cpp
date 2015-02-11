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

#include <sampgdk/a_players.h>
#include <sampgdk/a_objects.h>

#include "../Interface/DialogManager.hpp"
#include "../Player/PlayerManager.hpp"
#include "MapManager.hpp"

#include "MapDialogs.hpp"

namespace swcu {

MapManagerDialog::MapManagerDialog(int playerid) :
    MenuDialog(playerid, "地图管理")
{
}

bool MapManagerDialog::build()
{
    int playerid = mPlayerId;
    addItem("查看已载入的地图", [playerid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "已载入的地图",
            [playerid](const std::shared_ptr<Map>& map) {
                DialogManager::get().push<MapEditDialog>(
                    playerid, map
                );
            }
        );
    });
    addItem("重新载入所有地图",
        std::bind(&MapManager::loadAllMaps, std::ref(MapManager::get()))
    );
    return true;
}

MapViewDialog::MapViewDialog(int playerid, const std::string& title,
        Callback callback, Filter filter) :
    ItemListDialog<std::shared_ptr<Map>>(playerid, title)
    , mCallback(callback), mFilter(filter)
{
}

bool MapViewDialog::build()
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
    return true;
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

bool MapEditDialog::build()
{
    auto map = mMap;
    auto playerid = mPlayerId;
    addItem("传送到中心", [=]() {
        auto p = PlayerManager::get().getPlayer(playerid);
        if(p == nullptr || !p->isLoggedIn())
        {
            return;
        }
        p->teleportTo(map->getBoundCenter());
    });
    addItem("更新包围体积", [=]() { map->updateBounding(); });
    addItem("更改地图类型", [=]() {
        DialogManager::get().push<MapSetTypeDialog>(
            playerid, map
        );
    });
    addItem("添加物体", [=]() {
        DialogManager::get().push<MapAddObjectDialog>(
            playerid, map
        );
    });
    if(map->getType() == PROPERTY)
    {
        addItem("查看房产信息", [=]() {
            DialogManager::get().push<PropertyEditDialog>(
                playerid, map
            );
        });
    }

    return true;
}

MapSetTypeDialog::MapSetTypeDialog(int playerid, std::shared_ptr<Map> m) :
    RadioListDialog<MapType>(playerid, STR("设置 " << m->getName() << " 的类型")),
    mMap(std::move(m))
{
}

bool MapSetTypeDialog::build()
{
    addItem(LANDSCAPE,  "地形", mMap->getType() == LANDSCAPE);
    addItem(DEATHMATCH, "死斗", mMap->getType() == DEATHMATCH);
    addItem(STUNT,      "特技", mMap->getType() == STUNT);
    addItem(PROPERTY,   "房产", mMap->getType() == PROPERTY);
    addItem(BUSINESS,   "生意", mMap->getType() == BUSINESS);
    addItem(PRISON,     "监狱", mMap->getType() == PRISON);
    return true;
}

bool MapSetTypeDialog::process(MapType type)
{
    return mMap->setType(type);
}

MapAddObjectDialog::MapAddObjectDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    ItemListDialog<int>(playerid, STR("给 " << map->getName() <<  " 添加物体")),
    mMap(map)
{
}

bool MapAddObjectDialog::build()
{
    if(!mMap->isValid()) return false;
    addItem(2885, "原创牌");
    return true;
}

bool MapAddObjectDialog::process(int objectid)
{
    if(!mMap->isValid()) return true;
    float x, y, z;
    GetPlayerPos(mPlayerId, &x, &y, &z);
    auto obj = mMap->addObject(objectid,
        x + 5.0, y + 5.0, z, 0.0, 0.0, 0.0, true, -1);
    obj->startEditing(mPlayerId);
    return true;
}

PropertyDialog::PropertyDialog(int playerid) :
    MenuDialog(playerid, "房产")
{
}

bool PropertyDialog::build()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr || !p->isLoggedIn())
    {
        return false;
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
    addItem("编辑物体", [playerid]() {
        SelectObject(playerid);
        SendClientMessage(playerid, 0xFFFFFFFF, "请用鼠标选择要编辑的物体");
    });
    return true;
}

PropertyEditDialog::PropertyEditDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    MenuDialog(playerid, "房产 " + map->getName()), mMap(map)
{
}

bool PropertyEditDialog::build()
{
    if(mMap->getType() != PROPERTY) return false;
    std::shared_ptr<Map> map = mMap;
    int playerid = mPlayerId;
    auto player = PlayerManager::get().getPlayer(playerid);
    if(player == nullptr || !player->isLoggedIn())
    {
        return false;
    }
    if(mMap->getOwner() == player->getId())
    {
        addItem("名称: " + mMap->getName(), [=]() {
           DialogManager::get().push<PropertySetNameDialog>(
                playerid, map);
        });
        addItem(STR("价格: " << mMap->getPrice()), [=](){
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            if(p->getAdminLevel() >= 3)
                DialogManager::get().push<PropertySetPriceDialog>(
                    playerid, map);
        });
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
        addItem(STR("价格: " << mMap->getPrice()), [=](){
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            if(p->getAdminLevel() >= 3)
                DialogManager::get().push<PropertySetPriceDialog>(
                    playerid, map);
        });
        if(mMap->getOwner() != mongo::OID()) return false;
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
    return true;
}

PropertySetNameDialog::PropertySetNameDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    InputDialog(playerid, "更改房产名称"),
    mState(INIT), mMap(map)
{
}

bool PropertySetNameDialog::build()
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
    return true;
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

bool PropertySetPasswordDialog::build()
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
    return true;
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

PropertySetPriceDialog::PropertySetPriceDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    InputDialog(playerid, "更改房产价格"),
    mState(INIT), mMap(map)
{
}

bool PropertySetPriceDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("请输入价格.");
        break;
        case FAIL:
        setMessage("价格不符合要求, 或者服务器发生了内部错误.\n"
            "请重试. 如果反复出现错误请麻烦联系管理员.");
        break;
    }
    return true;
}

bool PropertySetPriceDialog::handleCallback(
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
    if(p->getAdminLevel() < 3)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "你没有权限更改房屋价格.");
        return true;
    }
    if(!mMap->setPrice(atoi(inputtext.c_str())))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "房产价格更改成功.");
        return true;
    }
}

}
