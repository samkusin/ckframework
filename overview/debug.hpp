/**
 * \file    overview/debug.hpp
 *
 * Internal debugging utilities for the Overview SDK.
 *
 * \note    Created by Samir Sinha on 2/1/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_DEBUG_HPP
#define CINEK_OVERVIEW_DEBUG_HPP

#include <cinek/framework/debug.hpp>

/*****************************************************************************/

/** @cond */

#if CK_DEBUG_LOGGING

#ifndef OV_LOG_SOURCE
#define OV_LOG_SOURCE "overview" 
#endif

#define OV_LOG_TRACE(...) cinek_debug_log(kCinekLogLevel_Trace, OV_LOG_SOURCE, __VA_ARGS__)
#define OV_LOG_INFO(...) cinek_debug_log(kCinekLogLevel_Info, OV_LOG_SOURCE, __VA_ARGS__)
#define OV_LOG_WARN(...) cinek_debug_log(kCinekLogLevel_Warn, OV_LOG_SOURCE, __VA_ARGS__)
#define OV_LOG_ERROR(...) cinek_debug_log(kCinekLogLevel_Error, OV_LOG_SOURCE,  __VA_ARGS__)

#endif /* CK_DEBUG_LOGGING */

#ifndef OV_LOG_TRACE
#define OV_LOG_TRACE(...)
#endif
#ifndef OV_LOG_INFO
#define OV_LOG_INFO(...)
#endif
#ifndef OV_LOG_WARN
#define OV_LOG_WARN(...)
#endif
#ifndef OV_LOG_ERROR
#define OV_LOG_ERROR(...)
#endif

/** @endcond */

/*****************************************************************************/
/**
 * \def OV_ASSERT(_cond_)
 * An assertion macro used by the Overview SDK, invoking a user supplied debug break
 * handler.
 */
/**
 * \def OV_ASSERT_MSG(_cond_)
 * An assertion macro displaying additional text, used by the Overview SDK, invoking a 
 * user supplied debug break handler.
 */

#if CK_DEBUG_ASSERT

#define OV_ASSERT(_cond_) do { \
 if (!(_cond_)) { \
    cinek_debug_break(); \
    OV_LOG_ERROR("(%s) failed", #_cond_); \
 } \
} while(0) 

#define OV_ASSERT_MSG(_cond_, _msg_) do { \
 if (!(_cond_)) { \
    cinek_debug_break(); \
    OV_LOG_ERROR("(%s) failed.  %s\n", #_cond_, _msg_); \
 } \
} while(0) 

#endif /* CK_DEBUG_ASSERT */

#ifndef OV_ASSERT
#define OV_ASSERT(_cond_)
#endif

#ifndef OV_ASSERT_MSG
#define OV_ASSERT_MSG(_cond_, _msg_)
#endif

/*****************************************************************************/

#endif /* CINEK_OVERVIEW_DEBUG_HPP */
