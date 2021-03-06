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
 
#include <mongo/client/dbclient.h>
#include <sstream>

#include "Internal/easylogging++.h"
#include "Internal/EncodingUtility.hpp"
#include "Internal/StringFuncUtil.hpp"
#include "Internal/Config.hpp"
 
/********** Mongo Exception Handler Wrapper **********/

#define MONGO_WRAPPER(x) \
    try \
    { \
        x \
    } \
    catch (const mongo::DBException &e) \
    { \
        LOG(ERROR) << e.what(); \
    } \
    catch (const std::exception& e) \
    { \
        LOG(ERROR) << e.what(); \
    } \
    catch (...) \
    { \
        LOG(ERROR) << "Unknown error."; \
    } \
    do {} while (false)

#define STR(x) ((dynamic_cast<std::ostringstream&>( \
    std::ostringstream() << x)).str())
#define CSTR(x) ((dynamic_cast<std::ostringstream&>( \
    std::ostringstream() << x)).str().c_str())

namespace swcu {

mongo::DBClientConnection*  getDBConn();
bool                        dbCheckError();


}
