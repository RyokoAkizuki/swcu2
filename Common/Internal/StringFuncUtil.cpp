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

#include <sampgdk/a_players.h>
#include <sampgdk/a_samp.h>

#include "StringFuncUtil.hpp"

namespace swcu {

std::string getPlayerNameFixed(int playerid)
{
	std::string realname;
	char temp[24 * 4];
	GetPlayerName(playerid, temp, 24 * 4);
	for (int i = 0; (i < 24 * 4 && temp[i] != 0); ++i)
	{
		if (temp[i] != -1)
		{
			realname.push_back(temp[i]);
		}
	}
	return realname;
}

std::string getPlayerIP(int playerid)
{
	char ip[16];
	GetPlayerIp(playerid, ip, 16);
	return ip;
}

std::string getGPCI(int playerid)
{
	char gpcidest[25];
	gpci(playerid, gpcidest, 25);
	return gpcidest;
}

}
