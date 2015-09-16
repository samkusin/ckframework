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
 * @file    cinek/entity/entitydatatable
 * @author  Samir Sinha
 * @date    5/18/15
 * @brief   ECS Framework Entity Database
 * @copyright Cinekine
 */

#ifndef CINEK_ENTITY_STORE_HPP
#define CINEK_ENTITY_STORE_HPP

#include "entity.h"
#include "entitydatatable.hpp"
#include "entitygroupmap.hpp"

#include "cinek/vector.hpp"
#include "cinek/map.hpp"
#include "cinek/allocator.hpp"

namespace cinek {

struct EntityDiagnostics
{
    struct component
    {
        const char* name;
        uint32_t allocated;
        uint32_t limit;
    };
    struct group
    {
        EntityGroupMapId id;
        uint32_t allocated;
        uint32_t limit;
    };
    
    vector<component> components;
    vector<group> groups;
    
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
    
    EntityStore(EntityIndexType numEntities,
        const vector<component::MakeDescriptor>& components,
        const vector<EntityGroupMap::MakeDescriptor>& entityGroups,
        const Allocator& allocator=Allocator());
    
    EntityStore(EntityStore&& other);
    EntityStore& operator=(EntityStore&& other);
    
    uint32_t capacity() const { return (uint32_t)_iterations.capacity(); }
    
    Entity create(EntityContextType context=0);
    void destroy(Entity eid);
    
    bool valid(Entity eid) const;
    
    //  garbage collects unused components - run once per frame
    void gc();

    template<typename Component> component::Table<Component> table() const;
    EntityGroupTable entityGroupTable(EntityGroupMapId id) const;
    
    void diagnostics(EntityDiagnostics& diagnostics);
    
private:
    //  objects indexed by the offset value of the EntityId
    vector<EntityIterationType> _iterations;
    vector<EntityIndexType> _freed;
    EntityIterationType _entityIdIteration;
    EntityIndexType _entityCount;
    unordered_map<ComponentId, EntityDataTable> _components;
    unordered_map<EntityGroupMapId, EntityGroupMap> _entityGroups;
};

template<typename Component>
component::Table<Component> EntityStore::table() const
{
    auto componentIt = _components.find(Component::kComponentId);
    if (componentIt == _components.end())
        return nullptr;
    const EntityDataTable& table = componentIt->second;
    return component::Table<Component>(const_cast<EntityDataTable*>(&table));
}

} /* namespace cinek */

#endif /* defined(CINEK_ENTITY_STORE_HPP) */
