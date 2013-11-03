/**
 * \file    rendermodel/debug.hpp
 *
 * Internal debugging utilities for the Rendermodel SDK.
 *
 * \note    Created by Samir Sinha on 7/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_DEBUG_HPP
#define CINEK_RENDER_DEBUG_HPP

#include "cinek/core/ckdebug.h"

/*****************************************************************************/

/** @cond */

#ifndef RENDER_LOG_SOURCE
#define RENDER_LOG_SOURCE "render" 
#endif

#define RENDER_LOG_TRACE(...) CK_LOG_TRACE(RENDER_LOG_SOURCE, __VA_ARGS__)
#define RENDER_LOG_INFO(...) CK_LOG_INFO(RENDER_LOG_SOURCE, __VA_ARGS__)
#define RENDER_LOG_WARN(...) CK_LOG_WARN(RENDER_LOG_SOURCE, __VA_ARGS__)
#define RENDER_LOG_ERROR(...) CK_LOG_ERROR(RENDER_LOG_SOURCE, __VA_ARGS__)

/** @endcond */
/*****************************************************************************/

#endif
