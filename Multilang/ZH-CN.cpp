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
    "ȷ��",
    // YES
    "��",
    // NO
    "��",
    // BACK
    "����",
    // UNDEF
    "<δ����>",
    // TOGGLE
    "�л�",

    // PLAYER_NOT_FOUND
    "δ�ҵ������.",

    // DLG_REG_TITLE
    "��ӭ����SWCU���ɽ���",
    // DLG_REG_MSG
    "������һ������6λ�����������ע��.",
    // DLG_REG_MUST_CREATE
    "�����ע��һ���˺Ų��ܽ�����Ϸ.",
    // DLG_REG_ALREADY_REGISTERED
    "���Ѿ�ע����˺���.",
    // DLG_REG_SUCCESS
    "����˺�ע��ɹ�.",
    // DLG_REG_ERR
    "������벻����Ҫ��, ���߷������������ڲ�����.\n"
        "�����³���, ����������ִ������鷳��ϵ����Ա.",

    // DLG_LOG_TITLE
    "�ص����ɽ���",
    // DLG_LOG_MSG
    "��������������¼.",
    // DLG_LOG_MUST_LOGIN
    "������¼���ܼ�����Ϸ.",
    // DLG_LOG_LOG_TWICE
    "���Ѿ���¼����, ����Ҫ�ظ���¼.",
    // DLG_LOG_SUCCESS
    "��¼�ɹ�.",
    // DLG_LOG_BAD_LOGIN
    "�������, ������.",

    // NOT_LOGGED_IN
    "�㻹û�е�¼.",

    // DLG_EDIT_PROF_TITLE
    "�ҵ�����",
    // DLG_EDIT_PROF_LOG_NAME
    "�ҵĵ�¼��: ",
    // DLG_EDIT_PROF_PASSWORD
    "��������",
    // DLG_EDIT_PROF_NICKNAME
    "�ҵ��ǳ�: ",

    // DLG_VIEW_PROF_TITLE_POST_FIX
    "������",
    // DLG_VIEW_PROF_ID
    "ID: ",
    // DLG_VIEW_PROF_LOGNAME
    "��¼��: ",
    // DLG_VIEW_PROF_NICKNAME
    "�ǳ�: ",
    // DLG_VIEW_PROF_JOINTIME
    "ע��ʱ��: ",
    // DLG_VIEW_PROF_GAMETIME
    "��Ϸʱ��: ",
    // DLG_VIEW_PROF_ADMINLEVEL
    "����Ա�ȼ�: ",
    // DLG_VIEW_PROF_TARGET_NOT_LOGGED_IN
    "�Է�û�е�¼.",

    // DLG_CHANGE_PASSWORD_TITLE
    "��������",
    // DLG_CHANGE_PASSWORD_MSG
    "������һ��6λ���ϵ�����.",
    // DLG_CHANGE_PASSWORD_FAIL
    "������벻����Ҫ��, ���߷������������ڲ�����.\n"
        "�����³���, ����������ִ������鷳��ϵ����Ա.",
    // DLG_CHANGE_PASSWORD_SUCCESS
    "������ĳɹ�.",

    // DLG_CHANGE_LOGNAME_TITLE
    "���ĵ�¼��",
    // DLG_CHANGE_LOGNAME_MSG
    "������һ���µ�����.",
    // DLG_CHANGE_LOGNAME_FAIL
    "���Ʋ�����Ҫ��, ���߷������������ڲ�����.\n"
        "������. ����������ִ������鷳��ϵ����Ա.",
    // DLG_CHANGE_LOGNAME_SUCCESS
    "��¼�����ĳɹ�.",

    // DLG_CHANGE_NICKNAME_TITLE
    "�����ǳ�",
    // DLG_CHANGE_NICKNAME_MSG
    "������һ���µ�����.",
    // DLG_CHANGE_NICKNAME_FAIL
    "���Ʋ�����Ҫ��, ���߷������������ڲ�����.\n"
        "������. ����������ִ������鷳��ϵ����Ա.",
    // DLG_CHANGE_NICKNAME_SUCCESS
    "�ǳƸ��ĳɹ�.",

    // DLG_EMPTY_CONTENT
    "������.",

    // DLG_MAP_MGR_TITLE
    "��ͼ����",
    // DLG_MAP_MGR_LOADED
    "�鿴������ĵ�ͼ",
    // DLG_MAP_MGR_RELOAD_ALL
    "�����������е�ͼ",

    // DLG_MAP_LOADED_TITLE
    "������ĵ�ͼ",

    // DLG_MAP_EDIT_TITLE
    "��ͼ����",
    // DLG_MAP_EDIT_UNLOAD
    "ж��",
    // DLG_MAP_EDIT_CALC_BOUND
    "�����Χ��",
    // DLG_MAP_EDIT_LOAD
    "����",

    // TELEPORT_NOT_FOUND
    "���͵㲻����.",
    // TELEPORT_INVALID
    "���͵���Ч.",
    // TELEPORT_CREATE_SUCCESS
    "���͵㴴���ɹ�.",
    // TELEPORT_CREATE_FAILED
    "���͵㴴��ʧ��.",

    // DLG_PLAYER_CTL_TITLE
    "���",
    // DLG_PLAYER_CTL_SENDMSG
    "��������",
    // DLG_PLAYER_CTL_VIEWPROFILE
    "�鿴����",
    // DLG_PLAYER_CTL_ARREST
    "����",
    // DLG_PLAYER_CTL_SETWANTED
    "ͨ��",
    // DLG_PLAYER_CTL_GOTO
    "���͹�ȥ",
    // DLG_PLAYER_CTL_GETTHERE
    "������",
    // DLG_PLAYER_CTL_UNMUTE
    "������",
    // DLG_PLAYER_CTL_MUTE
    "��ֹ����",
    // DLG_PLAYER_CTL_EJECT
    "���³�",
    // DLG_PLAYER_CTL_UNFREEZE
    "�ⶳ",
    // DLG_PLAYER_CTL_FREEZE
    "����",
    // DLG_PLAYER_CTL_KILL
    "��������",
    // DLG_PLAYER_CTL_FORCERESPAWN
    "ǿ������ѡ��",
    // DLG_PLAYER_CTL_KICK
    "�߳�������",
    // DLG_PLAYER_CTL_EXPLODE
    "��ը",
    // DLG_PLAYER_CTL_BAN
    "���",
    // DLG_PLAYER_CTL_SET_ADMIN
    "���Ĺ���Ա�ȼ�",
    // DLG_PLAYER_CTL_SET_POLICE
    "���ľ���",

    // DLG_PM_TITLE
    "��������",
    // DLG_PM_MSG
    "��������Ҫ���͵���Ϣ",
    // DLG_PM_SENT
    "��Ϣ���͸� ",
    // DLG_PM_RECEIVE
    "��Ϣ���� ",
    
    // DLG_SET_ADMIN_LEVEL_TITLE
    "���Ĺ���Ա�ȼ�",
    // ADMIN_LEVEL_0
    "�޹���Ȩ��",
    // ADMIN_LEVEL_1
    "1�� ����Ա",
    // ADMIN_LEVEL_2
    "2�� ����Ա",
    // ADMIN_LEVEL_3
    "3�� ����",

    // DLG_SET_POLICE_RANK_TITLE
    "���þ���",

    // DLG_CTLPANEL_TITLE
    "�������",
    // DLG_CTLPANEL_PROFILE
    "�༭����",
    // DLG_CTLPANEL_LANG
    "�������� Select Language",
    // DLG_CTLPANEL_MAP_MGR
    "��ͼ����",
    // DLG_CTLPANEL_SURRENDER
    "����",

    // INFO_PUT_IN_PRISON
    "�㱻�ͽ��˼���.",
    // WARN_ESCAPE_FROM_PRISON
    "��ҪԽ��.",

    // DLG_SET_WANDTED_TITLE
    "����ͨ���ȼ�",
    // WANDTED_LEVEL_0
    "��ͨ��",
    // WANDTED_LEVEL_1
    "һ��ͨ��",
    // WANDTED_LEVEL_2
    "����ͨ��",
    // WANDTED_LEVEL_3
    "����ͨ��",
    // WANDTED_LEVEL_4
    "����ͨ��",
    // WANDTED_LEVEL_5
    "����ͨ��",
    // WANDTED_LEVEL_6
    "����ͨ��",
    // DLG_SET_WANTED_BEYOND_AUTH
    "��û��Ȩ�޽�������ͨ���ȼ�.",

    // DLG_SELECT_LANG_TITLE
    "ѡ������ Select Language",

    // DLG_SURRENDER_TITLE
    "ͨ��",
    // DLG_SURRENDER_MSG
    "�㱻ͨ����, ���������(ֻ��Ѻ������)������. ��Ҫ������?",

    // DLG_ARREST_TITLE
    "����",
    // DLG_ARREST_5MIN
    "��Ѻ5����",
    // DLG_ARREST_10MIN
    "��Ѻ10����",
    // DLG_ARREST_15MIN
    "��Ѻ15����",

    // LANG_ENTRY_END
    ""
};

}
