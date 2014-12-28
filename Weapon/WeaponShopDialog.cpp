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
    { 90, "100 Health Points",  "补满血" },
    { 91, "100 Armour Points",  "补满甲" },

    // *** 突击步枪 ***
    { 30, "AK-47",              "AK-47 突击步枪" },
    { 31, "M4",                 "M4 卡宾枪" },

    // *** 霰弹枪 ***
    { 25, "Shotgun",            "霰弹枪" },
    { 26, "Sawnoff Shotgun",    "短管霰弹枪" },
    { 27, "Combat Shotgun",     "战斗霰弹枪" },

    // *** 冲锋枪 ***
    { 28, "Micro SMG/Uzi",      "乌兹冲锋枪 (Micro-UZI)" },
    { 29, "MP5",                "MP5 冲锋枪" },
    { 32, "Tec-9",              "Tec-9 微型冲锋枪" },

    // *** 手枪 ***
    { 22, "9mm",                "9mm手枪" },
    { 23, "Silenced 9mm",       "9mm消音手枪" },
    { 24, "Desert Eagle",       "沙漠之鹰手枪" },

    // *** 投掷武器 ***
    { 16, "Grenade",            "手榴弹" },
    { 17, "Tear Gas",           "催泪弹" },
    { 18, "Molotov Cocktail",   "燃烧弹" },
    { 39, "Satchel Charge",     "炸药包" },

    // *** 狙击步枪 ***
    { 33, "Country Rifle",      "乡村步枪" },
    { 34, "Sniper Rifle",       "狙击步枪" },

    // *** 重型武器 ***
    { 35, "RPG",                "火箭发射器 (RPG)" },
    { 36, "HS Rocket",          "热追踪火箭发射器" },
    { 37, "Flamethrower",       "火焰喷射器" },
    // { 38, "Minigun",            "迷你炮机枪 (Minigun)" },


    // *** 装备 ***
    { 41, "Spraycan",           "喷漆罐" },
    { 42, "Fire Extinguisher",  "灭火器" },
    { 43, "Camera",             "照相机" },
    // { 44, "Night Vis Goggles",  "夜视镜" },
    // { 45, "Thermal Goggles",    "热成像夜视镜" },
    { 46, "Parachute",          "降落伞" },

    // *** 徒手 ***
    // { 0, "Fist",               "拳头" },
    {  1, "Brass Knuckles",     "铜指虎" },
    {  2, "Golf Club",          "高尔夫球杆" },
    {  3, "Nightstick",         "警棍" },
    {  4, "Knife",              "匕首" },
    {  5, "Baseball Bat",       "棒球棍" },
    {  6, "Shovel",             "铁铲" },
    {  7, "Pool Cue",           "台球杆" },
    {  8, "Katana",             "武士刀" },
    {  9, "Chainsaw",           "电锯" },

    // *** 其他物品 ***
    { 10, "Purple Dildo",       "紫色的假阳具" },
    { 11, "Dildo",              "白色的震动棒(小)" },
    { 12, "Vibrator",           "白色的震动棒(大)" },
    { 13, "Silver Vibrator",    "银色的震动棒" },
    { 14, "Flowers",            "花束" },
    { 15, "Cane",               "拐杖" }
};

WeaponShopDialog::WeaponShopDialog(int playerid) :
    ItemListDialog<int>(playerid, "武器商店")
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
