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

#include "Event.hpp"

namespace swcu {

void EventManager::_addListener(EventListener* listener)
{
    mListeners.insert(listener);
}

void EventManager::_removeListener(EventListener* listener)
{
    mListeners.erase(listener);
}

void EventManager::sendEvent(EventType type, boost::any p1, boost::any p2)
{
    Event evt(type, p1, p2);
    for(auto &i : mListeners)
    {
        i->handleEvent(evt);
    }   
}

EventListener::EventListener()
{
    EventManager::get()._addListener(this);
}

EventListener::~EventListener()
{
    EventManager::get()._removeListener(this);
}

}
