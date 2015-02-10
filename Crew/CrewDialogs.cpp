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

#include <memory>

#include "../Interface/DialogManager.hpp"
#include "../Player/PlayerManager.hpp"
#include "Crew.hpp"
#include "CrewManager.hpp"

#include "CrewDialogs.hpp"

namespace swcu {

CrewControlPanelDialog::CrewControlPanelDialog(int playerid) :
    MenuDialog(playerid, "����")
{
}

bool CrewControlPanelDialog::build()
{
    auto p          = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr)
    {
        return false;
    }
    int playerid    = mPlayerId;
    mongo::OID oid  = p->getId();
    if(p->isCrewMember())
    {
        auto crew = CrewManager::get().getCrew(p->getCrew());
        mongo::OID crewid = crew->getId();
        addItem("�ҵİ���: " + crew->getColoredName(), [](){});
        if(p->getId() != crew->getLeader())
            addItem("�˳�����", std::bind(&Crew::removeMember, crew, oid));
        else
        {
            addItem("���İ�������", [playerid, crewid]() {
                DialogManager::get().push<CrewChangeNameDialog>(
                    playerid, crewid);
            });
            addItem("�鿴���ɳ�Ա", [playerid, crewid]() {
                DialogManager::get().push<CrewViewMembersDialog>(
                    playerid, crewid);
            });
        }
    }
    else
    {
        addItem("��������", [playerid]() {
            DialogManager::get().push<CreateCrewDialog>(playerid);
        });
        addItem("�������", [playerid, oid]() {
            DialogManager::get().push<CrewFindByNameDialog>(
            playerid, [playerid, oid](const mongo::OID& crew) {
                if(CrewManager::get().getCrew(crew)->applyToJoin(oid))
                {
                    SendClientMessage(playerid, 0xFFFFFFFF,
                        "���������ѷ���.");
                    return true;
                }
                return false;
            });
        });
    }
    return true;
}

CrewViewMembersDialog::CrewViewMembersDialog(
    int playerid, const mongo::OID& crew) :
    ItemListDialog<std::string>(playerid, "���ɳ�Ա"), mCrew(crew)
{
}

bool CrewViewMembersDialog::build()
{
    MONGO_WRAPPER({
        auto doc        = getDBConn()->findOne(
            Config::colNameCrew,
            BSON("_id" << mCrew)
        );
        auto members    = doc["members"].Obj();
        auto it         = mongo::BSONObjIterator(members);
        for(; it.more(); ++it)
        {
            auto member                 = it.next();
            std::string memberIdStr     = member.fieldName();
            mongo::OID memberId         = mongo::OID(memberIdStr);
            auto prof                   = getDBConn()->findOne(
                Config::colNamePlayer,
                BSON("_id" << memberId)
            );
            std::string pName           = prof["logname"].str();
            std::stringstream msg;
            msg << getCrewHierarchyStr(
                CrewHierarchy(member.numberInt()))
                << "\t" << pName;
            addItem(memberIdStr, msg.str());
        }
    });
    return true;
}

bool CrewViewMembersDialog::process(std::string key)
{
    DialogManager::get().push<CrewEditMemberDialog>(
        mPlayerId, mCrew, mongo::OID(key));
    return true;
}

CrewEditMemberDialog::CrewEditMemberDialog(int playerid,
    const mongo::OID& crew, const mongo::OID& member) :
    MenuDialog(playerid, "�༭��Ա"), mCrew(crew), mMember(member)
{
}

bool CrewEditMemberDialog::build()
{
    auto crew       = CrewManager::get().getCrew(mCrew);
    if(crew->getLeader() == mMember) return false;
    Player p(mMember);
    auto hier       = crew->getMemberHierarchy(mMember);
    mongo::OID poid = mMember;
    int playerid    = mPlayerId;
    mongo::OID cid  = crew->getId();

    addItem("�û���: " + p.getLogName(), [](){});

    if(hier == PENDING)
    {
        addItem("��׼���", std::bind(&Crew::approveToJoin, crew, poid));
    }
    else if(hier > PENDING)
    {
        addItem(std::string("�׼�: ") + getCrewHierarchyStr(
            crew->getMemberHierarchy(mMember)), [playerid, cid, poid]() {
            DialogManager::get().push<CrewMemberSetHierarchy>(
                playerid, cid, poid);
        });
        addItem("����", std::bind(&Crew::removeMember, crew, poid));
    }
    return true;
}

CrewChangeNameDialog::CrewChangeNameDialog(int playerid,
    const mongo::OID& crew) : InputDialog(playerid, "���İ�������"),
    mCrew(crew)
{
}

CrewMemberSetHierarchy::CrewMemberSetHierarchy(int playerid,
    const mongo::OID& crew, const mongo::OID& mem) :
    RadioListDialog<int>(playerid, "���ó�Ա�׼�"), mCrew(crew), mMember(mem)
{
}

bool CrewMemberSetHierarchy::build()
{
    CrewHierarchy hier = CrewManager::get().getCrew(mCrew)
        ->getMemberHierarchy(mMember);
    addItem(COMMISSIONERS,
        "����    \t�������ֵ�������ĳ�Ա��",
        hier == COMMISSIONERS
    );
    addItem(LIEUTENANTS,
        "�ɲ�    \t�����Ա����Ȩ�ޣ��Ա�����ᡣ",
        hier == LIEUTENANTS
    );
    addItem(REPRESENTATIVES,
        "��ͷ����\t֤���Լ��԰������ĳ�Ա��",
        hier == REPRESENTATIVES
    );
    addItem(MUSCLE,
        "����    \t���а���Ա��",
        hier == MUSCLE
    );
    return true;
}

bool CrewMemberSetHierarchy::process(int hier)
{
    return CrewManager::get().getCrew(mCrew)->setMemberHierarchy(
        mMember, CrewHierarchy(hier));
}

bool CrewChangeNameDialog::build()
{
    setMessage("�����µ�����");
    return true;
}

bool CrewChangeNameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response) return true;
    if(inputtext.length() == 0)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�������Ʋ���Ϊ��");
        return false;
    }
    auto crew       = CrewManager::get().getCrew(mCrew);
    auto p          = PlayerManager::get().getPlayer(mPlayerId);
    if(p == nullptr) return true;
    if(crew->getLeader() != p->getId())
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�㲻�ǰ�������");
        return true;
    }
    if(crew->setName(inputtext))
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "���Ƹ��ĳɹ�");
        return true;
    }
    SendClientMessage(mPlayerId, 0xFFFFFFFF, "���Ƹ���ʧ��");
    return false;
}

CreateCrewDialog::CreateCrewDialog(int playerid) :
    InputDialog(playerid, "��������")
{
}

bool CreateCrewDialog::build()
{
    setMessage("�����İ�������");
    return true;
}

bool CreateCrewDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(!response) return true;
    if(inputtext.length() == 0)
    {
        SendClientMessage(mPlayerId, 0xFFFFFFFF, "�������Ʋ���Ϊ��");
        return false;
    }
    std::unique_ptr<Crew> crew(new Crew(inputtext));
    if(crew->isValid() && !crew->getLeader().isSet())
    {
        auto p = PlayerManager::get().getPlayer(mPlayerId);
        if(p != nullptr)
        {
            crew->setLeader(p->getId());
            SendClientMessage(mPlayerId, 0xFFFFFFFF, "���ɴ����ɹ�");
            return true;
        }
    }
    SendClientMessage(mPlayerId, 0xFFFFFFFF, "���ɴ���ʧ��");
    return false;
}

CrewFindByNameDialog::CrewFindByNameDialog(int playerid, CallbackType cb) :
    InputDialog(playerid, "���Ұ���"), mCallback(cb)
{
}

bool CrewFindByNameDialog::build()
{
    setMessage("��������ɵ�����");
    return true;
}

bool CrewFindByNameDialog::handleCallback(
    bool response, int /* listitem */, const std::string &inputtext)
{
    if(response)
    {
        DialogManager::get().push<_CrewFindByNameResultDialog>(
            mPlayerId, inputtext, mCallback);
    }
    return true;
}

_CrewFindByNameResultDialog::_CrewFindByNameResultDialog(
    int playerid, const std::string& keyword, CallbackType cb) :
    ItemListDialog<std::string>(playerid, "���Ұ���"),
    mKeyWord(keyword), mCallback(cb)
{
}

bool _CrewFindByNameResultDialog::build()
{
    MONGO_WRAPPER({
        auto cur = getDBConn()->query(
            Config::colNameCrew,
            QUERY("name" << BSON("$regex" << GBKToUTF8(mKeyWord)))
        );
        while(cur->more())
        {
            auto doc = cur->next();
            addItem(doc["_id"].OID().str(), UTF8ToGBK(doc["name"].str()));
        }
    });
    return true;
}

bool _CrewFindByNameResultDialog::process(std::string key)
{
    return mCallback(mongo::OID(key));
}

}
