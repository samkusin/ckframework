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
 * @date    7/25/15
 * @brief   ECS Framework Entity Group Collection
 * @copyright Cinekine
 */

#ifndef CINEK_ENTITY_GROUPMAP_HPP
#define CINEK_ENTITY_GROUPMAP_HPP

#include "entitygroup.hpp"
#include "entitydatatable.hpp"
#include "vectordatarowset.hpp"

#include "cinek/map.hpp"

namespace cinek {

//  The EntityGroupMap is a container for EntityGroup objects defined by a set
//  of Roles.
//
//  This object preallocates all memory required for the entirety of the Map,
//  which means all EntityGroup objects as well.
//
class EntityGroupMap
{
public:
    using container_type = VectorDataRowset<EntityGroup>;
    using index_type = typename container_type::index_type;
    using id_type = EntityGroupMapId;
    static constexpr index_type npos = container_type::npos;
    
    struct MakeDescriptor
    {
        id_type id;
        EntityGroup::RoleLimits roleLimits;
        uint32_t cnt;
    };
    
    EntityGroupMap(const MakeDescriptor& makeDesc,
                   const Allocator& allocator=Allocator());
    
    
    id_type id() const { return _id; }
    
    const container_type& rowset() const { return _rowset; }
    container_type& rowset() { return _rowset; }
    
    index_type rowIndexFromEntity(Entity eid) const;
    index_type allocateIndexForEntity(Entity eid);
    void removeDataFromEntity(Entity eid);
    
    index_type usedCount() const { return (index_type)_entityToGroupIndex.size(); }
    
private:
    VectorDataRowset<EntityGroup> _rowset;
    unordered_map<Entity, uint32_t> _entityToGroupIndex;
    id_type _id;
};

using EntityGroupTable = component::Table<EntityGroup, EntityGroupMap>;

namespace component
{

template<>
Table<EntityGroup, EntityGroupMap>::Table(EntityGroupMap*);

} /* namespace component */

} /* namespace cinek */

#endif /* defined(CINEK_ENTITY_GROUPMAP_HPP) */
