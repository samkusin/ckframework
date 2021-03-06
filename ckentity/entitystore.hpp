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
 * @file    ckentity/entitydatatable
 * @author  Samir Sinha
 * @date    5/18/15
 * @brief   ECS Framework Entity Database
 * @copyright Cinekine
 */

#ifndef CINEK_ENTITY_STORE_HPP
#define CINEK_ENTITY_STORE_HPP


#include "entity.h"

#include "cinek/vector.hpp"
#include "cinek/map.hpp"
#include "cinek/allocator.hpp"

#include <random>
#include <functional>

namespace cinek {

struct EntityDiagnostics
{
    uint32_t entityCount;
    uint32_t entityLimit;
};

// using techniques from:
// http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html
//
class EntityStore
{
    CK_CLASS_NON_COPYABLE(EntityStore);
    
public:
    EntityStore();
    
    struct InitParams
    {
        EntityIndexType numEntities;
    };
    
    EntityStore(const InitParams& params, const Allocator& allocator=Allocator());
    
    EntityStore(EntityStore&& other);
    EntityStore& operator=(EntityStore&& other);
    
    ~EntityStore();
    
    uint32_t capacity() const { return (uint32_t)_iterations.capacity(); }
    
    Entity create(EntityContextType context=0);
    void destroy(Entity eid);
    
    bool valid(Entity eid) const;
    void gc();
    
    void diagnostics(EntityDiagnostics& diagnostics);
    
private:
    //  objects indexed by the offset value of the EntityId
    vector<EntityIterationType> _iterations;
    vector<EntityIndexType> _freed;
    EntityIterationType _entityIdIteration;
    EntityIndexType _entityCount;
};

} /* namespace cinek */

#endif /* defined(CINEK_ENTITY_STORE_HPP) */
