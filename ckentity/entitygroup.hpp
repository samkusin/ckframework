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
 * @date    7/23/15
 * @brief   ECS Framework Entity Group Container
 * @copyright Cinekine
 */
 
#ifndef CINEK_ENTITY_GROUP_HPP
#define CINEK_ENTITY_GROUP_HPP

#include "entity.h"

#include "cinek/vector.hpp"

#include <array>

namespace cinek {

//  The EntityGroup is a convenience container organizing entities into roles
//  and slots.   This is an alternative organization model to the Transform's
//  compoennt hierarchy model.
//
//  Several examples of EntityGroups are the Party (in an RPG), a Sports Team,
//  or a ship's crew:
//      Captain
//      Medic
//      Engineer
//      Scientist
//      Tactical
//      Etc...
//
//  Or a player's Loadout in a combat game:
//      Hands           : Primary, Secondary
//      Armor           : Head, Torso, Legs, Arms
//      Accessories     : Left Hand, Right Hand, Belt)
//

class EntityGroup
{
public:
    using Role = uint16_t;
    using RoleLimits = std::array<uint16_t, 32>;
    using Slot = uint16_t;
    
    static constexpr Role kInvalidRole = (Role)(-1);
    static constexpr Slot kInvalidSlot = (Slot)(-1);
    
    //  Roles are synonymous for indices to optimize for lookup within vectors
    //  So define role constants with this in mind (i.e. zero indexed)
    //  The sum of all role limits calculated equals the group limit.
    //  Attempting to add a new entity to a role with no available slots will
    //  fail.
    
    EntityGroup() = default;
    //  Maximum of 32 roles for optimization and filtering purposes
    EntityGroup(const RoleLimits& roleLimits,
                const Allocator& allocator=Allocator());
    
    //  clears all slots (Roles remain)
    void reset();
   
    //  returns entities with roles that match the role mask.  this method
    //  iterates through the playerlist and is O(n).  The function callback must
    //  have the prototype:
    //      void fn(Entity, Role, Slot)
    template<typename Fn> void forEach(Fn fn, uint32_t mask=0xffffffff) const;

    //  retrieve entities by role
    //
    //  returns a pair indicating entity index and entity count.  returns (0,0)
    //  if not a valid role.   always check the count component (pair::second)
    //  for a valid returned range.
    //
    std::pair<uint32_t, uint32_t> entityIndexAndCountByRole(Role role) const; // O(1)
    Entity firstEntityByRole(Role role) const;      // O(1)
    Entity entityByIndex(int32_t index) const;      // O(1)
    Role roleByEntityIndex(int32_t index) const;    // O(N)
    uint32_t roleEntityCount(Role role) const;      // O(1)
    uint32_t roleEntityLimit(Role role) const;      // O(1)
    Entity entityWithRoleAndSlot(Role role, Slot slot) const; //  O(1)
    std::pair<Role, Slot> findEntityRoleAndSlot(Entity entity) const; // O(N)
    
    //  modify the list
    bool addEntityToRole(Entity entity, Role role);         // O(1)
    bool addEntityToRoleAsHead(Entity entity, Role role);   // O(N)
    void removeEntityFromRole(Entity entity, Role role);    // O(N)
    void clearEntityAtRoleAndSlot(Role role, Slot slot);  // O(1)
    void setEntityAtRoleAndSlot(Entity entity, Role role, Slot slot); // O(1)
    
    
private:
    vector<uint32_t> _roleRanges;       // Index::Limit combination (hi16/lo16)
    vector<Entity> _entityByRoleTable;  // ordered vector of entities by role
    
    static inline std::pair<uint32_t, uint32_t> splitIntoIndexAndCount(uint32_t idxcnt) {
        return std::make_pair((idxcnt >> 16), (idxcnt & 0xffff));
    }
    static inline uint16_t indexFromIndexCount(uint32_t idxcnt) {
        return (uint16_t)(idxcnt >> 16);
    }
    static inline uint16_t countFromIndexCount(uint32_t idxcnt) {
        return (uint16_t)(idxcnt & 0xffff);
    }
    static inline uint32_t mergeIntoIndexCount(std::pair<uint16_t, uint16_t> idxcnt) {
        return ((uint32_t)idxcnt.first << 16) | idxcnt.second;
    }
};

template<typename Fn>
void EntityGroup::forEach(Fn fn, uint32_t mask) const
{
    for (auto roleIdx = 0; roleIdx < _roleRanges.size(); ++roleIdx)
    {
        if ((mask & (1<<roleIdx)))
        {
            uint32_t roleIdxCnt = _roleRanges[roleIdx];
            auto itStart = _entityByRoleTable.begin() + indexFromIndexCount(roleIdxCnt);
            auto itEnd = itStart + countFromIndexCount(roleIdxCnt);
            for (auto it = itStart; it != itEnd; ++it)
            {
                auto entity = *it;
                if (entity)
                {
                    fn(entity, (Role)roleIdx, (Slot)(it - itStart));
                }
            }
        }
    }
}

} /* namespace cinek */


#endif /* defined(CINEK_ENTITY_GROUP_HPP) */
