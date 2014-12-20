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

#include "../Interface/CommandManager.hpp"
#include "../Multilang/Language.hpp"

#include "PlayerCommands.hpp"

namespace swcu {

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

bool pcmdCreateVehicle(int playerid, std::stringstream& cmdline)
{
    int vehiclemodel;
    cmdline >> vehiclemodel;
    float x, y, z, a;
    int interior, world;
    GetPlayerPos(playerid, &x, &y, &z);
    GetPlayerFacingAngle(playerid, &a);
    interior = GetPlayerInterior(playerid);
    world = GetPlayerVirtualWorld(playerid);
    int v = CreateVehicle(vehiclemodel, x, y, z, a,
        rand() % 256, rand() % 256, 60);
    PutPlayerInVehicle(playerid, v, 0);
    LinkVehicleToInterior(v, interior);
    SetVehicleVirtualWorld(v, world);
    return true;
}

void registerPlayerCommands()
{
    CommandManager::get().registerCommand("hf",         &pcmdChangeSkin);
    CommandManager::get().registerCommand("skin",       &pcmdChangeSkin);
    CommandManager::get().registerCommand("c",          &pcmdCreateVehicle);
}

}
