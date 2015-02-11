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
#include "../Player/PlayerManager.hpp"
#include "../Weapon/WeaponShopDialog.hpp"

#include "PlayerCommands.hpp"

namespace swcu {

bool pcmdFixCar(int playerid, std::stringstream& /* cmdline */)
{
    SendClientMessage(playerid, 0xFFFFFFFF,
        "您的交通工具已修复");
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
        SendClientMessage(playerid, 0xFFFFFFFF, "皮肤ID无效");
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
        int vid = atoi(subfunc.c_str());
        if(p->createPrivateVehicle(vid))
            SendClientMessage(playerid, 0xFFFFFFFF, 
                CSTR("你创建了ID为" << vid << "的车辆"));
        else
            SendClientMessage(playerid, 0xFFFFFFFF,
                "车辆创建失败");
        return true;
    }
    if(subfunc == "wode" || subfunc == "mine")
    {
        if(p->getPrivateVehicleId() != INVALID_VEHICLE_ID)
        {
            p->teleportPrivateVehicleToPlayer();
            SendClientMessage(playerid, 0xFFFFFFFF,
               "你将自己的车辆传送了过来");
        }
        else
        {
            SendClientMessage(playerid, 0xFFFFFFFF,
                "你没车");
        }
        return true;
    }
    if(subfunc == "color")
    {
        if(p->getPrivateVehicleId() != INVALID_VEHICLE_ID)
        {
            int c1, c2;
            cmdline >> c1 >> c2;
            ChangeVehicleColor(p->getPrivateVehicleId(), c1, c2);
            SendClientMessage(playerid, 0xFFFFFFFF,
                CSTR("你将车辆颜色更改为 " << c1 << " " << c2));
        }
        else
        {
            SendClientMessage(playerid, 0xFFFFFFFF,
                "你没车");
        }
        return true;
    }
    if(subfunc == "rengdiao" || subfunc == "drop")
    {
        if(p->getPrivateVehicleId() != INVALID_VEHICLE_ID)
        {
            p->dropPrivateVehicle();
            SendClientMessage(playerid, 0xFFFFFFFF,
                "你扔掉了自己的车");
        }
        else
        {
            SendClientMessage(playerid, 0xFFFFFFFF,
                "你没车");
        }
        return true;
    }
    return true;
}

bool pcmdSuicide(int playerid, std::stringstream& /* cmdline */)
{
    SetPlayerHealth(playerid, -100.0);
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

bool pcmdHelp(int playerid, std::stringstream& /* cmdline */)
{
    SendClientMessage(playerid, 0xFFFFFFFF,
        "欢迎来到SWCU自由地带");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "按Tab键双击玩家名字与对方互动, 双击自己的名字或按Y键打开控制面板.");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "控制面板中可以更改自己的登录名(进游戏的名字),");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "密码和昵称(对外显示的名字, 支持中文和颜色代码), 也可以购买武器, 创建和加入帮派.");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "刷车指令 /c 车辆ID 更改颜色 /c 颜色1 颜色2");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "新自由地带正式开启 群：111738228");
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
    CommandManager::get().registerCommand("help",       &pcmdHelp);
}

}
