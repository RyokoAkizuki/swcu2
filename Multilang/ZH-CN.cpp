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

const char* LANG_ZH_CN[LANG_ENTRY_END + 1] = {
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

    // DLG_VIEW_PROF_TITLE_POST_FIX
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

    // DLG_EMPTY_CONTENT
    "无内容.",

    // DLG_MAP_MGR_TITLE
    "地图管理",
    // DLG_MAP_MGR_LOADED
    "查看已载入的地图",
    // DLG_MAP_MGR_RELOAD_ALL
    "重新载入所有地图",

    // DLG_MAP_LOADED_TITLE
    "已载入的地图",

    // DLG_MAP_EDIT_TITLE
    "地图管理",
    // DLG_MAP_EDIT_UNLOAD
    "卸载",
    // DLG_MAP_EDIT_CALC_BOUND
    "计算包围球",
    // DLG_MAP_EDIT_LOAD
    "加载",

    // TELEPORT_NOT_FOUND
    "传送点不存在.",
    // TELEPORT_INVALID
    "传送点无效.",
    // TELEPORT_CREATE_SUCCESS
    "传送点创建成功.",
    // TELEPORT_CREATE_FAILED
    "传送点创建失败.",

    // DLG_PLAYER_CTL_TITLE
    "玩家",
    // DLG_PLAYER_CTL_SENDMSG
    "发送密语",
    // DLG_PLAYER_CTL_VIEWPROFILE
    "查看资料",
    // DLG_PLAYER_CTL_ARREST
    "逮捕",
    // DLG_PLAYER_CTL_SETWANTED
    "通缉",
    // DLG_PLAYER_CTL_GOTO
    "传送过去",
    // DLG_PLAYER_CTL_GETTHERE
    "拉过来",
    // DLG_PLAYER_CTL_UNMUTE
    "允许发言",
    // DLG_PLAYER_CTL_MUTE
    "禁止发言",
    // DLG_PLAYER_CTL_EJECT
    "拉下车",
    // DLG_PLAYER_CTL_UNFREEZE
    "解冻",
    // DLG_PLAYER_CTL_FREEZE
    "冻结",
    // DLG_PLAYER_CTL_KILL
    "重置生命",
    // DLG_PLAYER_CTL_FORCERESPAWN
    "强制重新选人",
    // DLG_PLAYER_CTL_KICK
    "踢出服务器",
    // DLG_PLAYER_CTL_EXPLODE
    "爆炸",
    // DLG_PLAYER_CTL_BAN
    "封禁",
    // DLG_PLAYER_CTL_SET_ADMIN
    "更改管理员等级",
    // DLG_PLAYER_CTL_SET_POLICE
    "更改警衔",

    // DLG_PM_TITLE
    "发送密语",
    // DLG_PM_MSG
    "请输入你要发送的消息",
    // DLG_PM_SENT
    "消息发送给 ",
    // DLG_PM_RECEIVE
    "消息来自 ",
    
    // DLG_SET_ADMIN_LEVEL_TITLE
    "更改管理员等级",
    // ADMIN_LEVEL_0
    "无管理权限",
    // ADMIN_LEVEL_1
    "1级 调解员",
    // ADMIN_LEVEL_2
    "2级 管理员",
    // ADMIN_LEVEL_3
    "3级 领袖",

    // DLG_SET_POLICE_RANK_TITLE
    "设置警衔",

    // DLG_CTLPANEL_TITLE
    "控制面板",
    // DLG_CTLPANEL_PROFILE
    "编辑资料",
    // DLG_CTLPANEL_LANG
    "更改语言 Select Language",
    // DLG_CTLPANEL_MAP_MGR
    "地图管理",
    // DLG_CTLPANEL_SURRENDER
    "自首",

    // INFO_PUT_IN_PRISON
    "你被送进了监狱.",
    // WARN_ESCAPE_FROM_PRISON
    "不要越狱.",

    // DLG_SET_WANDTED_TITLE
    "设置通缉等级",
    // WANDTED_LEVEL_0
    "不通缉",
    // WANDTED_LEVEL_1
    "一星通缉",
    // WANDTED_LEVEL_2
    "二星通缉",
    // WANDTED_LEVEL_3
    "三星通缉",
    // WANDTED_LEVEL_4
    "四星通缉",
    // WANDTED_LEVEL_5
    "五星通缉",
    // WANDTED_LEVEL_6
    "六星通缉",
    // DLG_SET_WANTED_BEYOND_AUTH
    "你没有权限降低他的通缉等级.",

    // DLG_SELECT_LANG_TITLE
    "选择语言 Select Language",

    // DLG_SURRENDER_TITLE
    "通缉",
    // DLG_SURRENDER_MSG
    "你被通缉了, 你可以自首(只关押两分钟)或逃跑. 你要自首吗?",

    // DLG_ARREST_TITLE
    "逮捕",
    // DLG_ARREST_5MIN
    "关押5分钟",
    // DLG_ARREST_10MIN
    "关押10分钟",
    // DLG_ARREST_15MIN
    "关押15分钟",

    // LANG_ENTRY_END
    ""
};

}
