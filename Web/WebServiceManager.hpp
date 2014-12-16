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

#include <memory>
#include <thread>
#include "server_http.hpp"

#include "../Utility/Singleton.hpp"

namespace swcu {

typedef std::shared_ptr<SimpleWeb::ServerBase<SimpleWeb::HTTP>::Request>
    HTTPRequertPtr;

class WebServiceManager : public Singleton<WebServiceManager>
{
protected:
    std::unique_ptr<SimpleWeb::Server<SimpleWeb::HTTP>>     mServer;
    std::unique_ptr<std::thread>                            mServerThread;

                    WebServiceManager();
    friend class Singleton<WebServiceManager>;
    
public:
    typedef std::function<void(std::ostream&, HTTPRequertPtr)>
        WebRequestHandler;

    virtual         ~WebServiceManager() {}

    /**
     * IMPORTANT: Methods bound after starting the server won't take effect.
     */
            void    bindMethod(
                const std::string& pattern,
                const std::string& method,
                const WebRequestHandler& handler
            );

            void    startServer();
};

        std::string getStatusString(int status);

inline  void        writeResponse(
    std::ostream& response,
    int status,
    const std::string& content)
{
    response << "HTTP/1.1 " << getStatusString(status) << "\r\n"
        "Content-Length: " << content.length() << "\r\n\r\n"
        << content;
}

}
