/**
 * \file    cktypes.h
 * \brief   Contains basic types used by all modules.
 * \note    (c) 2012 Cinekine Media
 */

#ifndef CINEK_BASE_H
#define CINEK_BASE_H

#include "ckdefs.h"

#if CK_COMPILER_HAS_STDINT
  #ifdef __cplusplus
    #include <cstdint>
  #else
    #include <stdint.h>
  #endif
#endif

/** Time value type.  Made into a typedef to allow for 32 or 64-bit times. */
typedef uint32_t cinek_time;

/* CINEK_TYPES_H */
#endif
