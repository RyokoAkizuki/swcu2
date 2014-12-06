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

#include "../Player/PlayerManager.hpp"
#include "../Common/Common.hpp"

#include "Language.hpp"

namespace swcu {

extern const char* LANG_ZH_CN[];
extern const char* LANG_EN_US[];

const char** LANGUAGES[MAX_LANGUAGES] = {
    LANG_ZH_CN,
    LANG_EN_US
};

const char* t(int playerid, int entry)
{
    auto p = PlayerManager::get().getPlayer(playerid);
    return t(p, entry);
}

const char* t(Player* player, int entry)
{
    int lang = (player != nullptr) ? player->getLanguage() : 0;
    if(lang > MAX_LANGUAGES - 1 || entry > LANG_ENTRY_END)
    {
        LOG(ERROR) << "Language entry not found.";
        return "LANG_ENTRY_OUT_OF_BOUND";
    }
    return LANGUAGES[lang][entry];
}

}
