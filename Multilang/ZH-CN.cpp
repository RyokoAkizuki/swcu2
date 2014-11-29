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

#include "Language.hpp"

namespace swcu {

char* LANG_ZH_CN[LANG_ENTRY_END + 1] = {
    // OK
    "确定",
    // YES
    "是",
    // NO
    "否",
    // BACK
    "返回",
    // UNDEF
    "<未定义>",
    // TOGGLE
    "切换",

    // PLAYER_NOT_FOUND
    "未找到该玩家.",

    // DLG_REG_TITLE
    "欢迎来到SWCU自由街区",
    // DLG_REG_MSG
    "请输入一个至少6位长的密码完成注册.",
    // DLG_REG_MUST_CREATE
    "你必须注册一个账号才能进入游戏.",
    // DLG_REG_ALREADY_REGISTERED
    "你已经注册过账号了.",
    // DLG_REG_SUCCESS
    "你的账号注册成功.",
    // DLG_REG_ERR
    "你的密码不符合要求, 或者服务器发生了内部错误.\n"
        "请重新尝试, 如果反复出现错误请麻烦联系管理员.",

    // DLG_LOG_TITLE
    "回到自由街区",
    // DLG_LOG_MSG
    "请输入密码来登录.",
    // DLG_LOG_MUST_LOGIN
    "你必须登录才能加入游戏.",
    // DLG_LOG_LOG_TWICE
    "你已经登录过了, 不需要重复登录.",
    // DLG_LOG_SUCCESS
    "登录成功.",
    // DLG_LOG_BAD_LOGIN
    "密码错误, 请重试.",

    // NOT_LOGGED_IN
    "你还没有登录.",

    // DLG_EDIT_PROF_TITLE
    "我的资料",
    // DLG_EDIT_PROF_LOG_NAME
    "我的登录名: ",
    // DLG_EDIT_PROF_PASSWORD
    "更改密码",
    // DLG_EDIT_PROF_NICKNAME
    "我的昵称: ",

    // DLG_EDIT_PROF_TITLE_POSTFIX
    "的资料",
    // DLG_VIEW_PROF_ID
    "ID: ",
    // DLG_VIEW_PROF_LOGNAME
    "登录名: ",
    // DLG_VIEW_PROF_NICKNAME
    "昵称: ",
    // DLG_VIEW_PROF_JOINTIME
    "注册时间: ",
    // DLG_VIEW_PROF_GAMETIME
    "游戏时间: ",
    // DLG_VIEW_PROF_ADMINLEVEL
    "管理员等级: ",
    // DLG_VIEW_PROF_TARGET_NOT_LOGGED_IN
    "对方没有登录.",

    // DLG_CHANGE_PASSWORD_TITLE
    "更改密码",
    // DLG_CHANGE_PASSWORD_MSG
    "请输入一个6位以上的密码.",
    // DLG_CHANGE_PASSWORD_FAIL
    "你的密码不符合要求, 或者服务器发生了内部错误.\n"
        "请重新尝试, 如果反复出现错误请麻烦联系管理员.",
    // DLG_CHANGE_PASSWORD_SUCCESS
    "密码更改成功.",

    // DLG_CHANGE_LOGNAME_TITLE
    "更改登录名",
    // DLG_CHANGE_LOGNAME_MSG
    "请输入一个新的名字.",
    // DLG_CHANGE_LOGNAME_FAIL
    "名称不符合要求, 或者服务器发生了内部错误.\n"
        "请重试. 如果反复出现错误请麻烦联系管理员.",
    // DLG_CHANGE_LOGNAME_SUCCESS
    "登录名更改成功.",

    // DLG_CHANGE_NICKNAME_TITLE
    "更改昵称",
    // DLG_CHANGE_NICKNAME_MSG
    "请输入一个新的名字.",
    // DLG_CHANGE_NICKNAME_FAIL
    "名称不符合要求, 或者服务器发生了内部错误.\n"
        "请重试. 如果反复出现错误请麻烦联系管理员.",
    // DLG_CHANGE_NICKNAME_SUCCESS
    "昵称更改成功.",

    // LANG_ENTRY_END
    ""
};

}
