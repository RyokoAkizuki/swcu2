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

#include <iconv.h>
#include <memory>
#include "sha1.h"
#include "EncodingUtility.hpp"
#include "easylogging++.h"

namespace swcu {

std::string sha1(const std::string& src)
{
    unsigned char hash[20];
    char hexstring[41];
    sha1::calc(src.c_str(), src.length(), hash);
    sha1::toHexString(hash, hexstring);
    return hexstring;
}

/**
 * From http://my.oschina.net/xiaot99/blog/204041
 */
int convert(const char *from_charset, const char *to_charset,
    char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    iconv_t     cd;
    char        **pin   = &inbuf;
    char        **pout  = &outbuf;

    cd = iconv_open(to_charset, from_charset);
    if(cd == 0)
    {
        LOG(ERROR) << "Can't convert from " << from_charset << " to "
            << to_charset << ".";
        return -1;
    }
    memset(outbuf, 0, outlen);
    if(iconv(cd, pin, &inlen, pout, &outlen) == static_cast<size_t>(-1))
    {
        LOG(ERROR) << "Convertion from " << from_charset << " to "
            << to_charset << " failed.";
        return -1;
    }
    iconv_close(cd);
    return 0;
}

std::string GBKToUTF8(const std::string& src)
{
    size_t len = src.size();
    std::unique_ptr<char[]> srcCopy(new char[len]);
    std::unique_ptr<char[]> dest(new char[len * 3]);
    memset(dest.get(), 0, sizeof(*dest.get()));
    memcpy(srcCopy.get(), src.c_str(), src.size());
    int r = convert("GBK", "UTF-8", srcCopy.get(), len, dest.get(), len * 3);
    std::string retStr;
    if(r == 0)
    {
        retStr = dest.get();
    }
    return retStr;
}

std::string UTF8ToGBK(const std::string& src)
{
    size_t len = src.size();
    std::unique_ptr<char[]> srcCopy(new char[len]);
    std::unique_ptr<char[]> dest(new char[len]);
    memset(dest.get(), 0, sizeof(*dest.get()));
    memcpy(srcCopy.get(), src.c_str(), src.size());
    int r = convert("UTF-8", "GBK", srcCopy.get(), len, dest.get(), len);
    std::string retStr;
    if(r == 0)
    {
        retStr = dest.get();
    }
    return retStr;
}

}
