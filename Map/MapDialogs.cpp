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
    addItem("查看房屋", [playerid]() {
        DialogManager::get().push<MapViewDialog>(
            playerid,
            "查看房屋",
            [playerid](const std::shared_ptr<Map>& map) {

            },
            [](const std::shared_ptr<Map>& m) {
                return m->getType() == PROPERTY;
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

MapEditDialog::MapEditDialog(int playerid, const std::shared_ptr<Map>& map) :
    MenuDialog(playerid, "地图管理"), mMap(map)
{
}

void MapEditDialog::build()
{
    // addItem("更新包围体积", std::bind(mMap, &Map::updateBounding));
}

}
