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
 * @file    cinek/map.hpp
 * @author  Samir Sinha
 * @date    1/6/2013
 * @brief   Customized std::map container with allocator
 * @copyright Cinekine
 */

#ifndef CINEK_MAP_HPP
#define CINEK_MAP_HPP

#include "cinek/allocator.hpp"

#include <unordered_map>

namespace cinek {

//  Std types using the overview allocator.
//
/** An allocator for string memory. */
template<typename Key, typename Value>
    using unordered_map = std::unordered_map<Key, Value,
                                             std::hash<Key>, std::equal_to<Key>,
                                             std_allocator<std::pair<const Key, Value>>>;

} /* cinekine */

#endif
