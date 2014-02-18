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
 * @file    cinek/framework/defs.hpp
 * @author  Samir Sinha
 * @date    6/22/2013
 * @brief   C++ common macros and definitions 
 * @copyright Cinekine
 */



#ifndef CINEK_DEFINES_HPP
#define CINEK_DEFINES_HPP

#include "cinek/core/ckdefs.h"

#ifdef __cplusplus
/**
 * \def CK_CPP_11_BASIC
 * C++11 support (move semantics, lambda, auto).   Removed initializer_lists
 * support until VS2012 or later fully supports that feature.
 */
#ifndef CK_CPP_11_BASIC
  #if __clang__
    #if __has_feature(cxx_lambdas) && __has_feature(cxx_nullptr) \
        && __has_feature(cxx_auto_type) && __has_feature(cxx_rvalue_references) 
      #define CK_CPP_11_BASIC
    #endif
  #elif __GNUC__ >= 4
    #if __GNUC__ == 4
      #if __GNUC_MINOR__ >= 7
        #define CK_CPP_11_BASIC
      #endif
    #else
      #define CK_CPP_11_BASIC
    #endif
  #elif _MSC_VER >= 1600
    #define CK_CPP_11_BASIC
  #endif
  /* CK_CPP_11_BASIC */
  #endif  
/* __cplusplus */
#else
  #undef CK_CPP_11_BASIC
#endif

/**
 * \def CK_CPP_EXCEPTIONS
 * Define if C++ exception handling is enabled.
 */
#define CK_CPP_EXCEPTIONS 0
/**@}*/

#if CK_CPP_EXCEPTIONS
  #ifdef __cplusplus
    #include <stdexcept>
  #endif
#endif

#ifdef __cplusplus
/** 
 *  \name C++ Macros
 *  Useful macros for C++ development.
 */
/**@{*/ 
/**
 * \def CK_CLASS_NON_COPYABLE(__class_name_)
 * Prevents copy operations for the specified class.  This must be placed 
 * the C++ class definition.
 */
#ifdef CK_CPP_11_BASIC
#define CK_CLASS_NON_COPYABLE(__class_name_) \
  __class_name_(const __class_name_&) = delete; \
  __class_name_& operator=(const __class_name_&) = delete;
#else
  /* must be in a private decl. */
#define CK_CLASS_NON_COPYABLE(__class_name_) \
  __class_name_(const __class_name_&); \
  __class_name_& operator=(const __class_name_&);
#endif
/**@}*/
#endif


#endif
