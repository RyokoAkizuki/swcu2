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
    MenuDialog(playerid, "��ͼ����")
{
}

void MapManagerDialog::build()
{
    int playerid = mPlayerId;
    addItem("�鿴������ĵ�ͼ", [playerid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "������ĵ�ͼ",
            [playerid](const std::shared_ptr<Map>& map) {
                DialogManager::get().push<MapEditDialog>(playerid, map);
            }
        );
    });
    addItem("�����������е�ͼ",
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

MapEditDialog::MapEditDialog(int playerid, const std::shared_ptr<Map>& map) :
    MenuDialog(playerid, "��ͼ����"), mMap(map)
{
}

void MapEditDialog::build()
{
    // addItem("���°�Χ���", std::bind(mMap, &Map::updateBounding));
}

PropertyDialog::PropertyDialog(int playerid) :
    MenuDialog(playerid, "����")
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
}

PropertyEditDialog::PropertyEditDialog(int playerid,
    const std::shared_ptr<Map>& map) :
    MenuDialog(playerid, "���� " + map->getName()), mMap(map)
{
}

void PropertyEditDialog::build()
{
    if(mMap->getType() != PROPERTY) return;
    addItem("����: " + mMap->getOwner().str(), [](){});
    
}

}
