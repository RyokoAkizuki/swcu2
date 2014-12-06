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

    LANG_ENTRY_END
};

const char* t(int playerid, int entry);
const char* t(class Player* player, int entry);

}
