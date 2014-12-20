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

#include "server_http.hpp"

#include "../Common/Common.hpp"

#include "WebServiceManager.hpp"

namespace swcu {

const char* gContentTypes[CONTENT_TYPE_END] = {
    "text/plain",
    "text/html",
    "application/json",
    "application/javascript"
};

WebServiceManager::WebServiceManager()
{
    mServer.reset(new SimpleWeb::Server<SimpleWeb::HTTP>(
        Config::webServerPort, Config::webServerThread));

    // The following codes are from Simple-Web-Server.
    // Please refer to its license.
    mServer->default_resource["^/?(.*)$"]["GET"] = 
    [](std::ostream& response, HTTPRequertPtr request) {
        std::string     filename    = "web/";
        std::string     path        = request->path_match[1];

        // Replace all ".." with "." (so we can't leave the web-directory)
        size_t          pos;
        while((pos = path.find("..")) != std::string::npos)
        {
            path.erase(pos, 1);
        }

        filename += path;
        std::ifstream   ifs;
        // A simple platform-independent file-or-directory check do not
        // exist, but this works in most of the cases:
        if(filename.find('.') == std::string::npos)
        {
            if(filename[filename.length() - 1] != '/')
                filename += '/';
            filename += "index.html";
        }
        ifs.open(filename, std::ifstream::in);
        if(ifs)
        {
            ifs.seekg(0, std::ios::end);
            size_t      length      = ifs.tellg();
            ifs.seekg(0, std::ios::beg);
            // The file-content is copied to the response-stream. 
            // Should not be used for very large files.
            response << "HTTP/1.1 200 OK\r\nContent-Length: "
                << length << "\r\n\r\n" << ifs.rdbuf();
            ifs.close();
        }
        else {
            std::string content     = "Could not open file " + filename;
            response << "HTTP/1.1 400 Bad Request\r\nContent-Length: "
                << content.length() << "\r\n\r\n" << content;
        }
    };
}

void WebServiceManager::bindMethod(
    const std::string& pattern,
    const std::string& method,
    const WebRequestHandler& handler
)
{
    auto& patternmap = mServer->resource[pattern];
    auto handleriter = patternmap.find(method);
    if(handleriter == patternmap.end())
    {
        patternmap.insert(std::make_pair(method, handler));
    }
    else
    {
        handleriter->second = handler;
        LOG(WARNING) << "Handler overwritten " << method << ":" << pattern;
    }
}

void WebServiceManager::startServer()
{
    mServerThread.reset(new std::thread(
        &SimpleWeb::Server<SimpleWeb::HTTP>::start, mServer.get()));
    LOG(INFO) << "HTTP server started on " << Config::webServerPort;
}

std::unordered_map<int, std::string> gHTTPStatusStrings = {
    { 100, "Continue" },
    { 101, "Switching Protocols" },
    { 102, "Processing" },
    { 200, "OK" },
    { 201, "Created" },
    { 202, "Accepted" },
    { 203, "Non-Authoritative Information" },
    { 204, "No Content" },
    { 205, "Reset Content" },
    { 206, "Partial Content" },
    { 207, "Multi-Status" },
    { 208, "Already Reported" },
    { 226, "IM Used" },
    { 300, "Multiple Choices" },
    { 301, "Moved Permanently" },
    { 302, "Found" },
    { 303, "See Other" },
    { 304, "Not Modified" },
    { 305, "Use Proxy" },
    { 306, "(Unused)" },
    { 307, "Temporary Redirect" },
    { 308, "Permanent Redirect" },
    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Timeout" },
    { 409, "Conflict" },
    { 410, "Gone" },
    { 411, "Length Required" },
    { 412, "Precondition Failed" },
    { 413, "Payload Too Large" },
    { 414, "URI Too Long" },
    { 415, "Unsupported Media Type" },
    { 416, "Range Not Satisfiable" },
    { 417, "Expectation Failed" },
    { 422, "Unprocessable Entity" },
    { 423, "Locked" },
    { 424, "Failed Dependency" },
    { 426, "Upgrade Required" },
    { 428, "Precondition Required" },
    { 429, "Too Many Requests" },
    { 431, "Request Header Fields Too Large" },
    { 500, "Internal Server Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Gateway Timeout" },
    { 505, "HTTP Version Not Supported" },
    { 506, "Variant Also Negotiates" },
    { 507, "Insufficient Storage" },
    { 508, "Loop Detected" },
    { 510, "Not Extended" },
    { 511, "Network Authentication Required" }
};

std::string getStatusString(int status)
{
    auto iter = gHTTPStatusStrings.find(status);
    if(iter != gHTTPStatusStrings.end())
    {
        return iter->second;
    }
    LOG(WARNING) << "Unexpected HTTP status code used.";
    return "";
}

}
