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
 
#include "entitydatatable.hpp"

namespace cinek {


EntityDataTable::EntityDataTable
(
    const component::MakeDescriptor& desc,
    const Allocator& allocator
) :
    _descriptor(desc.desc),
    _entityToRow(allocator),
    _rowset(desc.desc, desc.cnt, allocator)
{
}

auto EntityDataTable::allocateIndexForEntity(Entity eid) -> index_type
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
    {
        auto idx = _rowset.allocate(eid);
        _descriptor.initCb(eid, _rowset.at(idx));
        indexIt = _entityToRow.insert(std::make_pair(eid, idx)).first;
    }
    return indexIt->second;
}

void EntityDataTable::removeDataFromEntity
(
    Entity eid
)
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
        return;
    
    auto index = indexIt->second;
    _entityToRow.erase(indexIt);
    _rowset.free(index);
}

} /* namespace cinek */
