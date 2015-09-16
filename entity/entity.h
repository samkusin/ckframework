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
 * @file    cinek/entity/entity.h
 * @author  Samir Sinha
 * @date    9/9/2015
 * @brief   ECS Framework Types Header
 * @copyright Cinekine
 */

#ifndef CINEK_ENTITY_H
#define CINEK_ENTITY_H

#include <stdint.h>

#if CINEK_ENTITY_TYPE_64
typedef uint64_t CKEntity;
typedef uint16_t CKEntityIteration;
typedef uint16_t CKEntityContext;
typedef uint32_t CKEntityIndex;

enum
{
    kCKEntityIndexMask      = 0x00000000ffffffff,
    kCKEntityIndexBits      = 32,
    kCKEntityIterationMask  = 0x0000ffff00000000,
    kCKEntityIterationBits  = 16,
    kCKEntityContextMask    = 0xffff000000000000,
    kCKEntityContextBits    = 16
};
#else
typedef uint32_t CKEntity;
typedef uint16_t CKEntityIteration;
typedef uint8_t CKEntityContext;
typedef uint32_t CKEntityIndex;

enum
{
    kCKEntityIndexMask      = 0x000fffff,
    kCKEntityIndexBits      = 20,
    kCKEntityIterationMask  = 0x0ff00000,
    kCKEntityIterationBits  = 8,
    kCKEntityContextMask    = 0xf0000000,
    kCKEntityContextBits    = 4
};
#endif

enum
{
    kCKEntityIterationShift = kCKEntityIndexBits,
    kCKEntityContextShift = kCKEntityIndexBits + kCKEntityIterationBits
};

#define cinek_make_entity(_iter_,_ctx_,_idx_) \
    ((((Entity)(_ctx_)<<kCKEntityContextShift) & kCKEntityContextMask) | \
     (((Entity)(_iter_)<<kCKEntityIterationShift) & kCKEntityIterationMask) | \
     ((_idx_) & kCKEntityIndexMask))

#define cinek_entity_index(_eid_) \
    ((CKEntityIndex)((_eid_) & kCKEntityIndexMask))

#define cinek_entity_context(_eid_) \
    ((CKEntityContext)((_eid_) & kCKEntityContextMask))

#define cinek_entity_iteration(_eid_) \
    ((CKEntityIteration)((_eid_) & kCKEntityIterationMask))

typedef uint32_t CKComponentId;
typedef uint32_t CKComponentRowIndex;
typedef uint32_t CKEntityGroupMapId;

enum
{
    kCKEntityNullComponentRow = UINT32_MAX
};


#ifdef __cplusplus

#include <cstddef>
#include <functional>

namespace cinek {

    //  Forward decls --------------------------------------------------------------

    class EntityStore;
    class EntityGroup;
    class EntityGroupMap;
    class EntityDataTable;
    struct EntityDiagnostics;

    //  Components -----------------------------------------------------------------

    namespace component
    {
        template<typename _Component, typename _Container> class Table;
    }
    
    using Entity = CKEntity;
    using EntityContextType = CKEntityContext;
    using EntityIndexType = CKEntityIndex;
    using EntityIterationType = CKEntityIteration;
    using ComponentId = CKComponentId;
    using ComponentRowIndex = CKComponentRowIndex;
    using EntityGroupMapId = CKEntityGroupMapId;
    constexpr ComponentRowIndex kNullComponentRow = kCKEntityNullComponentRow;

    constexpr ComponentId MakeComponentId(uint16_t category, uint16_t id)
    {
        return (((ComponentId)category) << 16) | id;
    }

    namespace component
    {
        struct Descriptor
        {
            ComponentId id;
            size_t recordSize;
            const char* name;
            void (*initCb)(::cinek::Entity, void*);
        };
        
        struct MakeDescriptor
        {
            Descriptor desc;
            uint32_t cnt;
        };

        enum
        {
            kEmpty              = MakeComponentId(0xffff,        0xffff)
        };
    }

} /* namespace cinek */


#define COMPONENT_DEFINITION(_type_) \
    using this_type = _type_; \
    static const ::cinek::ComponentId kComponentId; \
    static const ::cinek::component::Descriptor kComponentType; \
    static ::cinek::component::MakeDescriptor makeDescriptor( uint32_t cnt); \
    void initialize(::cinek::Entity entity);

#define COMPONENT_TYPE_IMPL(_type_, _id_, _init_body_) \
    void _type_::initialize(::cinek::Entity entity) { \
        _init_body_ \
    } \
    ::cinek::component::MakeDescriptor _type_::makeDescriptor ( uint32_t cnt) { \
        ::cinek::component::MakeDescriptor md; \
        md.desc = kComponentType; \
        md.cnt = cnt; \
        return md; \
    } \
    void _type_ ## initialize_hook(::cinek::Entity e, void* pdata) { \
        _type_::this_type* data = reinterpret_cast<_type_::this_type*>(pdata); \
        data->initialize(e); \
    } \
    const ::cinek::ComponentId _type_::kComponentId = _id_; \
    const ::cinek::component::Descriptor _type_::kComponentType =  \
        { _type_::kComponentId, sizeof(_type_), #_type_,  _type_ ## initialize_hook };

#endif  /* __cplusplus */


#endif  /* CINEK_ENTITY_H */
