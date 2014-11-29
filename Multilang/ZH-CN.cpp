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

    // DLG_EDIT_PROF_TITLE_POSTFIX
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

    // LANG_ENTRY_END
    ""
};

}
