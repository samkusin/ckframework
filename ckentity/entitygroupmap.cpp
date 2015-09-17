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

#include "entitygroupmap.hpp"

#include "cinek/debug.hpp"

namespace cinek {

EntityGroupMap::EntityGroupMap
(
    const MakeDescriptor& makeDesc,
    const Allocator& allocator
) :
    _rowset(makeDesc.cnt, makeDesc.roleLimits, allocator),
    _entityToGroupIndex(allocator),
    _id(makeDesc.id)
{
}

auto EntityGroupMap::allocateIndexForEntity(Entity entity) -> index_type
{
    CK_ASSERT_RETURN_VALUE(entity != 0, npos);

    auto idx = _rowset.allocate(entity);
    CK_ASSERT_RETURN_VALUE(idx != npos, npos);
    if (idx == npos)
        return npos;
    
    auto res = _entityToGroupIndex.emplace(entity, idx);
    if (res.first == _entityToGroupIndex.end())
        return npos;
    CK_ASSERT(res.second);  // two rows belonging to same entity?!

    return idx;
}
    

void EntityGroupMap::removeDataFromEntity(Entity entity)
{
    if (!entity)
        return;
    
    auto it = _entityToGroupIndex.find(entity);
    if (it == _entityToGroupIndex.end())
        return;
 
    uint32_t idx = it->second;
    _entityToGroupIndex.erase(it);
    _rowset.free(idx);
}
    
auto EntityGroupMap::rowIndexFromEntity(Entity entity) const -> index_type
{
    if (!entity)
        return npos;
    
    auto it = _entityToGroupIndex.find(entity);
    if (it == _entityToGroupIndex.end())
        return npos;

    uint32_t idx = it->second;
    CK_ASSERT(idx < _rowset.size());
    return idx;
}

namespace component
{

//template<> class Table<EntityGroup, EntityGroupMap>;

//  specialization of constructor - no need to verify ComponentId of dataTable
//  matches the Component (which is the EntityGroup object - a dynamic type)
template<>
Table<EntityGroup, EntityGroupMap>::Table(EntityGroupMap* dataTable) :
    _table(dataTable)
{

}


} /* namespace component */

} /* namespace cinek */