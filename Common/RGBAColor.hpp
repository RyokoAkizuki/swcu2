/*
 * Copyright 2015 Yukino Hayakawa<tennencoll@gmail.com>
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

#include <sstream>
#include <cstdlib>

namespace swcu {

class RGBAColor
{
protected:
    int8_t mR, mG, mB, mA;

public:
    // Generate a random one.
    RGBAColor()
    {
        float c = (float)rand() / (float)RAND_MAX;
        int32_t rgb = static_cast<int32_t>(
            c * static_cast<float>(0xFFFFFF));
        assignRGB(rgb);
    }

    RGBAColor(int32_t rgba)
    {
        assignRGBA(rgba);
    }

    RGBAColor(int8_t r, int8_t g, int8_t b, int8_t a) :
        mR(r), mG(g), mB(b), mA(a)
    {
    }

    std::string getEmbedCode() const
    {
        char code[24];
        sprintf(code + 1, "%06x", getRGB());
        code[0] = '{'; code[7] = '}';
        return std::string(code, 8);
    }

    int32_t     getRGBA() const
    {
        int r = 0;
        r += mR; r <<= 8; r += mG; r <<= 8;
        r += mB; r <<= 8; r += mA;
        return r;
    }

    int32_t     getRGB() const
    {
        int r = 0;
        r += mR; r <<= 8; r += mG; r <<= 8; r += mB;
        return r;
    }

    void        assignRGBA(int32_t rgba)
    {
        mR = static_cast<int8_t>(rgba >> 24);
        mG = static_cast<int8_t>(rgba >> 16);
        mB = static_cast<int8_t>(rgba >> 8);
        mA = static_cast<int8_t>(rgba);
    }

    void        assignRGB(int32_t rgb)
    {
        mR = static_cast<int8_t>(rgb >> 16);
        mG = static_cast<int8_t>(rgb >> 8);
        mB = static_cast<int8_t>(rgb);
        mA = static_cast<int8_t>(0xFF);
    }

    void        setAlpha(int8_t alpha)
    {
        mA = alpha;
    }
};

}
