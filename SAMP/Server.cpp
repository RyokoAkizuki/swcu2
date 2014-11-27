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

#include <stdio.h>
#include <string.h>
#include <thread>
#include <sstream>

#include <sampgdk/a_players.h>
#include <sampgdk/a_samp.h>
#include <sampgdk/a_vehicles.h>
#include <sampgdk/core.h>
#include <sampgdk/sdk.h>

#include "../Common/Common.hpp"
#include "../Streamer/Streamer.hpp"
#include "../Player/PlayerManager.hpp"
#include "../Player/PlayerDialogs.hpp"
#include "../Interface/DialogManager.hpp"

/** ~~ Streamer Only ~~ **/

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerEnterCheckpoint(int playerid)
{
    Streamer_OnPlayerEnterCheckpoint(playerid);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerLeaveCheckpoint(int playerid)
{
    Streamer_OnPlayerLeaveCheckpoint(playerid);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerEnterRaceCheckpoint(int playerid)
{
    Streamer_OnPlayerEnterRaceCheckpoint(playerid);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerLeaveRaceCheckpoint(int playerid)
{
    Streamer_OnPlayerLeaveRaceCheckpoint(playerid);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerPickUpPickup(
    int playerid, int pickupid)
{
    Streamer_OnPlayerPickUpPickup(playerid, pickupid);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerEditObject(
    int playerid, bool playerobject, int objectid, int response,
    float fX, float fY, float fZ, float fRotX, float fRotY, float fRotZ)
{
    Streamer_OnPlayerEditObject(playerid, playerobject, objectid, response,
        fX, fY, fZ, fRotX, fRotY, fRotZ);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerSelectObject(int playerid,
    int type, int objectid, int modelid, float x, float y, float z)
{
    Streamer_OnPlayerSelectObject(playerid, type, objectid, modelid,
        x, y, z);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerWeaponShot(int playerid,
    int weaponid, int hittype, int hitid, float x, float y, float z)
{
    Streamer_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid,
        x, y, z);
    return true;
}

/** ^^ Streamer Only ^^ **/

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid)
{
    Streamer_OnPlayerConnect(playerid);
    swcu::Player* p = swcu::PlayerManager::get().addPlayer(playerid);
    if(p != nullptr)
    {
        LOG(INFO) << "Player connected. ID = " << playerid;
        if(p->isRegistered())
        {
            swcu::DialogManager::get().push
                <swcu::PlayerLoginDialog>(playerid);
        }
        else
        {
            swcu::DialogManager::get().push
                <swcu::PlayerRegisterDialog>(playerid);
        }
    }
    else
    {
        LOG(ERROR) << "Addition of player to PlayerManager instance failed.";
    }
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDisconnect(int playerid, int reason)
{
    Streamer_OnPlayerDisconnect(playerid, reason);
    if(swcu::PlayerManager::get().removePlayer(playerid))
    {
        LOG(INFO) << "Player disconnected. ID = " << playerid <<
        ", Reason = " << reason;
    }
    else
    {
        LOG(ERROR) << "Removal of player from PlayerManager instance failed.";
    }
    swcu::DialogManager::get().clearPlayerStack(playerid);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnGameModeInit()
{
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerSpawn(int playerid)
{
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid,
    const char *cmdtext)
{
    return false;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnDialogResponse(int playerid, int dialogid,
    int response, int listitem, const char * inputtext)
{
    swcu::DialogManager::get().handleCallback(playerid, dialogid, response,
        listitem, inputtext);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerClickPlayer(int playerid,
    int clickedplayerid, int source)
{
    if(playerid == clickedplayerid)
    {
        swcu::DialogManager::get().push
            <swcu::PlayerEditProfileDialog>(playerid);
    }
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerText(int playerid,
    const char * text)
{
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerUpdate(int playerid)
{
    return true;
}
