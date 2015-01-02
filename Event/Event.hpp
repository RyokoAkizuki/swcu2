/*
 * Copyright 2015 Yukino Hayakawa<tennencoll@gmail.com>
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

#include <utility>
#include <boost/any.hpp>
#include <unordered_set>

#include "../Utility/Singleton.hpp"

namespace swcu {

enum EventType
{
    onPlayerEnterServer,            // Player*
    onPlayerExitServer,             // Player*
    onPlayerSpawn,                  // Player*
    onPlayerDeath,                  // Player*
    onPlayerNicknameChanged,        // Player*
    onPlayerLogNameChanged,         // Player*
    onPlayerMoneyAmountChanged,     // Player*
    onPlayerJailed,                 // Player*
    onPlayerUnjailed,               // Player*
    onPlayerAdminLevelChanged,      // Player*
    onPlayerPoliceRankChanged,      // Player*
    onPlayerWantedLevelChanged,     // Player*
    onPlayerColorChanged,           // Player*
    onCrewPlayerApplyToJoin,        // Crew*, OID:ProfileId
    onCrewPlayerApprovedToJoin,     // Crew*, OID:ProfileId
    onCrewPlayerDeniedToJoin,       // Crew*, OID:ProfileId
    onCrewMemberAdded,              // Crew*, OID:ProfileId
    onCrewMemberRemoved,            // Crew*, OID:ProfileId
    onCrewMemberHierarchyChanged,   // Crew*, OID:ProfileId
    onCrewLeaderChanged,            // Crew*
    onCrewColorChanged,             // Crew*
    onCrewNameChanged,              // Crew*

    invalidEvent
};

typedef std::tuple<EventType, boost::any, boost::any> Event;

class EventListener;

class EventManager : public Singleton<EventManager>
{
protected:
    std::unordered_set<EventListener*>  mListeners;

protected:
                    EventManager() {}

    friend class Singleton<EventManager>;

public:
    virtual         ~EventManager() {}
            void    _addListener(EventListener* listener);
            void    _removeListener(EventListener* listener);
            void    sendEvent(EventType type,
                boost::any p1 = nullptr, boost::any p2 = nullptr);
};

class EventListener
{
public:
                    EventListener();
    virtual         ~EventListener();
    virtual void    handleEvent(const Event& evt) = 0;
};

}
