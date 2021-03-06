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

#include <string>

namespace swcu {

struct Config
{
    static std::string  dbHost;
    static std::string  colNameMap;
    static std::string  colNameMapObject;
    static std::string  colNameMapVehicle;
    static std::string  colNamePlayer;
    static std::string  colNameTeleport;
    static std::string  colNameCrew;
    static std::string  colNameGangZone;
    static std::string  colNameEventLog;
    static int          webServerPort;
    static size_t       webServerThread;
};

}
