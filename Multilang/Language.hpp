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

#pragma once

namespace swcu {

enum Languages
{
    ZH_CN,
    EN_US,

    MAX_LANGUAGES
};

enum LanguageEntries
{
    OK,
    YES,
    NO,
    BACK,
    UNDEF,
    TOGGLE,

    PLAYER_NOT_FOUND,

    DLG_REG_TITLE,
    DLG_REG_MSG,
    DLG_REG_MUST_CREATE,
    DLG_REG_ALREADY_REGISTERED,
    DLG_REG_SUCCESS,
    DLG_REG_ERR,

    DLG_LOG_TITLE,
    DLG_LOG_MSG,
    DLG_LOG_MUST_LOGIN,
    DLG_LOG_LOG_TWICE,
    DLG_LOG_SUCCESS,
    DLG_LOG_BAD_LOGIN,

    NOT_LOGGED_IN,

    DLG_EDIT_PROF_TITLE,
    DLG_EDIT_PROF_LOG_NAME,
    DLG_EDIT_PROF_PASSWORD,
    DLG_EDIT_PROF_NICKNAME,

    DLG_VIEW_PROF_TITLE_POSTFIX,
    DLG_VIEW_PROF_ID,
    DLG_VIEW_PROF_LOGNAME,
    DLG_VIEW_PROF_NICKNAME,
    DLG_VIEW_PROF_JOINTIME,
    DLG_VIEW_PROF_GAMETIME,
    DLG_VIEW_PROF_ADMINLEVEL,
    DLG_VIEW_PROF_TARGET_NOT_LOGGED_IN,

    DLG_CHANGE_PASSWORD_TITLE,
    DLG_CHANGE_PASSWORD_MSG,
    DLG_CHANGE_PASSWORD_FAIL,
    DLG_CHANGE_PASSWORD_SUCCESS,

    DLG_CHANGE_LOGNAME_TITLE,
    DLG_CHANGE_LOGNAME_MSG,
    DLG_CHANGE_LOGNAME_FAIL,
    DLG_CHANGE_LOGNAME_SUCCESS,

    DLG_CHANGE_NICKNAME_TITLE,
    DLG_CHANGE_NICKNAME_MSG,
    DLG_CHANGE_NICKNAME_FAIL,
    DLG_CHANGE_NICKNAME_SUCCESS,

    DLG_EMPTY_CONTENT,

    DLG_MAP_MGR_TITLE,
    DLG_MAP_MGR_LOADED,
    DLG_MAP_MGR_RELOAD_ALL,

    DLG_MAP_LOADED_TITLE,

    DLG_MAP_EDIT_TITLE,
    DLG_MAP_EDIT_UNLOAD,
    DLG_MAP_EDIT_CALC_BOUND,
    DLG_MAP_EDIT_LOAD,

    TELEPORT_NOT_FOUND,
    TELEPORT_INVALID,
    TELEPORT_CREATE_SUCCESS,
    TELEPORT_CREATE_FAILED,

    DLG_PLAYER_CTL_TITLE,
    DLG_PLAYER_CTL_SENDMSG,
    DLG_PLAYER_CTL_VIEWPROFILE,
    DLG_PLAYER_CTL_SETWANTED,
    DLG_PLAYER_CTL_GOTO,
    DLG_PLAYER_CTL_GETTHERE,
    DLG_PLAYER_CTL_UNMUTE,
    DLG_PLAYER_CTL_MUTE,
    DLG_PLAYER_CTL_EJECT,
    DLG_PLAYER_CTL_UNFREEZE,
    DLG_PLAYER_CTL_FREEZE,
    DLG_PLAYER_CTL_KILL,
    DLG_PLAYER_CTL_FORCERESPAWN,
    DLG_PLAYER_CTL_KICK,
    DLG_PLAYER_CTL_EXPLODE,
    DLG_PLAYER_CTL_BAN,
    DLG_PLAYER_CTL_SET_ADMIN,
    DLG_PLAYER_CTL_SET_POLICE,

    DLG_PM_TITLE,
    DLG_PM_MSG,
    DLG_PM_SENT,
    DLG_PM_RECEIVE,

    DLG_SET_ADMIN_LEVEL_TITLE,
    ADMIN_LEVEL_0,
    ADMIN_LEVEL_1,
    ADMIN_LEVEL_2,
    ADMIN_LEVEL_3,

    DLG_SET_POLICE_RANK_TITLE,

    DLG_CTLPANEL_TITLE,
    DLG_CTLPANEL_PROFILE,
    DLG_CTLPANEL_MAP_MGR,
    
    INFO_PUT_IN_PRISON,
    WARN_ESCAPE_FROM_PRISON,

    DLG_SET_WANDTED_TITLE,
    WANDTED_LEVEL_0,
    WANDTED_LEVEL_1,
    WANDTED_LEVEL_2,
    WANDTED_LEVEL_3,
    WANDTED_LEVEL_4,
    WANDTED_LEVEL_5,
    WANDTED_LEVEL_6,
    DLG_SET_WANTED_BEYOND_AUTH,

    LANG_ENTRY_END
};

class Player;

const char* t(int playerid, LanguageEntries entry);
const char* t(Player* player, LanguageEntries entry);

}
