/*
 * Copyright 2014-2015 Yukino Hayakawa<tennencoll@gmail.com>
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
#include "../Player/PlayerCommands.hpp"
#include "../Interface/DialogManager.hpp"
#include "../Interface/CommandManager.hpp"
#include "../Map/MapManager.hpp"
#include "../Map/MapDialogs.hpp"
#include "../Area/AreaManager.hpp"
#include "../Web/WebServiceManager.hpp"

/** ~~ Event Forwarding for Streamer ~~ **/

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

/** ^^ Event Forwarding for Streamer ^^ **/

PLUGIN_EXPORT bool PLUGIN_CALL OnGameModeInit()
{
    srand(time(NULL));
    ShowNameTags(0);
    for(int i = 0; i < 299; ++i)
    {
        AddPlayerClass(i, 1958.3783, 1343.1572, 15.3746, 270.1425,
            0, 0, 0, 0, -1, -1);
    }
    swcu::registerPlayerCommands();
    swcu::MapManager::get().loadAllMaps();
    swcu::WebServiceManager::get().bindMethod("^/hello$", "GET",
    [](std::ostream& response, swcu::HTTPRequertPtr request) {
        swcu::writeResponse(response, 200, swcu::CONTENT_TYPE_TEXT_PLAIN,
        "Hello.");
    });
    swcu::MapManager::get().addWebServices();
    swcu::WebServiceManager::get().startServer();
    LOG(INFO) << "Game mode initialized.";
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid)
{
    Streamer_OnPlayerConnect(playerid);
    swcu::Player* p = swcu::PlayerManager::get().addPlayer(playerid);
    if(p != nullptr)
    {
        LOG(INFO) << "Player connected. ID = " << playerid;
        if(p->isValid())
        {
            if(p->hasFlags(swcu::STATUS_BANNED))
            {
                Kick(playerid);
                return false;
            }
            swcu::DialogManager::get().push
                <swcu::PlayerLoginDialog>(playerid);
        }
        else
        {
            swcu::DialogManager::get().push
                <swcu::PlayerRegisterDialog>(playerid);
        }
        SendClientMessageToAll(0xFFFFFFFF,
            CSTR("玩家 " << p->getColoredNickname()
            << "(" << playerid << ") 进入了服务器."));
        SendDeathMessage(INVALID_PLAYER_ID, playerid, 200 /* ICON_CONNECT */);
        OnPlayerCommandText(playerid, "/help");
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
    swcu::Player* p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p != nullptr)
    {
        SendClientMessageToAll(0xFFFFFFFF,
            CSTR("玩家 " << p->getColoredNickname()
            << "(" << playerid << ") 离开了服务器."));
        SendDeathMessage(INVALID_PLAYER_ID, playerid, 201 /* ICON_DISCONNECT */);
    }
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

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerUpdate(int playerid)
{
    auto p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return false;
    return p->onUpdate();
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerSpawn(int playerid)
{
    auto p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return false;
    return p->onSpawn();
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDeath(int playerid, int killerid,
    int reason)
{
    auto p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p == nullptr) return false;
    return p->onDeath();
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerText(int playerid, const char * text)
{
    auto p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p == nullptr)
    {
        return false;
    }
    if(p->hasFlags(swcu::STATUS_MUTED))
    {
        return false;
    }
    std::stringstream chat;
    chat << p->getColoredNickname() << "(" << playerid << "): " << text;
    SendClientMessageToAll(0xFFFFFFFF, chat.str().c_str());
    return false;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid,
    const char *cmdtext)
{
    auto p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p == nullptr)
    {
        return false;
    }
    if(cmdtext[1] == '/' && sizeof(cmdtext) > 2)
    {
        p->teleportTo(cmdtext + 2);
        return true;
    }
    return swcu::CommandManager::get().handleCallback(playerid, cmdtext);
}

PLUGIN_EXPORT bool PLUGIN_CALL OnDialogResponse(int playerid, int dialogid,
    int response, int listitem, const char * inputtext)
{
    swcu::DialogManager::get().handleCallback(playerid, dialogid, response,
        listitem, inputtext);
    Streamer_Update(playerid);
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerClickPlayer(int playerid,
    int clickedplayerid, int /* source */)
{
    auto p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p == nullptr || !p->isLoggedIn())
    {
        return false;
    }
    if(playerid == clickedplayerid)
    {
        swcu::DialogManager::get().push
            <swcu::PlayerControlPanelDialog>(playerid);
    }
    else
    {
        swcu::DialogManager::get().push
            <swcu::PlayerControlDialog>(playerid, clickedplayerid);
    }
    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerKeyStateChange(int playerid,
    int newkeys, int oldkeys)
{
    if((newkeys & KEY_YES) && !(oldkeys & KEY_YES))
    {
        auto p = swcu::PlayerManager::get().getPlayer(playerid);
        if(p == nullptr || !p->isLoggedIn())
        {
            return false;
        }
        swcu::DialogManager::get().push
            <swcu::PlayerControlPanelDialog>(playerid);
    }
    if((newkeys & (KEY_FIRE | KEY_ACTION))
        && IsPlayerInAnyVehicle(playerid)
        && GetPlayerVehicleSeat(playerid) == 0)
    {
        AddVehicleComponent(GetPlayerVehicleID(playerid), 1010);
    }
    return true;
}

/** Streamer Callbacks **/

void OnDynamicObjectMoved(int objectid)
{

}

void OnPlayerEditDynamicObject(int playerid, int objectid,
    int response, float x, float y, float z, float rx, float ry, float rz)
{
    if(response != EDIT_RESPONSE_FINAL) return;
    auto obj = swcu::getObject(objectid);
    if(obj != nullptr)
    {
        if(obj->changePose(x, y, z, rx, ry, rz))
        {
            SendClientMessage(playerid, 0xFFFFFFFF, "物体位置已保存");
        }
        else
        {
            SendClientMessage(playerid, 0xFFFFFFFF, "物体位置保存失败");
        }
    }
}

void OnPlayerSelectDynamicObject(int playerid, int objectid,
    int modelid, float x, float y, float z)
{
    auto p = swcu::PlayerManager::get().getPlayer(playerid);
    if(p == nullptr || !p->isLoggedIn())
    {
        return;
    }
    auto obj = swcu::getObject(objectid);
    if(obj != nullptr && p->getAdminLevel() > 2)
    {
        std::string& op = p->pVar["SelectObject"];
        if(op == "ToEditPosition")
            obj->startEditing(playerid);
        else if(op == "ToEditText")
            swcu::DialogManager::get().push
                <swcu::ObjectSetTextDialog>(playerid, objectid, obj);
        else if(op == "ToRemove")
            obj->remove();
        op = "";
    }
}

void OnPlayerShootDynamicObject(int playerid, int weaponid,
    int objectid, float x, float y, float z)
{

}

void OnPlayerPickUpDynamicPickup(int playerid, int pickupid)
{

}

void OnPlayerEnterDynamicCP(int playerid, int checkpointid)
{

}

void OnPlayerLeaveDynamicCP(int playerid, int checkpointid)
{

}

void OnPlayerEnterDynamicRaceCP(int playerid, int checkpointid)
{

}

void OnPlayerLeaveDynamicRaceCP(int playerid, int checkpointid)
{

}

void OnPlayerEnterDynamicArea(int playerid, int areaid)
{
    swcu::AreaManager::get().handleEnterAreaCallback(playerid, areaid);
}

void OnPlayerLeaveDynamicArea(int playerid, int areaid)
{
    swcu::AreaManager::get().handleLeaveAreaCallback(playerid, areaid);
}
