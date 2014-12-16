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

#include "Common.hpp"

namespace swcu {

class MongoDBInitializer
{
public:
    mongo::DBClientConnection   mConn;

public:
    MongoDBInitializer() : mConn(true)
    {
        MONGO_WRAPPER({
            mConn.connect(Config::dbHost);
            LOG(INFO) << "Connected to " << Config::dbHost;
        });
    }
};

mongo::DBClientConnection* getDBConn()
{
    static MongoDBInitializer db;
    return &db.mConn;
}

bool dbCheckError()
{
    auto err = getDBConn()->getLastError();
    if (err.size())
    {
        LOG(ERROR) << err;
        return false;
    }
    return true;
}

}
