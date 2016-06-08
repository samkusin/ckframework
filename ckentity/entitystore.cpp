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

#include <cinek/debug.h>
#include <random>

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
    _entityCount(0)
{
    _iterations.reserve(params.numEntities);
    _freed.reserve(params.numEntities);
}


EntityStore::EntityStore(EntityStore&& other) :
    _iterations(std::move(other._iterations)),
    _freed(std::move(other._freed)),
    _entityIdIteration(other._entityIdIteration),
    _entityCount(other._entityCount)
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

void EntityStore::gc()
{
}

void EntityStore::diagnostics(EntityDiagnostics& diagnostics)
{
    diagnostics.entityCount = _entityCount;
    diagnostics.entityLimit = (uint32_t)_iterations.capacity();
}

} /* namespace cinek */