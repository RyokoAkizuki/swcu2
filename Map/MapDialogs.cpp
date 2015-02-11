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
    MenuDialog(playerid, "��ͼ����")
{
}

bool MapManagerDialog::build()
{
    int playerid = mPlayerId;
    addItem("�鿴������ĵ�ͼ", [playerid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "������ĵ�ͼ",
            [playerid](const std::shared_ptr<Map>& map) {
                DialogManager::get().push<MapEditDialog>(
                    playerid, map
                );
            }
        );
    });
    addItem("�����������е�ͼ",
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
    MenuDialog(playerid, "��ͼ����"), mMap(map)
{
}

bool MapEditDialog::build()
{
    auto map = mMap;
    auto playerid = mPlayerId;
    addItem("���͵�����", [=]() {
        auto p = PlayerManager::get().getPlayer(playerid);
        if(p == nullptr || !p->isLoggedIn())
        {
            return;
        }
        p->teleportTo(map->getBoundCenter());
    });
    addItem("���°�Χ���", [=]() { map->updateBounding(); });
    addItem("���ĵ�ͼ����", [=]() {
        DialogManager::get().push<MapSetTypeDialog>(
            playerid, map
        );
    });
    addItem("�������", [=]() {
        DialogManager::get().push<MapAddObjectDialog>(
            playerid, map
        );
    });
    if(map->getType() == PROPERTY)
    {
        addItem("�鿴������Ϣ", [=]() {
            DialogManager::get().push<PropertyEditDialog>(
                playerid, map
            );
        });
    }

    return true;
}

MapSetTypeDialog::MapSetTypeDialog(int playerid, std::shared_ptr<Map> m) :
    RadioListDialog<MapType>(playerid, STR("���� " << m->getName() << " ������")),
    mMap(std::move(m))
{
}

bool MapSetTypeDialog::build()
{
    addItem(LANDSCAPE,  "����", mMap->getType() == LANDSCAPE);
    addItem(DEATHMATCH, "����", mMap->getType() == DEATHMATCH);
    addItem(STUNT,      "�ؼ�", mMap->getType() == STUNT);
    addItem(PROPERTY,   "����", mMap->getType() == PROPERTY);
    addItem(BUSINESS,   "����", mMap->getType() == BUSINESS);
    addItem(PRISON,     "����", mMap->getType() == PRISON);
    return true;
}

bool MapSetTypeDialog::process(MapType type)
{
    return mMap->setType(type);
}

MapAddObjectDialog::MapAddObjectDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    ItemListDialog<int>(playerid, STR("�� " << map->getName() <<  " �������")),
    mMap(map)
{
}

bool MapAddObjectDialog::build()
{
    if(!mMap->isValid()) return false;
    addItem(2885, "ԭ����");
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
    MenuDialog(playerid, "����")
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
    addItem("�鿴�ҵķ���", [playerid, pid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "�鿴�ҵķ���",
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
    addItem("������", [playerid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "������",
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
    addItem("�༭����", [playerid]() {
        SelectObject(playerid);
        SendClientMessage(playerid, 0xFFFFFFFF, "�������ѡ��Ҫ�༭������");
    });
    return true;
}

PropertyEditDialog::PropertyEditDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    MenuDialog(playerid, "���� " + map->getName()), mMap(map)
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
        addItem("����: " + mMap->getName(), [=]() {
           DialogManager::get().push<PropertySetNameDialog>(
                playerid, map);
        });
        addItem(STR("�۸�: " << mMap->getPrice()), [=](){
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            if(p->getAdminLevel() >= 3)
                DialogManager::get().push<PropertySetPriceDialog>(
                    playerid, map);
        });
        addItem("�����͵������ڽ���", [=]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            std::string name = mongo::OID::gen().str();
            if(p->createTeleport(name) && map->setEntrance(name))
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "���͵����óɹ�");
            }
            else
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "���͵�����ʧ��");
            }
        });
        addItem("���͵��ſ�", [=]() {
            map->teleportToEntrance(playerid);
        });
        addItem("��������", [=]() {
            DialogManager::get().push<PropertySetPasswordDialog>(
                playerid, map);
        });
        addItem("����", [=]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            if(map->sell(p))
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "���۳ɹ�");
            }
            else
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "����ʧ��");
            }
        });
    }
    else
    {
        addItem("����: " + mMap->getName(), [](){});
        addItem(STR("�۸�: " << mMap->getPrice()), [=](){
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
        addItem("����", [=]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            if(map->buy(p))
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "����ɹ�");
            }
            else
            {
                SendClientMessage(playerid, 0xFFFFFFFF, "����ʧ��");
            }
        });
    }
    return true;
}

PropertySetNameDialog::PropertySetNameDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    InputDialog(playerid, "���ķ�������"),
    mState(INIT), mMap(map)
{
}

bool PropertySetNameDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("������һ���µ�����.");
        break;
        case FAIL:
        setMessage("���Ʋ�����Ҫ��, ���߷������������ڲ�����.\n"
            "������. ����������ִ������鷳��ϵ����Ա.");
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
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�㲻�Ƿ���.");
        return true;
    }
    if(inputtext.size() == 0 || !mMap->setName(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�������Ƹ��ĳɹ�.");
        return true;
    }
}

PropertySetPasswordDialog::PropertySetPasswordDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    InputDialog(playerid, "���ķ�������"),
    mState(INIT), mMap(map)
{
}

bool PropertySetPasswordDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("����������.");
        break;
        case FAIL:
        setMessage("���벻����Ҫ��, ���߷������������ڲ�����.\n"
            "������. ����������ִ������鷳��ϵ����Ա.");
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
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�㲻�Ƿ���.");
        return true;
    }
    if(!mMap->setPassword(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "����������ĳɹ�.");
        return true;
    }
}

PropertySetPriceDialog::PropertySetPriceDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    InputDialog(playerid, "���ķ����۸�"),
    mState(INIT), mMap(map)
{
}

bool PropertySetPriceDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("������۸�.");
        break;
        case FAIL:
        setMessage("�۸񲻷���Ҫ��, ���߷������������ڲ�����.\n"
            "������. ����������ִ������鷳��ϵ����Ա.");
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
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "��û��Ȩ�޸��ķ��ݼ۸�.");
        return true;
    }
    if(!mMap->setPrice(atoi(inputtext.c_str())))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�����۸���ĳɹ�.");
        return true;
    }
}

}
