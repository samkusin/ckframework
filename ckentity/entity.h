/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Cinekine Media
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
 * @file    ckentity/entity.h
 * @author  Samir Sinha
 * @date    9/9/2015
 * @brief   ECS Framework Types Header
 * @copyright Cinekine
 */

#ifndef CINEK_ENTITY_H
#define CINEK_ENTITY_H

#include <stdint.h>

#if CINEK_ENTITY_TYPE_32
typedef uint32_t CKEntity;
typedef uint16_t CKEntityIteration;
typedef uint8_t CKEntityContext;
typedef uint32_t CKEntityIndex;

constexpr uint32_t kCKEntityIndexMask = 0x000fffff;
constexpr uint32_t kCKEntityIndexBits = 20;
constexpr uint32_t kCKEntityIterationMask = 0x0ff00000;
constexpr uint32_t kCKEntityIterationBits = 8;
constexpr uint32_t kCKEntityContextMask = 0xf0000000;
constexpr uint32_t kCKEntityContextBits = 4;

#else
typedef uint64_t CKEntity;
typedef uint16_t CKEntityIteration;
typedef uint16_t CKEntityContext;
typedef uint32_t CKEntityIndex;

constexpr uint64_t kCKEntityIndexMask = 0x00000000ffffffff;
constexpr uint64_t kCKEntityIndexBits = 32;
constexpr uint64_t kCKEntityIterationMask = 0x0000ffff00000000;
constexpr uint64_t kCKEntityIterationBits = 16;
constexpr uint64_t kCKEntityContextMask = 0xffff000000000000;
constexpr uint64_t kCKEntityContextBits = 16;

#endif

enum
{
    kCKEntityIterationShift = kCKEntityIndexBits,
    kCKEntityContextShift = kCKEntityIndexBits + kCKEntityIterationBits
};

#define cinek_make_entity(_iter_,_ctx_,_idx_) \
    ((((CKEntity)(_ctx_)<<kCKEntityContextShift) & kCKEntityContextMask) | \
     (((CKEntity)(_iter_)<<kCKEntityIterationShift) & kCKEntityIterationMask) | \
     ((_idx_) & kCKEntityIndexMask))

#define cinek_entity_index(_eid_) \
    ((CKEntityIndex)((_eid_) & kCKEntityIndexMask))

#define cinek_entity_context(_eid_) \
    ((CKEntityContext)(((_eid_) & kCKEntityContextMask)>>kCKEntityContextShift))

#define cinek_entity_iteration(_eid_) \
    ((CKEntityIteration)(((_eid_) & kCKEntityIterationMask)>>kCKEntityIterationShift))

#ifdef __cplusplus

namespace cinek {

    //  Forward decls --------------------------------------------------------------

    class EntityStore;
    struct EntityDiagnostics;

    using Entity = CKEntity;
    using EntityContextType = CKEntityContext;
    using EntityIndexType = CKEntityIndex;
    using EntityIterationType = CKEntityIteration;

} /* namespace cinek */

#endif  /* __cplusplus */


#endif  /* CINEK_ENTITY_H */
