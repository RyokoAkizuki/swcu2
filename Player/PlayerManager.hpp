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

#include <unordered_map>
#include <memory>

#include "../Utility/Singleton.hpp"

#include "Player.hpp"
 
namespace swcu {

class PlayerManager : public Singleton<PlayerManager>
{
protected:
    std::unordered_map<int, std::unique_ptr<Player>>    mPlayers;

protected:
                    PlayerManager();
    friend class Singleton<PlayerManager>;
    
public:
    virtual         ~PlayerManager() {}

    virtual Player* addPlayer(int playerid);
    virtual bool    removePlayer(int playerid);
    virtual bool    hasPlayer(int playerid);
    virtual Player* getPlayer(int playerid);
};

}
