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
 * @file    cinek/framework/string.hpp
 * @author  Samir Sinha
 * @date    1/6/2013
 * @brief   std::string with custom allocators
 * @copyright Cinekine
 */


#ifndef CINEK_STRING_HPP
#define CINEK_STRING_HPP

#include "cinek/framework/allocator.hpp"

#include <string>
#include <sstream>
#include <cstring>


namespace cinekine {

//  Std types using the overview allocator.
//
/** An allocator for string memory. */
typedef std_allocator<char> string_allocator;
/** A standard C++ string object using the default string allocator. */
typedef std::basic_string<char, std::char_traits<char>, string_allocator > string;
/** A standard C++ stringstream using string_allocator. */
typedef std::basic_stringstream<char, std::char_traits<char>, string_allocator > stringstream;
/** A standard C++ istringstream using string_allocator. */
typedef std::basic_istringstream<char, std::char_traits<char>, string_allocator > istringstream;
/** A standard C++ ostringstream using string_allocator. */
typedef std::basic_ostringstream<char, std::char_traits<char>, string_allocator > ostringstream;

} /* cinekine */

#endif
