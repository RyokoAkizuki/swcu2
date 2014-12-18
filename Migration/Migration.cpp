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

#include "../Common/Common.hpp"

namespace swcu {

void migratePlayerProfiles()
{
    mongo::DBClientConnection migrationSrcDb(true);
    migrationSrcDb.connect("localhost");
    MONGO_WRAPPER({
        auto oldcur = migrationSrcDb.query(
            "swcuserver.account", mongo::Query() // find all
        );
        while(oldcur->more())
        {
            auto data = oldcur->next();
            auto pte = data["playingtime"];
            int64_t playtime = pte.eoo() ? 0 : pte.numberLong();
            getDBConn()->insert(
                Config::colNamePlayer,
                BSON(
                    "_id"           << data["_id"]          <<
                    "logname"       << data["logname"]      <<
                    "password"      << data["password"]     <<
                    "lang"          << 0                    <<
                    "color"         << 0                    <<
                    "gametime"      << playtime             <<
                    "adminlevel"    << data["adminlevel"]   <<
                    "flags"         << 0                    <<
                    "nickname"      << data["nickname"]     <<
                    "money"         << 0                    <<
                    "jointime"      << data["jointime"]     <<
                    "policerank"    << 0                    <<
                    "wantedlevel"   << 0                    <<
                    "timeinprison"  << 0                    <<
                    "timetofree"    << 0
                )
            );
            MONGO_WRAPPER({ dbCheckError(); });
        }
    });
}

void migrateMaps()
{
    mongo::DBClientConnection migrationSrcDb(true);
    migrationSrcDb.connect("localhost");
    MONGO_WRAPPER({
        auto oldcur = migrationSrcDb.query(
            "swcuserver.map.brief", mongo::Query() // find all
        );
        while(oldcur->more())
        {
            auto data = oldcur->next();
            getDBConn()->insert(
                Config::colNameMap,
                BSON(
                    "_id"           << data["_id"]          <<
                    "type"          << 0                    <<
                    "name"          << data["name"]         <<
                    "owner"         << mongo::OID()         <<
                    "addtime"       << data["date"]         <<
                    "activated"     << data["autoload"]     <<
                    "world"         << -1                   <<
                    "area"          << 0.0
                )
            );
            MONGO_WRAPPER({ dbCheckError(); });
        }
    });
    MONGO_WRAPPER({
        auto oldcur = migrationSrcDb.query(
            "swcuserver.map.detail.object", mongo::Query() // find all
        );
        while(oldcur->more())
        {
            auto data = oldcur->next();
            getDBConn()->insert(
                Config::colNameMapObject,
                BSON(
                    "_id"       << data["_id"]  <<
                    "map"       << data["mapid"]<<
                    "model"     << data["model"]<<
                    "x"         << data["x"]    <<
                    "y"         << data["y"]    <<
                    "z"         << data["z"]    <<
                    "rx"        << data["rx"]   <<
                    "ry"        << data["ry"]   <<
                    "rz"        << data["rz"]   <<
                    "interior"  << -1           <<
                    "editable"  << false
                )
            );
            MONGO_WRAPPER({ dbCheckError(); });
        }
    });
    MONGO_WRAPPER({
        auto oldcur = migrationSrcDb.query(
            "swcuserver.map.detail.vehicle", mongo::Query() // find all
        );
        while(oldcur->more())
        {
            auto data = oldcur->next();
            getDBConn()->insert(
                Config::colNameMapVehicle,
                BSON(
                    "_id"           << data["_id"] <<
                    "map"           << data["mapid"] <<
                    "model"         << data["model"] <<
                    "x"             << data["x"] <<
                    "y"             << data["y"] <<
                    "z"             << data["z"] <<
                    "rotate"        << data["angle"] <<
                    "interior"      << -1 <<
                    "respawndelay"  << 60
                )
            );
            MONGO_WRAPPER({ dbCheckError(); });
        }
    });
}

void migrateHouses()
{

}

}
