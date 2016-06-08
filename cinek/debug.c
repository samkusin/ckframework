/**
 * \file    debug.cpp
 *
 *
 * \note    Created by Samir Sinha on 2/1/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "debug.h"
#include "ckdefs.h"

#if defined(CK_TARGET_OSX) || defined(CK_TARGET_LINUX)
    #include <signal.h>
    #define CK_DEBUG_BREAK_RAISE_SIGNAL 1
#else
    #if CK_COMPILER_MSVC
    #define CK_DEBUG_BREAK_MSVC         1
    #endif
#endif

#if CK_DEBUG_LOGGING

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

static void stdlog(void* , const char*, const char* , va_list args);
static void stdlogerr(void* , const char*, const char* , va_list args);
static void stdlogflush(void*);

/*  global logging provider. */
struct
{
    cinek_log_callbacks cbs;
    void* context;
}
g_cinek_logProvider =
{
    {
        {
            &stdlog,
            &stdlog,
            &stdlog,
            &stdlog,
            &stdlogerr
        },
        &stdlogflush
    },
    NULL
};


/*  default logger. */
static void stdlog(void* context, const char* sourceId, const char* fmt, va_list args)
{
    (void)context;
    (void)sourceId;
    vfprintf(stdout, fmt, args);
    fputc('\n', stdout);
}

static void stdlogerr(void* context, const char* sourceId, const char* fmt, va_list args)
{
    (void)context;
    (void)sourceId;
    vfprintf(stderr, fmt, args);
    fputc('\n', stdout);
}

static void stdlogflush(void* context)
{
    (void)context;
    fflush(stdout);
    fflush(stderr);
}

/*  logger functions */
void cinek_debug_log(cinek_log_level level, const char* sourceId, const char* fmt, ... )
{
    va_list args;
    va_start(args, fmt);
    (*g_cinek_logProvider.cbs.logger[level])(g_cinek_logProvider.context, sourceId, fmt, args);
    va_end(args);
}

void cinek_debug_log_start(
        const cinek_log_callbacks* callbacks,
        void* context
    )
{
    cinek_debug_log_flush();

    if (callbacks != NULL)
    {
        memcpy(&g_cinek_logProvider.cbs, callbacks, sizeof(g_cinek_logProvider.cbs));
        g_cinek_logProvider.context = context;
    }
    else
    {
        g_cinek_logProvider.cbs.logger[kCinekLogLevel_Trace] = &stdlog;
        g_cinek_logProvider.cbs.logger[kCinekLogLevel_Debug] = &stdlog;
        g_cinek_logProvider.cbs.logger[kCinekLogLevel_Info] = &stdlog;
        g_cinek_logProvider.cbs.logger[kCinekLogLevel_Warn] = &stdlog;
        g_cinek_logProvider.cbs.logger[kCinekLogLevel_Error] = &stdlogerr;
        g_cinek_logProvider.cbs.flush = &stdlogflush;
        g_cinek_logProvider.context = NULL;
    }
}

void cinek_debug_log_flush(void)
{
    if (g_cinek_logProvider.cbs.flush != NULL)
    {
        (*g_cinek_logProvider.cbs.flush)(g_cinek_logProvider.context);
    }
}

#endif


#if CK_DEBUG_ASSERT

#include <assert.h>

void cinek_debug_break(void)
{
#if CK_DEBUG_BREAK_RAISE_SIGNAL
    raise(SIGTRAP);
#elif CK_DEBUG_BREAK_MSVC
    __debugbreak;
#else
    assert(0);
#endif
}

#endif
