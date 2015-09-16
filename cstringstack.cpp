/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Cinekine Media
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
 * @file    cinek/cstringstack.cpp
 * @author  Samir Sinha
 * @date    4/5/2015
 * @brief   String allocation within a pooled heap
 * @copyright Cinekine
 */

#include "cstringstack.hpp"
#include <cstring>

namespace cinek {

    const char* CStringStack::kEmptyString = "";

    CStringStack::CStringStack() :
        _count(0)
    {
    }

    CStringStack::CStringStack(size_t initSize, const Allocator& allocator) :
        _stack(initSize, allocator),
        _count(0)
    {
    }

    CStringStack::CStringStack(CStringStack&& other) :
        _stack(std::move(other._stack)),
        _count(other._count)
    {
        other._count = 0;
    }

    CStringStack& CStringStack::operator=(CStringStack&& other)
    {
        _stack = std::move(other._stack);
        _count = other._count;
        other._count = 0;
        return *this;
    }

    const char* CStringStack::create(const char* str)
    {
        char* buf = reinterpret_cast<char*>(_stack.allocate(strlen(str)+1));
        if (!buf)
            return kEmptyString;
        strcpy(buf, str);
        return buf;
    }

    bool CStringStack::growBy(size_t cnt)
    {
        return _stack.growBy(cnt);
    }

    void CStringStack::reset()
    {
        _stack.reset();
        _count = 0;
    }



} /* namespace cinek */
