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

#include "../Multilang/Language.hpp"
#include "../Interface/DialogManager.hpp"
#include "MapManager.hpp"

#include "MapDialogs.hpp"

namespace swcu {

MapManagerDialog::MapManagerDialog(int playerid) :
    MenuDialog(playerid, t(playerid, DLG_MAP_MGR_TITLE))
{
}

void MapManagerDialog::build()
{
    int playerid = mPlayerId;
    addItem(t(mPlayerId, DLG_MAP_MGR_LOADED), [playerid]() {
        DialogManager::get().push<MapViewLoadedDialog>(playerid);
    });
    addItem(t(mPlayerId, DLG_MAP_MGR_RELOAD_ALL),
        std::bind(&MapManager::loadAllMaps, std::ref(MapManager::get()))
    );
}

MapViewLoadedDialog::MapViewLoadedDialog(int playerid) :
    ItemListDialog<std::string>(playerid, t(playerid, DLG_MAP_LOADED_TITLE))
{
}

void MapViewLoadedDialog::build()
{
    MapManager& mgr = MapManager::get();
    for(auto& iter : mgr.mLoadedMaps)
    {
        addItem(iter.first, iter.first);
    }
}

bool MapViewLoadedDialog::process(std::string key)
{
    DialogManager::get().push<MapEditDialog>(mPlayerId, key);
    return true;
}

MapEditDialog::MapEditDialog(int playerid, const std::string& name) :
    MenuDialog(playerid, t(playerid, DLG_MAP_EDIT_TITLE)), mName(name)
{
}

void MapEditDialog::build()
{
    if(MapManager::get().isMapLoaded(mName))
    {
        addItem(t(mPlayerId, DLG_MAP_EDIT_UNLOAD),
            std::bind(&MapManager::unloadMap,
                std::ref(MapManager::get()), mName)
        );
        std::string name = mName;
        addItem(t(mPlayerId, DLG_MAP_EDIT_CALC_BOUND), [name]() {
            Map* map = MapManager::get().findMap(name);
            if(map != nullptr)
            {
                map->calculateBoundingSphere();
            }
        });
    }
    else
    {
        addItem(t(mPlayerId, DLG_MAP_EDIT_LOAD),
            std::bind(&MapManager::loadMap,
                std::ref(MapManager::get()), mName)
        );
    }
}

}
