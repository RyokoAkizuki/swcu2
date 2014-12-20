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

const char* LANG_EN_US[LANG_ENTRY_END + 1] = {
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
    "Gaming Time: ",
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

    // DLG_EMPTY_CONTENT
    "No content.",

    // DLG_MAP_MGR_TITLE
    "Map Manager",
    // DLG_MAP_MGR_LOADED
    "Loaded Maps",
    // DLG_MAP_MGR_RELOAD_ALL
    "Reload All",

    // DLG_MAP_LOADED_TITLE
    "Loaded Maps",

    // DLG_MAP_EDIT_TITLE
    "Edit Map",
    // DLG_MAP_EDIT_UNLOAD
    "Unload",
    // DLG_MAP_EDIT_CALC_BOUND
    "Calculate Bounding Shpere",
    // DLG_MAP_EDIT_LOAD
    "Load",

    // TELEPORT_NOT_FOUND
    "Teleport not found.",
    // TELEPORT_INVALID
    "Invalid teleport.",
    // TELEPORT_CREATE_SUCCESS
    "Teleport has been created.",
    // TELEPORT_CREATE_FAILED
    "Failed to create teleport.",

    // DLG_PLAYER_CTL_TITLE
    "Player",
    // DLG_PLAYER_CTL_SENDMSG
    "Send Message",
    // DLG_PLAYER_CTL_VIEWPROFILE
    "View Profile",
    // DLG_PLAYER_CTL_ARREST
    "Arrest",
    // DLG_PLAYER_CTL_SETWANTED
    "Wanted to Arrest",
    // DLG_PLAYER_CTL_FREE
    "Free from Prison",
    // DLG_PLAYER_CTL_GOTO
    "Go To",
    // DLG_PLAYER_CTL_GETTHERE
    "Get Here",
    // DLG_PLAYER_CTL_UNMUTE
    "Unmute",
    // DLG_PLAYER_CTL_MUTE
    "Mute",
    // DLG_PLAYER_CTL_EJECT
    "Eject",
    // DLG_PLAYER_CTL_UNFREEZE
    "Unfreeze",
    // DLG_PLAYER_CTL_FREEZE
    "Freeze",
    // DLG_PLAYER_CTL_KILL
    "Reset Health",
    // DLG_PLAYER_CTL_FORCERESPAWN
    "Force To Respawn Screen",
    // DLG_PLAYER_CTL_KICK
    "Kick",
    // DLG_PLAYER_CTL_EXPLODE
    "Explode",
    // DLG_PLAYER_CTL_BAN
    "Ban",
    // DLG_PLAYER_CTL_SET_ADMIN
    "Set Admin Level",
    // DLG_PLAYER_CTL_SET_POLICE
    "Set Police Rank",

    // DLG_PM_TITLE
    "Send Private Message",
    // DLG_PM_MSG
    "Please enter your message to send.",
    // DLG_PM_SENT
    "Message sent to ",
    // DLG_PM_RECEIVE
    "Message from ",
    
    // DLG_SET_ADMIN_LEVEL_TITLE
    "Set Admin Level",
    // ADMIN_LEVEL_0
    "No Authority",
    // ADMIN_LEVEL_1
    "Level 1 Moderator",
    // ADMIN_LEVEL_2
    "Level 2 Administrator",
    // ADMIN_LEVEL_3
    "Level 3 Game Master",

    // DLG_SET_POLICE_RANK_TITLE
    "Set Police Rank",

    // DLG_CTLPANEL_TITLE
    "Control Panel",
    // DLG_CTLPANEL_PROFILE
    "Edit Profile",
    // DLG_CTLPANEL_LANG
    "Change Language",
    // DLG_CTLPANEL_MAP_MGR
    "Map Manager",
    // DLG_CTLPANEL_SURRENDER
    "Surrender to Police",
    // DLG_CTLPANEL_CREATE_TP
    "Create Teleport Here",
    // DLG_CTLPANEL_WEAPONSHOP
    "Weapon Shop",
    
    // INFO_PUT_IN_PRISON
    "You've been put in prison.",
    // WARN_ESCAPE_FROM_PRISON
    "Don't try to escape from prison.",

    // DLG_SET_WANDTED_TITLE
    "Set Wanted Level",
    // WANDTED_LEVEL_0
    "Not Wanted",
    // WANDTED_LEVEL_1
    "Wanted Level 1",
    // WANDTED_LEVEL_2
    "Wanted Level 2",
    // WANDTED_LEVEL_3
    "Wanted Level 3",
    // WANDTED_LEVEL_4
    "Wanted Level 4",
    // WANDTED_LEVEL_5
    "Wanted Level 5",
    // WANDTED_LEVEL_6
    "Wanted Level 6",
    // DLG_SET_WANTED_BEYOND_AUTH
    "It's beyond your authority to lower his wanted level.",

    // DLG_SELECT_LANG_TITLE
    "Select Language",

    // DLG_SURRENDER_TITLE
    "Surrender",
    // DLG_SURRENDER_MSG
    "You're wanted by police. You can give yourself up\n"
        "and enjoy a prison term of 2 minutes or just run away.\n"
        "Do you want to surrender?",

    // DLG_ARREST_TITLE
    "Arrest",
    // DLG_ARREST_5MIN
    "Jail for 5 minutes",
    // DLG_ARREST_10MIN
    "Jail for 10 minutes",
    // DLG_ARREST_15MIN
    "Jail for 15 minutes",

    // DLG_CREATE_TP_TITLE
    "Create Teleport",
    // DLG_CREATE_TP_MESSAGE
    "Select a name for your teleport",
    
    // DLG_WEAPONSHOP_TITLE
    "Weapon Shop",

    // SKIN_ID_INVALID
    "Invalid skin id.",

    // LANG_ENTRY_END
    ""
};

}
