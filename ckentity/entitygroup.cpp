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

#include "entitygroup.hpp"
#include "cinek/debug.hpp"

namespace cinek {

EntityGroup::EntityGroup
(
    const std::array<uint16_t, 32>& roleLimits,
    const Allocator& allocator
) :
    _roleRanges(allocator),
    _entityByRoleTable(allocator)
{
    vector<uint32_t> ranges(allocator);
    ranges.reserve(roleLimits.size());
    uint32_t index = 0;
    for (auto& limit : roleLimits)
    {
        ranges.push_back((index << 16));
        index += limit;
        CK_ASSERT_RETURN(index < UINT16_MAX);
    }
    
    _roleRanges = std::move(ranges);
}

void EntityGroup::reset()
{
    for (auto& range : _roleRanges)
    {
        range = range & 0xffff0000;     // clears count, keeps index
    }
}
   
std::pair<uint32_t, uint32_t> EntityGroup::entityIndexAndCountByRole(Role role) const
{
    CK_ASSERT_RETURN_VALUE(role < _roleRanges.size(), std::make_pair(0,0));
    
    uint32_t idxcnt = _roleRanges[role];
    return splitIntoIndexAndCount(idxcnt);
}

Entity EntityGroup::firstEntityByRole(Role role) const
{
    auto range = entityIndexAndCountByRole(role);
    return range.second > 0 ? entityByIndex(range.first) : 0;
}

Entity EntityGroup::entityByIndex(int32_t index) const
{
    CK_ASSERT_RETURN_VALUE(index < _entityByRoleTable.size(), 0);
    return _entityByRoleTable[index];
}

EntityGroup::Role EntityGroup::roleByEntityIndex(int32_t index) const
{
    for (auto it = _roleRanges.begin(); it != _roleRanges.end(); ++it)
    {
        auto idxcnt = splitIntoIndexAndCount(*it);
        if (index >= idxcnt.first && index < (idxcnt.first+idxcnt.second))
            return (Role)(it - _roleRanges.begin());
    }
    
    return kInvalidRole;
}

uint32_t EntityGroup::roleEntityCount(Role role) const
{
    CK_ASSERT_RETURN_VALUE(role < _roleRanges.size(), 0);
    return countFromIndexCount(_roleRanges[role]);
}

uint32_t EntityGroup::roleEntityLimit(Role role) const
{
    CK_ASSERT_RETURN_VALUE(role < _roleRanges.size(), 0);
    
    uint32_t maxIdx;
    uint16_t idx = indexFromIndexCount(_roleRanges[role]);
    if (role < _roleRanges.size()-1)
    {
        maxIdx = indexFromIndexCount(_roleRanges[role+1]);
    }
    else
    {
        maxIdx = (uint32_t)_entityByRoleTable.size();
    }
    
    return maxIdx - idx;
}

Entity EntityGroup::entityWithRoleAndSlot(Role role, Slot slot) const
{
    CK_ASSERT_RETURN_VALUE(role < _roleRanges.size(), 0);
    auto slotCount = countFromIndexCount(_roleRanges[role]);
    if (slot >= slotCount)
        return 0;
    auto baseIndex = indexFromIndexCount(_roleRanges[role]);
    return _entityByRoleTable[baseIndex + slot];
}

auto EntityGroup::findEntityRoleAndSlot(Entity entity) const -> std::pair<Role, Slot>
{
    std::pair<Role, Slot> roleAndSlot(0,0);
    
    if (entity)
    {
        auto it = _roleRanges.begin();
        auto head = indexFromIndexCount(*it);
        for (unsigned i = 0; i < _entityByRoleTable.size(); ++i)
        {
            if (i >= head)
            {
                ++it;
                head = indexFromIndexCount(*it);
            }
            if (_entityByRoleTable[i] == entity)
            {
                roleAndSlot.first = it - _roleRanges.begin();
                roleAndSlot.second = i - head;
                break;
            }
        }
    }
    
    return roleAndSlot;
}

//  modify the list
bool EntityGroup::addEntityToRole(Entity entity, Role role)
{
    auto idxcnt = entityIndexAndCountByRole(role);
    if (idxcnt.second >= roleEntityLimit(role))
        return false;
    
    _entityByRoleTable[idxcnt.first + idxcnt.second] = entity;
    ++idxcnt.second;
    _roleRanges[role] = mergeIntoIndexCount(idxcnt);
    return true;
}

bool EntityGroup::addEntityToRoleAsHead(Entity entity, Role role)
{
    CK_ASSERT_RETURN_VALUE(role < _roleRanges.size(), false);
    auto idxcnt = entityIndexAndCountByRole(role);
    if (idxcnt.second >= roleEntityLimit(role))
        return false;

    for (auto idx = idxcnt.second + idxcnt.first; idx > idxcnt.first; --idx)
        _entityByRoleTable[idx] = _entityByRoleTable[idx-1];
    _entityByRoleTable[idxcnt.first] = entity;
    
    ++idxcnt.second;
    _roleRanges[role] = mergeIntoIndexCount(idxcnt);
    return true;
}

void EntityGroup::removeEntityFromRole(Entity entity, Role role)
{
    CK_ASSERT_RETURN(role < _roleRanges.size());
    auto idxcnt = entityIndexAndCountByRole(role);
    if (!idxcnt.second)
        return;
    int idx = idxcnt.first;
    int end = idx+idxcnt.second;

    for (; idx < end; ++idx)
    {
        if (entity == _entityByRoleTable[idx])
            break;
    }
    //  compress the entity list for this role
    for (; idx < end-1; ++idx)
    {
        _entityByRoleTable[idx] = _entityByRoleTable[idx+1];
    }
    --idxcnt.second;
    _roleRanges[role] = mergeIntoIndexCount(idxcnt);
}

void EntityGroup::clearEntityAtRoleAndSlot(Role role, Slot slot)
{
    CK_ASSERT_RETURN(role < _roleRanges.size());
    auto idxcnt = entityIndexAndCountByRole(role);
    if (slot >= idxcnt.second)
        return;
    _entityByRoleTable[idxcnt.first+slot] = 0;
}

void EntityGroup::setEntityAtRoleAndSlot(Entity entity, Role role, Slot slot)
{
    CK_ASSERT_RETURN(role < _roleRanges.size());
    auto idxcnt = entityIndexAndCountByRole(role);
    auto nextidx = role+1 < _roleRanges.size() ? indexFromIndexCount(_roleRanges[role+1]) :
        _entityByRoleTable.size();
    if (slot >= idxcnt.second)
    {
        //   expand entity array with null values.
        CK_ASSERT_RETURN(slot < nextidx - idxcnt.first);
        for (unsigned i = idxcnt.second; i < slot; ++i)
            _entityByRoleTable[idxcnt.first + i] = 0;
        idxcnt.second = slot+1;
    }
    _entityByRoleTable[idxcnt.first + slot] = entity;
}
    
} /* namespace cinek */