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
 * @file    cinek/types.cpp
 * @author  Samir Sinha
 * @date    11/8/2014
 * @brief   types
 * @copyright Cinekine
 */


#include "types.hpp"
#include <cstring>

namespace cinek {

    UUID UUID::kNull = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    bool operator==(const UUID& l, const UUID& r)
    {
        return *(uint64_t*)(&l.bytes[0]) == *(uint64_t*)(&r.bytes[0]) &&
               *(uint64_t*)(&l.bytes[8]) == *(uint64_t*)(&r.bytes[8]);
        //return !memcmp(&l.bytes, &r.bytes, sizeof(UUID::bytes));
    }

    bool operator!=(const UUID& l, const UUID& r)
    {
        return *(uint64_t*)(&l.bytes[0]) != *(uint64_t*)(&r.bytes[0]) ||
               *(uint64_t*)(&l.bytes[8]) != *(uint64_t*)(&r.bytes[8]);
        //return !memcmp(&l.bytes, &r.bytes, sizeof(UUID::bytes));
    }

    bool operator<(const UUID& l, const UUID& r)
    {
        return memcmp(&l.bytes, &r.bytes, sizeof(UUID::bytes)) < 0;
    }
    
    bool operator!(const UUID& l)
    {
        return memcmp(&UUID::kNull.bytes, &l.bytes, sizeof(UUID::bytes)) != 0;
    }


} /* cinekine */
