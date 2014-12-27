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

#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>
#include <sampgdk/a_vehicles.h>
#include <algorithm>

#include "../Interface/CommandManager.hpp"
#include "../Interface/DialogManager.hpp"
#include "../Multilang/Language.hpp"
#include "../Player/PlayerManager.hpp"
#include "../Weapon/WeaponShopDialog.hpp"

#include "PlayerCommands.hpp"

namespace swcu {

bool pcmdFixCar(int playerid, std::stringstream& /* cmdline */)
{
    RepairVehicle(GetPlayerVehicleID(playerid));
    return true;
}

bool pcmdSpawnJetPack(int playerid, std::stringstream& /* cmdline */)
{
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USEJETPACK);
    return true;
}

bool pcmdChangeSkin(int playerid, std::stringstream& cmdline)
{
    int skinid;
    cmdline >> skinid;
    if(skinid < 0 || skinid > 299)
    {
        SendClientMessage(playerid, 0xFFFFFFFF,
            t(playerid, SKIN_ID_INVALID));
        return false;
    }
    return SetPlayerSkin(playerid, skinid);
}

bool pcmdVehicle(int playerid, std::stringstream& cmdline)
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return false;

    std::string subfunc;
    cmdline >> subfunc;

    if(subfunc.size() > 0 &&
        std::all_of(subfunc.begin(), subfunc.end(), ::isdigit))
    {
        p->createPrivateVehicle(atoi(subfunc.c_str()));
        return true;
    }
    if(subfunc == "wode" || subfunc == "mine")
    {
        p->teleportPrivateVehicleToPlayer();
        return true;
    }
    if(subfunc == "color")
    {
        int c1, c2;
        cmdline >> c1 >> c2;
        ChangeVehicleColor(p->getPrivateVehicleId(), c1, c2);
        return true;
    }
    if(subfunc == "rengdiao" || subfunc == "drop")
    {
        p->dropPrivateVehicle();
        return true;
    }
    return true;
}

bool pcmdSuicide(int playerid, std::stringstream& /* cmdline */)
{
    SetPlayerHealth(playerid, 0.0);
    return true;
}

bool pcmdChangeWorld(int playerid, std::stringstream& cmdline)
{
    int vworld;
    cmdline >> vworld;
    SetPlayerVirtualWorld(playerid, vworld);
    return true;
}

bool pcmdWeaponShop(int playerid, std::stringstream& /* cmdline */)
{
    DialogManager::get().push<WeaponShopDialog>(playerid);
    return true;
}

bool pcmdTeleportToPos(int playerid, std::stringstream& cmdline)
{
    float x, y, z;
    cmdline >> x >> y >> z;
    SetPlayerPos(playerid, x, y, z);
    return true;
}

void registerPlayerCommands()
{
    CommandManager::get().registerCommand("repair",     &pcmdFixCar);
    CommandManager::get().registerCommand("fix",        &pcmdFixCar);
    CommandManager::get().registerCommand("xiuche",     &pcmdFixCar);

    CommandManager::get().registerCommand("jetpack",    &pcmdSpawnJetPack);
    CommandManager::get().registerCommand("fxq",        &pcmdSpawnJetPack);

    CommandManager::get().registerCommand("hf",         &pcmdChangeSkin);
    CommandManager::get().registerCommand("skin",       &pcmdChangeSkin);

    CommandManager::get().registerCommand("c",          &pcmdVehicle);

    CommandManager::get().registerCommand("k",          &pcmdSuicide);
    CommandManager::get().registerCommand("kill",       &pcmdSuicide);

    CommandManager::get().registerCommand("w",          &pcmdChangeWorld);

    CommandManager::get().registerCommand("wuqi",       &pcmdWeaponShop);
    CommandManager::get().registerCommand("weapon",     &pcmdWeaponShop);

    CommandManager::get().registerCommand("t",          &pcmdTeleportToPos);
}

}
