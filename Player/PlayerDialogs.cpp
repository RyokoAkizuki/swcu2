/*
 * Copyright 2014-2015 Yukino Hayakawa<tennencoll@gmail.com>
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

#include <sampgdk/a_players.h>
#include <sstream>

#include "PlayerManager.hpp"
#include "../Interface/DialogManager.hpp"
#include "../Map/MapDialogs.hpp"
#include "../Weapon/WeaponShopDialog.hpp"
#include "../Crew/CrewDialogs.hpp"
#include "../Crew/Crew.hpp"

#include "PlayerDialogs.hpp"

namespace swcu {

PlayerRegisterDialog::PlayerRegisterDialog(int playerid) :
    InputDialog(playerid, "��ӭ����SWCU���ɽ���"), mState(INIT)
{
}

bool PlayerRegisterDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("������һ������6λ�����������ע��.");
        break;
        case MUST_REG:
        setMessage("�����ע��һ���˺Ų��ܽ�����Ϸ.");
        break;
        case ERROR:
        setMessage("������벻����Ҫ��, ���߷������������ڲ�����.\n"
            "�����³���, ����������ִ������鷳��ϵ����Ա.");
        break;
        case FINISH:
        return false;
    }
    return true;
}

bool PlayerRegisterDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        mState = MUST_REG;
        return false;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "δ�ҵ������.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(p->isValid())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "���Ѿ�ע����˺���.");
        return true;
    }
    if(p->createProfile(inputtext))
    {
        p->setLoggedIn(true);
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "����˺�ע��ɹ�.");
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "����������ǳ�");
        // force player to set nickname when register
        DialogManager::get().push<PlayerChangeNicknameDialog>(mPlayerId, true);
        mState = FINISH;
        return true;
    }
    else
    {
        mState = ERROR;
        return false;
    }
}

PlayerLoginDialog::PlayerLoginDialog(int playerid) :
    InputDialog(playerid, "�ص����ɽ���"), mState(INIT)
{
}

bool PlayerLoginDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("��������������¼.");
        break;
        case MUST_LOGIN:
        setMessage("������¼���ܼ�����Ϸ.");
        break;
        case BAD_LOGIN:
        setMessage("�������, ������.");
        break;
        case FINISH:
        return false;
    }
    return true;
}

bool PlayerLoginDialog::handleCallback(
     bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        mState = MUST_LOGIN;
        return false;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "δ�ҵ������.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            "���Ѿ���¼����, ����Ҫ�ظ���¼.");
        return true;
    }
    if(p->verifyPassword(inputtext))
    {
        p->setLoggedIn(true);
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "��¼�ɹ�.");
        mState = FINISH;
        return true;
    }
    else
    {
        mState = BAD_LOGIN;
        return false;
    }
}

PlayerEditProfileDialog::PlayerEditProfileDialog(int playerid) :
    MenuDialog(playerid, "�ҵ�����")
{
    auto p = PlayerManager::get().getPlayer(playerid);
    if(p == nullptr)
    {
        SendClientMessage(playerid, 0xFFFFFFFF, "δ�ҵ������.");
        LOG(ERROR) << "Player not found.";
        return;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(playerid, 0xFFFFFFFF, "�㻹û�е�¼.");
        return;
    }
}

bool PlayerEditProfileDialog::build()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        return false;
    }
    mItemList.clear();
    int playerid = mPlayerId;
    addItem(
        "�ҵ���Ϸ�ʺ�: " + p->getLogName(),
        [playerid]() {
            DialogManager::get().push<PlayerChangeLogNameDialog>(playerid);
        });
    addItem(
        "��������",
        [playerid]() {
            DialogManager::get().push<PlayerChangePasswordDialog>(playerid);
        });
    addItem(
        "�ҵ��ǳ�: " + p->getColoredNickname(),
        [playerid]() {
            DialogManager::get().push<PlayerChangeNicknameDialog>(playerid);
        });
    {
        std::stringstream al;
        al << "�ҵĹ���Ա�ȼ�: " << p->getAdminLevel();
        addItem(al.str(), []() {});
    }
    addItem("�ҵľ���: " + p->getPoliceRankStr(), [](){});
    return true;
}

PlayerViewProfileDialog::PlayerViewProfileDialog(
    int playerid, int targetplayer) :
    MessageDialog(playerid, ""), mTargetPlayer(targetplayer)
{
}

bool PlayerViewProfileDialog::build()
{
    auto tar = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(tar == nullptr || p == nullptr)
    {
        mTitle = "�������";
        mMessage = "δ�ҵ������.";
    }
    else
    {
        mTitle = tar->getLogName();
        mTitle += "������";
        if(tar->isLoggedIn())
        {
            std::stringstream msg;
            msg << "ID: "
                << tar->getId().str() << "\n"
                << "��Ϸ�ʺ�: "
                << tar->getLogName() << "\n"
                << "�ǳ�: "
                // In case of colored text, reset color at the end.
                << tar->getColoredNickname() << "\n"
                << "ע��ʱ��: "
                << tar->getTimestamp() << "\n"
                << "��Ϸʱ��: "
                << tar->getGameTime() << "\n"
                << "����Ա�ȼ�: "
                << tar->getAdminLevel() << "\n"
                << "����: "
                << tar->getPoliceRankStr();
            mMessage = msg.str();
        }
        else
        {
            mMessage = "�Է�û�е�¼.";
        }
    }
    return true;
}

PlayerChangePasswordDialog::PlayerChangePasswordDialog(int playerid) :
    InputDialog(playerid, "��������"),
    mState(INIT)
{
}

bool PlayerChangePasswordDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("������һ��6λ���ϵ�����.");
        break;
        case FAIL:
        setMessage("������벻����Ҫ��, ���߷������������ڲ�����.\n"
            "�����³���, ����������ִ������鷳��ϵ����Ա.");
        break;
    }
    return true;
}

bool PlayerChangePasswordDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "δ�ҵ������.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�㻹û�е�¼.");
        return true;
    }
    if(!p->changePassword(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "������ĳɹ�.");
        return true;
    }
}

PlayerChangeLogNameDialog::PlayerChangeLogNameDialog(int playerid) :
    InputDialog(playerid, "������Ϸ�ʺ�"),
    mState(INIT)
{
}

bool PlayerChangeLogNameDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("������һ���µ��ʺ�.");
        break;
        case FAIL:
        setMessage("���Ʋ�����Ҫ��, ���߷������������ڲ�����.\n"
            "������. ����������ִ������鷳��ϵ����Ա.");
        break;
    }
    return true;
}

bool PlayerChangeLogNameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "δ�ҵ������.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�㻹û�е�¼.");
        return true;
    }
    if(!p->setLogName(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "��Ϸ�ʺŸ��ĳɹ�.");
        return true;
    }
}

PlayerChangeNicknameDialog::PlayerChangeNicknameDialog(int playerid,
    bool force
) :
    InputDialog(playerid, "�����ǳ�"),
    mState(INIT), mForce(force)
{
}

bool PlayerChangeNicknameDialog::build()
{
    switch(mState)
    {
        case INIT:
        setMessage("������һ���µ�����.");
        break;
        case FAIL:
        setMessage("���Ʋ�����Ҫ��, ���߷������������ڲ�����.\n"
            "������. ����������ִ������鷳��ϵ����Ա.");
        break;
        case MUST_SET:
        setMessage("����������ǳƲ��ܽ�����Ϸ.");
        break;
    }
    return true;
}

bool PlayerChangeNicknameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        mState = MUST_SET;
        return !mForce;
    }
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "δ�ҵ������.");
        LOG(ERROR) << "Player not found.";
        return true;
    }
    if(!p->isLoggedIn())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�㻹û�е�¼.");
        return true;
    }
    if(!p->setNickname(inputtext))
    {
        mState = FAIL;
        return false;
    }
    else
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�ǳƸ��ĳɹ�.");
        return true;
    }
}

PlayerControlDialog::PlayerControlDialog(int playerid, int targetplayer)
    : MenuDialog(playerid, "���"), mTargetPlayer(targetplayer)
{
}

bool PlayerControlDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr || !p->isLoggedIn())
    {
        return false;
    }
    setTitle(target->getLogName());

    int playerid = mPlayerId, targetid = mTargetPlayer;

    // General Functions
    // * Private Message
    addItem("��������", [playerid, targetid]() {
        DialogManager::get().push<PlayerSendMessageDialog>(
            playerid, targetid
        );
    });
    // * View Profile
    addItem("�鿴����", [playerid, targetid]() {
        DialogManager::get().push<PlayerViewProfileDialog>(
            playerid, targetid
        );
    });
    // * Spectate

    // Police Rank POLICE_OFFICER
    int rank = p->getPoliceRank();

    if(rank > CIVILIAN && target->getWantedLevel() > 0)
    {
        float x, y, z;
        GetPlayerPos(targetid, &x, &y, &z);
        float d = GetPlayerDistanceFromPoint(playerid, x, y, z);
        if(d < 10.0)
        {
            addItem("����", [playerid, targetid]() {
                DialogManager::get().push<ArrestDialog>(playerid, targetid);
            });
        }
    }

    if(rank >= POLICE_OFFICER && rank < POLICE_DEPUTY_CHIEF)
    {
        if(!target->hasFlags(STATUS_JAILED))
        {
            addItem("ͨ��", [playerid, targetid]() {
                DialogManager::get().push<PlayerSetWantedLevelDialog>(
                    playerid, targetid, 1, 2
                );
            });
        }
    }

    // Police Rank POLICE_DEPUTY_CHIEF
    if(rank >= POLICE_DEPUTY_CHIEF && rank < CHIEF_OF_POLICE)
    {
        if(!target->hasFlags(STATUS_JAILED))
        {
            addItem("ͨ��", [playerid, targetid]() {
                DialogManager::get().push<PlayerSetWantedLevelDialog>(
                    playerid, targetid, 0, 4
                );
            });
        }
    }

    // Police Rank CHIEF_OF_POLICE
    if(rank == CHIEF_OF_POLICE)
    {
        if(!target->hasFlags(STATUS_JAILED))
        {
            addItem("ͨ��", [playerid, targetid]() {
                DialogManager::get().push<PlayerSetWantedLevelDialog>(
                    playerid, targetid, 0, 6
                );
            });
        }
    }

    // Police Rank >= POLICE_DEPUTY_CHIEF
    if(rank >= POLICE_DEPUTY_CHIEF)
    {
        if(target->hasFlags(STATUS_JAILED))
        {
            addItem("�ͷ�", [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->freeFromPrison();
            });
        }
    }

    // Admin Level 1
    if(p->getAdminLevel() >= 1)
    {
        // * Go To
        addItem("���͹�ȥ", [playerid, targetid]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p != nullptr) p->teleportTo(targetid);
        });
        // * Get There
        addItem("������", [playerid, targetid]() {
            auto target = PlayerManager::get().getPlayer(targetid);
            if(target != nullptr) target->teleportTo(playerid);
        });
        // * Mute / Unmute
        if(target->hasFlags(STATUS_MUTED))
        {
            addItem("������", [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->removeFlags(STATUS_MUTED);
            });
        }
        else
        {
            addItem("��ֹ����", [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->addFlags(STATUS_MUTED);
            });
        }
        // * Eject
        if(IsPlayerInAnyVehicle(targetid))
        {
            addItem("���³�",
                std::bind(&RemovePlayerFromVehicle, targetid)
            );
        }
    }

    // Admin Level 2
    if(p->getAdminLevel() >= 2)
    {
        // * Freeze / Unfreeze
        if(target->hasFlags(STATUS_FREEZED))
        {
            addItem("�ⶳ", [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->removeFlags(STATUS_FREEZED);
                TogglePlayerControllable(targetid, 1);
            });
        }
        else
        {
            addItem("����", [targetid]() {
                auto target = PlayerManager::get().getPlayer(targetid);
                if(target != nullptr) target->addFlags(STATUS_FREEZED);
                TogglePlayerControllable(targetid, 0);
            });
        }
        // * Kill
        addItem("��������",
            std::bind(&SetPlayerHealth, targetid, 0.0)
        );
        // * Force Respawn
        addItem("ǿ������ѡ��", [targetid]() {
            ForceClassSelection(targetid);
            SetPlayerHealth(targetid, 0.0);
        });
        // * Kick
        addItem("�߳�������",
            std::bind(&Kick, targetid)
        );
    }

    // Admin Level 3
    if(p->getAdminLevel() >= 3)
    {
        // * Explode
        addItem("��ը", [targetid]() {
            float x, y, z;
            GetPlayerPos(targetid, &x, &y, &z);
            CreateExplosion(x, y, z, 0, 5.0);
        });
        // * Ban
        addItem("���", [targetid]() {
            auto target = PlayerManager::get().getPlayer(targetid);
            if(target != nullptr) target->addFlags(STATUS_BANNED);
            Kick(targetid);
        });
        // * Set Admin Level
        addItem("���Ĺ���Ա�ȼ�",
        [playerid, targetid]() {
            DialogManager::get().push<PlayerSetAdminLevelDialog>(
                playerid, targetid
            );
        });
        // * Set Police Rank
        addItem("���ľ���",
        [playerid, targetid]() {
            DialogManager::get().push<PlayerSetPoliceRankDialog>(
                playerid, targetid
            );
        });
    }
    return true;
}

PlayerSendMessageDialog::PlayerSendMessageDialog(int playerid, int target) :
    InputDialog(playerid, "��������"), mTargetPlayer(target)
{
}

bool PlayerSendMessageDialog::build()
{
    setMessage("��������Ҫ���͵���Ϣ");
    return true;
}

bool PlayerSendMessageDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response)
    {
        return true;
    }
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return true;
    }
    if(!IsPlayerConnected(mTargetPlayer))
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "δ�ҵ������.");
        return true;
    }
    std::stringstream info;
    info << "��Ϣ���͸� " << target->getColoredNickname()
        << "{FFFFFF}(" << mTargetPlayer << "): " << inputtext;
    SendClientMessage(mPlayerId, 0xFFFFFFFF, info.str().c_str());
    std::stringstream msg;
    msg << "��Ϣ���� " << p->getColoredNickname()
        << "{FFFFFF}(" << mPlayerId << "): " << inputtext;
    SendClientMessage(mTargetPlayer, 0xFFFFFFFF, msg.str().c_str());
    LOG(INFO) << "Private message sent from " << p->getLogName()
        << " to " << target->getLogName() << ": " << inputtext;
    return false;
}

PlayerSetAdminLevelDialog::PlayerSetAdminLevelDialog(int playerid,
    int target) : RadioListDialog<int>(playerid, "���Ĺ���Ա�ȼ�"),
    mTargetPlayer(target)
{
}

bool PlayerSetAdminLevelDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return false;
    }
    addItem(0, "�޹���Ȩ��", target->getAdminLevel() == 0);
    addItem(1, "1�� ����Ա", target->getAdminLevel() == 1);
    addItem(2, "2�� ����Ա", target->getAdminLevel() == 2);
    addItem(3, "3�� ����", target->getAdminLevel() == 3);
    return true;
}

bool PlayerSetAdminLevelDialog::process(int adminlevel)
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    if(target != nullptr)
    {
        return target->setAdminLevel(adminlevel);
    }
    return false;
}

PlayerSetPoliceRankDialog::PlayerSetPoliceRankDialog(int playerid,
    int target) : RadioListDialog<PoliceRank>(playerid, "���þ���"),
    mTargetPlayer(target)
{
}

bool PlayerSetPoliceRankDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    if(target == nullptr)
    {
        return false;
    }
    addItem(CIVILIAN, PoliceRankString[CIVILIAN],
        target->getPoliceRank() == CIVILIAN);
    addItem(POLICE_OFFICER, PoliceRankString[POLICE_OFFICER],
        target->getPoliceRank() == POLICE_OFFICER);
    addItem(POLICE_DEPUTY_CHIEF, PoliceRankString[POLICE_DEPUTY_CHIEF],
        target->getPoliceRank() == POLICE_DEPUTY_CHIEF);
    addItem(CHIEF_OF_POLICE, PoliceRankString[CHIEF_OF_POLICE],
        target->getPoliceRank() == CHIEF_OF_POLICE);
    return true;
}

bool PlayerSetPoliceRankDialog::process(PoliceRank rank)
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    if(target != nullptr)
    {
        return target->setPoliceRank(rank);
    }
    return false;
}

PlayerControlPanelDialog::PlayerControlPanelDialog(int playerid) :
    MenuDialog(playerid, "�������")
{
}

bool PlayerControlPanelDialog::build()
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr || !p->isLoggedIn())
    {
        return false;
    }
    int playerid = mPlayerId;
    // General Functions
    // * Weapon Shop
    addItem("�����̵�", [playerid]() {
        DialogManager::get().push<WeaponShopDialog>(playerid);
    });
    // * Create Teleport
    addItem("�ڴ˴������͵�", [playerid]() {
        DialogManager::get().push<CreateTeleportDialog>(playerid);
    });
    // * Profile
    addItem("�ҵ�����", [playerid]() {
        DialogManager::get().push<PlayerEditProfileDialog>(playerid);
    });
    // * Crew
    addItem("����", [playerid]() {
        DialogManager::get().push<CrewControlPanelDialog>(playerid);
    });
    // * Property
    addItem("����", [playerid]() {
        DialogManager::get().push<PropertyDialog>(playerid);
    });
    // * Invincible
    if(p->hasFlags(STATUS_INVINCIBLE))
    {
        addItem("�����޵�", [playerid]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            p->removeFlags(STATUS_INVINCIBLE);
            SetPlayerHealth(playerid, 100.0);
            SendClientMessage(playerid, 0xFFFFFFFF, "���ѽ����޵�.");
        });
    }
    else
    {
        addItem("�����޵�", [playerid]() {
            auto p = PlayerManager::get().getPlayer(playerid);
            if(p == nullptr || !p->isLoggedIn())
            {
                return;
            }
            p->addFlags(STATUS_INVINCIBLE);
            SendClientMessage(playerid, 0xFFFFFFFF, "���������޵�.");
        });
    }
    // Admin Level 3
    // * Map Manager
    if(p->getAdminLevel() >= 3)
    {
        addItem("��ͼ����", [playerid]() {
            DialogManager::get().push<MapManagerDialog>(playerid);
        });
    }
    // Police System
    if(p->getWantedLevel() > 0)
    {
        addItem("����", [playerid]() {
            DialogManager::get().push<ArrestSurrenderDialog>(playerid);
        });
    }
    return true;
}

PlayerSetWantedLevelDialog::PlayerSetWantedLevelDialog(int playerid,
    int target, int minlevel, int maxlevel) :
    RadioListDialog<int>(playerid, "����ͨ���ȼ�"),
    mTargetPlayer(target), mMin(minlevel < 0 ? 0 : minlevel),
    mMax(maxlevel > 6 ? 6 : maxlevel)
{
}

bool PlayerSetWantedLevelDialog::build()
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return false;
    }
    if(0 >= mMin && 0 <= mMax)
        addItem(0, "��ͨ��", target->getWantedLevel() == 0);
    if(1 >= mMin && 1 <= mMax)
        addItem(1, "һ��ͨ��", target->getWantedLevel() == 1);
    if(2 >= mMin && 2 <= mMax)
        addItem(2, "����ͨ��", target->getWantedLevel() == 2);
    if(3 >= mMin && 3 <= mMax)
        addItem(3, "����ͨ��", target->getWantedLevel() == 3);
    if(4 >= mMin && 4 <= mMax)
        addItem(4, "����ͨ��", target->getWantedLevel() == 4);
    if(5 >= mMin && 5 <= mMax)
        addItem(5, "����ͨ��", target->getWantedLevel() == 5);
    if(6 >= mMin && 6 <= mMax)
        addItem(6, "����ͨ��", target->getWantedLevel() == 6);
    return true;
}

bool PlayerSetWantedLevelDialog::process(int level)
{
    auto target = PlayerManager::get().getPlayer(mTargetPlayer);
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(target == nullptr || p == nullptr)
    {
        return false;
    }
    // You can't down set wanted level if it's beyond your authority.
    if(target->getWantedLevel() > level && target->getWantedLevel() > mMax)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF,
            "��û��Ȩ�޽�������ͨ���ȼ�.");
        return false;
    }
    target->setWantedLevel(level);
    if(level > 0)
    {
        DialogManager::get().push<ArrestSurrenderDialog>(mTargetPlayer);
    }
    return true;
}

ArrestSurrenderDialog::ArrestSurrenderDialog(int playerid) :
    ConfirmDialog(playerid, "ͨ��")
{
}

bool ArrestSurrenderDialog::build()
{
    setMessage("�㱻ͨ����, ���������(ֻ��Ѻ������)������. ��Ҫ������?");
    return true;
}

bool ArrestSurrenderDialog::handleCallback(
    bool response, int /* listitem */, const std::string& /* inputtext */)
{
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr) return true;
    if(response && p->getWantedLevel() > 0)
    {
        p->putIntoPrison(60 * 2);
    }
    return true;
}

ArrestDialog::ArrestDialog(int playerid, int target) :
    MenuDialog(playerid, "����"),
    mTarget(target)
{
}

bool ArrestDialog::build()
{
    int targetid = mTarget;
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr) return false;
    addItem("��Ѻ5����", [targetid]() {
        auto target = PlayerManager::get().getPlayer(targetid);
        if(target != nullptr && target->getWantedLevel() > 0)
            target->putIntoPrison(60 * 5);
    });
    addItem("��Ѻ10����", [targetid]() {
        auto target = PlayerManager::get().getPlayer(targetid);
        if(target != nullptr && target->getWantedLevel() > 0)
            target->putIntoPrison(60 * 10);
    });
    addItem("��Ѻ15����", [targetid]() {
        auto target = PlayerManager::get().getPlayer(targetid);
        if(target != nullptr && target->getWantedLevel() > 0)
            target->putIntoPrison(60 * 15);
    });
    return true;
}

bool ArrestDialog::handleCallback(
    bool response, int listitem, const std::string &inputtext)
{
    MenuDialog::handleCallback(response, listitem, inputtext);
    return true;
}

CreateTeleportDialog::CreateTeleportDialog(int playerid) :
    InputDialog(playerid, "�������͵�")
{
}

bool CreateTeleportDialog::build()
{
    setMessage("������͵�����");
    return true;
}

bool CreateTeleportDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response) return true;
    auto p = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr) return false;
    return p->createTeleport(inputtext);
}

}
