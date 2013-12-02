/**
 * @file    ckmap.hpp
 *
 * @note    Created by Samir Sinha on 1/6/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */


#ifndef CINEK_MAP_HPP
#define CINEK_MAP_HPP

#include "ckdefs.hpp"
#include "ckalloc.hpp"

#include <unordered_map>

namespace cinekine {

//  Std types using the overview allocator.
//
/** An allocator for string memory. */
template<typename Key, typename Value>
    using unordered_map = std::unordered_map<Key, Value,
                                             std::hash<Key>, std::equal_to<Key>,
                                             std_allocator<std::pair<const Key, Value>>>;

} /* cinekine */

#endif
