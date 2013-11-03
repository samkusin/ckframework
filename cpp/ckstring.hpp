/**
 * @file    ckstring.hpp
 *
 * @note    Created by Samir Sinha on 1/6/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */


#ifndef CINEK_STRING_HPP
#define CINEK_STRING_HPP

#include "ckdefs.hpp"
#include "ckalloc.hpp"

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
