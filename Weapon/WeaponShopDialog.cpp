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

#include <vector>
#include <sampgdk/a_players.h>

#include "WeaponShopDialog.hpp"

namespace swcu {

struct WeaponItem
{
    int         id;
    const char* enName;
    const char* cnName;
};

std::vector<WeaponItem> gWeaponShopItems =
{
    // *** HP & AP ***
    { 90, "100 Health Points",  "����Ѫ" },
    { 91, "100 Armour Points",  "������" },

    // *** ͻ����ǹ ***
    { 30, "AK-47",              "AK-47 ͻ����ǹ" },
    { 31, "M4",                 "M4 ����ǹ" },

    // *** ����ǹ ***
    { 25, "Shotgun",            "����ǹ" },
    { 26, "Sawnoff Shotgun",    "�̹�����ǹ" },
    { 27, "Combat Shotgun",     "ս������ǹ" },

    // *** ���ǹ ***
    { 28, "Micro SMG/Uzi",      "���ȳ��ǹ (Micro-UZI)" },
    { 29, "MP5",                "MP5 ���ǹ" },
    { 32, "Tec-9",              "Tec-9 ΢�ͳ��ǹ" },

    // *** ��ǹ ***
    { 22, "9mm",                "9mm��ǹ" },
    { 23, "Silenced 9mm",       "9mm������ǹ" },
    { 24, "Desert Eagle",       "ɳĮ֮ӥ��ǹ" },

    // *** Ͷ������ ***
    { 16, "Grenade",            "����" },
    { 17, "Tear Gas",           "���ᵯ" },
    { 18, "Molotov Cocktail",   "ȼ�յ�" },
    { 39, "Satchel Charge",     "ըҩ��" },

    // *** �ѻ���ǹ ***
    { 33, "Country Rifle",      "��岽ǹ" },
    { 34, "Sniper Rifle",       "�ѻ���ǹ" },

    // *** �������� ***
    { 35, "RPG",                "��������� (RPG)" },
    { 36, "HS Rocket",          "��׷�ٻ��������" },
    { 37, "Flamethrower",       "����������" },
    // { 38, "Minigun",            "�����ڻ�ǹ (Minigun)" },


    // *** װ�� ***
    { 41, "Spraycan",           "�����" },
    { 42, "Fire Extinguisher",  "�����" },
    { 43, "Camera",             "�����" },
    // { 44, "Night Vis Goggles",  "ҹ�Ӿ�" },
    // { 45, "Thermal Goggles",    "�ȳ���ҹ�Ӿ�" },
    { 46, "Parachute",          "����ɡ" },

    // *** ͽ�� ***
    // { 0, "Fist",               "ȭͷ" },
    {  1, "Brass Knuckles",     "ָͭ��" },
    {  2, "Golf Club",          "�߶������" },
    {  3, "Nightstick",         "����" },
    {  4, "Knife",              "ذ��" },
    {  5, "Baseball Bat",       "�����" },
    {  6, "Shovel",             "����" },
    {  7, "Pool Cue",           "̨���" },
    {  8, "Katana",             "��ʿ��" },
    {  9, "Chainsaw",           "���" },

    // *** ������Ʒ ***
    { 10, "Purple Dildo",       "��ɫ�ļ�����" },
    { 11, "Dildo",              "��ɫ���𶯰�(С)" },
    { 12, "Vibrator",           "��ɫ���𶯰�(��)" },
    { 13, "Silver Vibrator",    "��ɫ���𶯰�" },
    { 14, "Flowers",            "����" },
    { 15, "Cane",               "����" }
};

WeaponShopDialog::WeaponShopDialog(int playerid) :
    ItemListDialog<int>(playerid, "�����̵�")
{
}

void WeaponShopDialog::build()
{
    for(auto &i : gWeaponShopItems)
    {
        addItem(i.id, i.cnName);
    }
}

bool WeaponShopDialog::process(int key)
{
    if(key == 90)
    {
        SetPlayerHealth(mPlayerId, 100.0);
        return false;
    }
    if(key == 91)
    {
        SetPlayerArmour(mPlayerId, 100.0);
        return false;
    }
    GivePlayerWeapon(mPlayerId, key, 1000);
    return false;
}

}
