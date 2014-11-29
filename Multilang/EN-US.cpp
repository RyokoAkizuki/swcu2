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

char* LANG_EN_US[LANG_ENTRY_END + 1] = {
    // OK
    "OK",
    // YES
    "Yes",
    // NO
    "No",
    // BACK
    "Back",
    // UNDEF
    "undef",
    // TOGGLE
    "Toggle",

    // PLAYER_NOT_FOUND
    "Player not found.",

    // DLG_REG_TITLE
    "Welcome to SWCU",
    // DLG_REG_MSG
    "Please enter a password to create your profile.\n"
        "The password must be longer than 6 characters.",
    // DLG_REG_MUST_CREATE
    "You must have a profile to join the game.",
    // DLG_REG_ALREADY_REGISTERED
    "You have a profile and don't have to create a new one.",
    // DLG_REG_SUCCESS
    "Your profile has been created.",
    // DLG_REG_ERR
    "Your password is illegal or a database error is occurred.\n"
        "Please retry.",

    // DLG_LOG_TITLE
    "Back to SWCU",
    // DLG_LOG_MSG
    "Please enter your password.",
    // DLG_LOG_MUST_LOGIN
    "You must login to join the game.",
    // DLG_LOG_LOG_TWICE
    "You've been logged in. You don't have to login twice.",
    // DLG_LOG_SUCCESS
    "You've logged in.",
    // DLG_LOG_BAD_LOGIN
    "Your password is incorrect. Please retry.",

    // NOT_LOGGED_IN
    "You haven't logged in.",

    // DLG_EDIT_PROF_TITLE
    "My Profile",
    // DLG_EDIT_PROF_LOG_NAME
    "My Login Name: ",
    // DLG_EDIT_PROF_PASSWORD
    "Change Password",
    // DLG_EDIT_PROF_NICKNAME
    "My Nickname: ",

    // DLG_VIEW_PROF_TITLE_POST_FIX
    "'s Profile",
    // DLG_VIEW_PROF_ID
    "ID: ",
    // DLG_VIEW_PROF_LOGNAME
    "Login Name: ",
    // DLG_VIEW_PROF_NICKNAME
    "Nickname: ",
    // DLG_VIEW_PROF_JOINTIME
    "Join Time: ",
    // DLG_VIEW_PROF_GAMETIME
    "Gaming Time: "
    // DLG_VIEW_PROF_ADMINLEVEL
    "Admin Level: ",
    // DLG_VIEW_PROF_TARGET_NOT_LOGGED_IN
    "Player is not logged in.",

    // DLG_CHANGE_PASSWORD_TITLE
    "Change Password",
    // DLG_CHANGE_PASSWORD_MSG
    "Please enter a new password below containing more than 6 characters.",
    // DLG_CHANGE_PASSWORD_FAIL
    "Your password is illegal "
        "or a database error is occurred.\nPlease retry.",
    // DLG_CHANGE_PASSWORD_SUCCESS
    "Your password is changed.",

    // DLG_CHANGE_LOGNAME_TITLE
    "Change Login Name",
    // DLG_CHANGE_LOGNAME_MSG
    "Please enter the new name you desired.",
    // DLG_CHANGE_LOGNAME_FAIL
    "The name you used contains illegal characters\n"
        "or a database error is occurred.\nPlease retry.",
    // DLG_CHANGE_LOGNAME_SUCCESS
    "Your login name is changed.",

    // DLG_CHANGE_NICKNAME_TITLE
    "Change Nickname",
    // DLG_CHANGE_NICKNAME_MSG
    "Please enter the new name you desired.",
    // DLG_CHANGE_NICKNAME_FAIL
    "The name you used contains illegal characters\n"
        "or a database error is occurred.\nPlease retry.",
    // DLG_CHANGE_NICKNAME_SUCCESS
    "Your nickname is changed.",

    // LANG_ENTRY_END
    ""
};

}
