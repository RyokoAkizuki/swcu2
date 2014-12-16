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

#include "Config.hpp"

namespace swcu {

std::string Config::dbHost              = "localhost";
std::string Config::colNameMap          = "swcu2.map";
std::string Config::colNameMapObject    = "swcu2.map.object";
std::string Config::colNameMapVehicle   = "swcu2.map.vehicle";
std::string Config::colNamePlayer       = "swcu2.playerprofile";
std::string Config::colNameTeleport     = "swcu2.teleport";
int         Config::webServerPort       = 8081;
size_t      Config::webServerThread     = 4;

}
