/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @file    cinek/string.cpp
 * @author  Samir Sinha
 * @date    2/22/2014
 * @brief   std::string with custom allocators
 * @copyright Cinekine
 */


#include "cinek/string.hpp"

#include <string>
#include <sstream>
#include <cstring>


namespace cinek {

    std::string directoryPath(const std::initializer_list<std::string>& elements)
    {
        const char kSep = '/';
        std::string path;
        path.reserve(MAX_PATH);
        auto it = elements.begin();
        auto itEnd = elements.end();
        if (it != itEnd)
        {
            path = *it;
            if (path.back() != kSep)
                path.push_back(kSep);
            ++it;
            for (; it != itEnd; ++it)
            {
                auto& element = *it;
                size_t len = element.length();
                size_t offset = 0;
                if (element.front() == kSep)
                {
                    ++offset;
                    --len;
                }
                path.append(element, offset, len);
                //  this logic is almost the same as the first element
                //  check done prior to the for loop - only that the first
                //  path element can have a forward and back path separator
                if (path.back() != kSep)
                    path.push_back(kSep);
            }
        }
        return path;
    }

} /* cinekine */
