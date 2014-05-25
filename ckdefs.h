/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2012 Cinekine Media
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
 * @file    cinek/ckdefs.h
 * @author  Samir Sinha
 * @date    11/1/2013
 * @brief   C and C++ common macros and definitions
 * @copyright Cinekine
 */

#ifndef CINEK_DEFS_H
#define CINEK_DEFS_H

#ifdef CINEK_HAS_CKOPTS_H
#include "cinek/ckopts.h"
#endif

#include <stddef.h>

/**
 *  \name Compiler Defines
 *  Defines properties related to the build compiler system.
 */
/**@{*/
/*
 *  Detect Compiler.
 */
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
 * \def CK_COMPILER_HAS_STDINT
 * Set to 1 if available, else set to 0 if not available.
 */
#ifndef CK_COMPILER_HAS_STDINT
  #ifdef _MSC_VER
    #if _MSC_VER < 1600
      #define CK_COMPILER_HAS_STDINT 0
    #else
      #define CK_COMPILER_HAS_STDINT 1
    #endif
  #else
  /* [TODO] - explicitly call out GNU/LLVM, etc. */
    #define CK_COMPILER_HAS_STDINT 1
  #endif

/* CK_COMPILER_HAS_STDINT */
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

/**
 *  \name Platform Defines
 *  Defines the target development platform
 */
/**@{*/
#ifdef CK_DOXYGEN_RUNNING_PASS
/**
 * \def CK_TARGET_OSX
 * Compiling for OS X
 */
#define CK_TARGET_OSX
/**
 * \def CK_TARGET_LINUX
 * Compiling for Linux
 */
#define CK_TARGET_LINUX
/**
 * \def CK_TARGET_WINDOWS
 * Compiling for a Windows desktop OS.
 */
#define CK_TARGET_WINDOWS
/**
 * \def CK_TARGET_IOS
 * Compiling for the iOS Platform.
 */
#define CK_TARGET_IOS
/**
 * \def CK_TARGET_IOS_SIMULATOR
 * Compiling specifically for the iPhone Simulator.
 */
#define CK_TARGET_IOS_SIMULATOR

#endif /* CK_DOXYGEN_RUNNING_PASS */
/**@}*/

#ifdef __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_OS_IPHONE
    #define CK_TARGET_IOS
  #elif TARGET_IPHONE_SIMULATOR
    #define CK_TARGET_IOS_SIMULATOR
  #elif TARGET_OS_MAC
    #define CK_TARGET_OSX
  #endif
#elif __linux__
  #define CK_TARGET_LINUX
#elif _WIN32
  #define CK_TARGET_WINDOWS
#else
  #error "Detected an unsupported platform."
#endif

/**
 *  \def CK_ALIGN_SIZE(_val, _align_)
 *  Returns a size value based on _val_, and aligned using the specified _align_ value.
 */
#define CK_ALIGN_SIZE(_val_, _align_) ( ((_val_)+(_align_)-1) & ~((_align_)-1) )
/**
 * \def CK_ALIGN_PTR(_ptr_, _align_)
 * Returns an adjusted pointer based on _ptr_, aligned by _align_ bytes.
 */
#define CK_ALIGN_PTR(_ptr_, _align_) ( ((uintptr_t)(_ptr_)+(_align_)-1) & ~((_align_)-1) )

/**
 * \def CK_ARCH_ALIGN_BYTES
 * The architecture specific alignment value (in bytes.)
 */
#define CK_ARCH_ALIGN_BYTES (4)
/**
 *  \def CK_ALIGN_SIZE_TO_ARCH(_val_)
 *  Invokes CK_ALIGN_SIZE with the platform's predefined alignment value.
 */
#define CK_ALIGN_SIZE_TO_ARCH(_val_) CK_ALIGN_SIZE( (_val_), CK_ARCH_ALIGN_BYTES )
/**
 *  \def CK_ALIGN_PTR_TO_ARCH(_ptr_)
 *  Invokes CK_ALIGN_PTR with the platform's predefined alignment value.
 */
#define CK_ALIGN_PTR_TO_ARCH(_ptr_) CK_ALIGN_PTR( (_ptr_), CK_ARCH_ALIGN_BYTES )


/**
 * \def DO_PRAGMA(_x_)
 * Adds a compiler pragma.
 */
#define DO_PRAGMA(_x_) _Pragma (#_x_)

/**@{*/
/**
 * \def TODO(_x_)
 * Adds a TODO message to the compile process.
 */
#define TODO(_x_) DO_PRAGMA( message("TODO: " #_x_ ) )
/**@}*/

#ifndef MAX_PATH
/**@{*/
/**
 * \def MAX_PATH
 * The suggested maximum length of a pathname.
 */
#define MAX_PATH 256
/**@}*/
#endif

/* CINEK_DEFS_H */
#endif
