/**
 * @file    ckvector.hpp
 *
 * @note    Created by Samir Sinha on 1/6/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */


#ifndef CINEK_VECTOR_HPP
#define CINEK_VECTOR_HPP

#include "ckdefs.hpp"
#include "ckalloc.hpp"

#include <vector>

namespace cinekine {

//  Std types using the overview allocator.
//
/** An allocator for string memory. */
template<typename T>
    using vector = std::vector<T, std_allocator<T>>;

} /* cinekine */

#endif
