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
 */

#include "entitystore.hpp"

#include "cinek/debug.h"

namespace cinek {

EntityStore::EntityStore() :
    _entityIdIteration(0),
    _entityCount(0)
{
}

EntityStore::EntityStore
(
    const InitParams& params,
    const Allocator& allocator
) :
    _iterations(allocator),
    _freed(allocator),
    _entityIdIteration(0),
    _entityCount(0),
    _components(allocator),
    _gcrandom(params.randomSeed)
{
    _iterations.reserve(params.numEntities);
    _freed.reserve(params.numEntities);
    
    for (auto& component : params.components)
    {
        EntityDataTable rowset(component, allocator);
        _components.emplace(component.desc.id, std::move(rowset));
    }
    for (auto& group : params.entityGroups)
    {
        EntityGroupMap rowset(group, allocator);
        _entityGroups.emplace(group.id, std::move(rowset));
    }
}


EntityStore::EntityStore(EntityStore&& other) :
    _iterations(std::move(other._iterations)),
    _freed(std::move(other._freed)),
    _entityIdIteration(other._entityIdIteration),
    _entityCount(other._entityCount),
    _components(std::move(other._components)),
    _gcrandom(std::move(other._gcrandom))
{
    other._entityIdIteration = 0;
    other._entityCount = 0;
}

EntityStore& EntityStore::operator=(EntityStore&& other)
{
    _iterations = std::move(other._iterations);
    _freed = std::move(other._freed);
    _entityIdIteration = other._entityIdIteration;
    _entityCount = other._entityCount;
    _components = std::move(other._components);
    _gcrandom = std::move(other._gcrandom);
    
    other._entityIdIteration = 0;
    other._entityCount = 0;
    
    return *this;
}

EntityStore::~EntityStore()
{
}
    
Entity EntityStore::create(EntityContextType context)
{
    EntityIndexType index;
    
    if (!_freed.empty())
    {
        index = _freed.back();
        _freed.pop_back();
    }
    else
    {
        index = (EntityIndexType)_iterations.size();
        _iterations.push_back(1);
    }
    
    Entity eid = cinek_make_entity(_iterations[index], context, index);
    ++_entityCount;
    return eid;
}

void EntityStore::destroy(Entity eid)
{
    if (eid==0)
        return;

    auto index = cinek_entity_index(eid);
    ++_iterations[index];
    if (!_iterations[index])
        _iterations[index] = 1;

    --_entityCount;

    _freed.push_back(index);
}

bool EntityStore::valid(Entity eid) const
{
    return (_iterations[cinek_entity_index(eid)] == cinek_entity_iteration(eid));
}

void EntityStore::gc(const EntityComponentDestroyFn& fn)
{
    // Using BitSquid's method of lazy component destruction.
    // At some point we'll have to differentiate between destroy immediate vs
    // delayed for components that acquire system resources.
    //
    // http://bitsquid.blogspot.com/2014/09/building-data-oriented-entity-system.html
    //
    for (auto& component : _components)
    {
        auto& table = component.second;
        
        uint32_t consecutiveLivingRows = 0;
        
        while (table.usedCount() > 0 && consecutiveLivingRows < 4)
        {
            component::DataRowset::index_type idx = _gcrandom() % table.usedCount();
            Entity eid = table.rowset().entityAt(idx);
            if (valid(eid))
            {
                ++consecutiveLivingRows;
                continue;
            }
            consecutiveLivingRows = 0;
            fn(table, idx);
            table.rowset().freeWithIndex(idx);
        }
    }
}

EntityGroupTable EntityStore::entityGroupTable(EntityGroupMapId id) const
{
    auto it = _entityGroups.find(id);
    if (it == _entityGroups.end())
        return EntityGroupTable();
    
    const EntityGroupMap& entityGroupMap = it->second;
    return EntityGroupTable(const_cast<EntityGroupMap*>(&entityGroupMap));
}

const EntityDataTable* EntityStore::entityTable(ComponentId compId) const
{
    auto componentIt = _components.find(compId);
    if (componentIt == _components.end())
        return nullptr;
    const EntityDataTable& table = componentIt->second;
    return &table;
}

EntityDataTable* EntityStore::entityTable(ComponentId compId)
{
    return const_cast<EntityDataTable*>(
        static_cast<const EntityStore*>(this)->entityTable(compId)
    );
}

void EntityStore::diagnostics(EntityDiagnostics& diagnostics)
{
    diagnostics.components.reserve(_components.size());
    diagnostics.components.clear();
    
    for (auto& component : _components)
    {
        diagnostics.components.emplace_back();
        
        auto& diag = diagnostics.components.back();
        diag.name = component.second.name();
        diag.allocated = component.second.usedCount();
        diag.limit = component.second.rowset().capacity();
    }
    
    diagnostics.groups.reserve(_entityGroups.size());
    diagnostics.groups.clear();
    
    for (auto& group : _entityGroups)
    {
        diagnostics.groups.emplace_back();
    
        auto& diag = diagnostics.groups.back();
        diag.id = group.first;
        diag.allocated = group.second.usedCount();
        diag.limit = group.second.rowset().capacity();
    }
    
    diagnostics.groups.reserve(_entityGroups.size());
    
    diagnostics.entityCount = _entityCount;
    diagnostics.entityLimit = (uint32_t)_iterations.capacity();
}

} /* namespace cinek */