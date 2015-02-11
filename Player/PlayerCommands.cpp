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
        "���Ľ�ͨ�������޸�");
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
        SendClientMessage(playerid, 0xFFFFFFFF, "Ƥ��ID��Ч");
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
                CSTR("�㴴����IDΪ" << vid << "�ĳ���"));
        else
            SendClientMessage(playerid, 0xFFFFFFFF,
                "��������ʧ��");
        return true;
    }
    if(subfunc == "wode" || subfunc == "mine")
    {
        if(p->getPrivateVehicleId() != INVALID_VEHICLE_ID)
        {
            p->teleportPrivateVehicleToPlayer();
            SendClientMessage(playerid, 0xFFFFFFFF,
               "�㽫�Լ��ĳ��������˹���");
        }
        else
        {
            SendClientMessage(playerid, 0xFFFFFFFF,
                "��û��");
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
                CSTR("�㽫������ɫ����Ϊ " << c1 << " " << c2));
        }
        else
        {
            SendClientMessage(playerid, 0xFFFFFFFF,
                "��û��");
        }
        return true;
    }
    if(subfunc == "rengdiao" || subfunc == "drop")
    {
        if(p->getPrivateVehicleId() != INVALID_VEHICLE_ID)
        {
            p->dropPrivateVehicle();
            SendClientMessage(playerid, 0xFFFFFFFF,
                "���ӵ����Լ��ĳ�");
        }
        else
        {
            SendClientMessage(playerid, 0xFFFFFFFF,
                "��û��");
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
        "��ӭ����SWCU���ɵش�");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "��Tab��˫�����������Է�����, ˫���Լ������ֻ�Y���򿪿������.");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "��������п��Ը����Լ��ĵ�¼��(����Ϸ������),");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "������ǳ�(������ʾ������, ֧�����ĺ���ɫ����), Ҳ���Թ�������, �����ͼ������.");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "ˢ��ָ�� /c ����ID ������ɫ /c ��ɫ1 ��ɫ2");
    SendClientMessage(playerid, 0xFFFFFFFF,
        "�����ɵش���ʽ���� Ⱥ��111738228");
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
