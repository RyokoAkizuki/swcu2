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

#include "../Common/Common.hpp"
#include "../Utility/Singleton.hpp"

namespace swcu {

class Crew;

class CrewManager : public Singleton<CrewManager>
{
protected:
    std::unordered_map<std::string, std::shared_ptr<Crew>> mCrews;

protected:
                    CrewManager();
    friend class Singleton<CrewManager>;

public:
    virtual         ~CrewManager() {}

            
            std::shared_ptr<Crew>   getCrew(const mongo::OID& id);
};

}
