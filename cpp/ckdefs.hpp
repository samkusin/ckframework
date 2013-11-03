/**
 * @file    ckdefs.hpp
 *
 * @note    Created by Samir Sinha on 6/22/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */


#ifndef CINEK_DEFINES_HPP
#define CINEK_DEFINES_HPP

#include "cinek/core/cktypes.h"

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

namespace cinekine
{
    /** Get the minimum value between v1 and v2 */
    template<typename T> T min(T v1, T v2) {
        return v1 < v2 ? v1 : v2;
    }
    /** Get the maximum value between v1 and v2. */
    template<typename T> T max(T v1, T v2) {
        return v1 > v2 ? v1 : v2;
    }
}

#endif
