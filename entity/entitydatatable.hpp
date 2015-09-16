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
 * @date    6/19/15
 * @brief   ECS Framework Component Templates
 * @copyright Cinekine
 */

#ifndef CINEK_ENTITY_COMPONENTTABLE_HPP
#define CINEK_ENTITY_COMPONENTTABLE_HPP

#include "entity.h"
#include "datarowset.hpp"

#include "cinek/map.hpp"
#include "cinek/debug.hpp"

namespace cinek {

class EntityDataTable
{
public:
    using container_type = component::DataRowset;
    using index_type = typename container_type::index_type;
    using id_type = ComponentId;
    static constexpr index_type npos = container_type::npos;
    
    EntityDataTable(const component::MakeDescriptor& desc, const Allocator& allocator);
    
    id_type id() const { return _descriptor.id; }
    const char* name() const { return _descriptor.name; }
    
    const container_type& rowset() const { return _rowset; }
    container_type& rowset() { return _rowset; }
    
    index_type rowIndexFromEntity(Entity eid) const;
    index_type allocateIndexForEntity(Entity eid);
    void removeDataFromEntity(Entity eid);
    
    index_type usedCount() const { return (index_type)_entityToRow.size(); }
    
private:
    component::Descriptor _descriptor;
    void* _context;
    unordered_map<Entity, index_type> _entityToRow;
    container_type _rowset;
};

inline auto EntityDataTable::rowIndexFromEntity(Entity eid) const -> index_type
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
        return npos;
    
    return indexIt->second;
}

namespace component
{

template<typename Component, typename Container=EntityDataTable>
class Table
{
public:
    using value_type = Component;
    using container_type = Container;
    using index_type = typename Container::index_type;
    
    Table() : _table(nullptr) {}
    Table(container_type* dataTable);
    Table(const Table& other) : _table(other._table) {}
    Table& operator=(const Table& other) { _table = other._table; return *this; }
    
    Table(Table&& other) : _table(other._table) { other._table = nullptr; }
    Table& operator=(Table&& other) { _table = other._table; other._table = nullptr; return *this; }
    
    operator bool() const { return _table != nullptr; }
    
    value_type* addDataToEntity(Entity eid);
    void removeDataFromEntity(Entity eid);

    const value_type* dataForEntity(Entity eid) const;
    value_type* dataForEntity(Entity eid);
    
    const container_type* dataTable() const { return _table; }
    container_type* dataTable() { return _table; }
    
    std::pair<Entity, value_type*> dataAtIndex(index_type index);
    std::pair<Entity, const value_type*> dataAtIndex(index_type index) const;
    
    index_type count() const { return _table->usedCount(); }
    index_type limit() const { return _table->rowset.capacity(); }
    
    //  iterate through all objects in the DB with the specified component
    //  Function signature should be:
    //      void fn(Entity eid, Component& component);
    template<typename Fn> Fn forEach(Fn fn);
    
private:
    container_type* _table;
};

template<typename Component, typename Container>
Table<Component, Container>::Table(container_type* dataTable) :
    _table(dataTable)
{
    CK_ASSERT(_table && _table->id() == Component::kComponentId);
}

template<typename Component, typename Container>
auto Table<Component, Container>::addDataToEntity(Entity eid) -> value_type*
{
    if (eid==0)
        return nullptr;
    
    auto index = _table->allocateIndexForEntity(eid);
    return reinterpret_cast<value_type*>(_table->rowset()[index]);
}

template<typename Component, typename Container>
void Table<Component, Container>::removeDataFromEntity(Entity eid)
{
    _table->removeDataFromEntity(eid);
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataForEntity(Entity eid) const -> const value_type*
{
    auto index = _table->rowIndexFromEntity(eid);
    if (index == Container::npos)
        return nullptr;
        
    return reinterpret_cast<const value_type*>(_table->rowset()[index]);
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataForEntity(Entity eid) -> value_type*
{
    return const_cast<value_type*>(
        static_cast<const Table*>(this)->dataForEntity(eid)
    );
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataAtIndex(index_type index)
    -> std::pair<Entity, value_type*>
{
    auto ret = static_cast<const Table*>(this)->dataAtIndex(index);
    return std::make_pair(ret.first, const_cast<value_type*>(ret.second));
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataAtIndex(index_type index) const
    -> std::pair<Entity, const value_type*>
{
    auto& rowset = _table->rowset();
    return std::make_pair(rowset.entityAt(index), rowset.template at<value_type>(index));
}

//  iterate through all objects in the DB with the specified component
//  Function signature should be:
//      void fn(EntityId eid, Component& component);
template<typename Component, typename Container>
template<typename Fn>
Fn Table<Component, Container>::forEach(Fn fn)
{
    auto& rowset = _table->rowset();
    auto idx = rowset.firstIndex();
    while (idx != Container::npos)
    {
        fn(rowset.entityAt(idx), *rowset.template at<value_type>(idx));
        idx = rowset.nextIndex(idx);
    }
    return fn;
}

} /* namespace component */

} /* namespace cinek */

#endif
