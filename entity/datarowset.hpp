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
 * @file    cinek/entity/datarowset.hpp
 * @author  Samir Sinha
 * @date    5/18/15
 * @brief   ECS Framework Component Data lookup container
 * @copyright Cinekine
 */

#ifndef CINEK_ENTITY_DATAROWSET_HPP
#define CINEK_ENTITY_DATAROWSET_HPP

#include "entity.h"

#include "cinek/allocator.hpp"
#include "cinek/types.hpp"

#include <iterator>

namespace cinek {

namespace component
{
    class DataRowset
    {
        CK_CLASS_NON_COPYABLE(DataRowset);
        
    public:
        using index_type = ComponentRowIndex;
        
        static const index_type npos = kNullComponentRow;

        DataRowset(const Descriptor& desc,
            uint32_t rowCount,
            const Allocator& allocator=Allocator());
        ~DataRowset();
        
        DataRowset(DataRowset&& other);
        DataRowset& operator=(DataRowset&& other);
        
        index_type allocate(Entity eid);
        void free(index_type index);
        
        uint32_t size() const;
        uint32_t capacity() const;
        
        uint8_t* operator[](index_type index);
        const uint8_t* operator[](index_type index) const;
        
        template<typename Component> Component* at(index_type index);
        template<typename Component> const Component* at(index_type index) const;
        
        uint8_t* at(index_type index);
        const uint8_t* at(index_type index) const;
        
        Entity entityAt(index_type index) const;
        
        index_type firstIndex(index_type idx=0) const;
        index_type nextIndex(index_type) const;
        index_type prevIndex(index_type) const;
        
    private:
        Allocator _allocator;
        Descriptor _header;
        
        // rows
        //  each row contains a EntityId header + data[header.recordSize]
        //  the entity id will be zero if the row is not used
        uint8_t* _rowstart;
        uint8_t* _rowend;
        uint8_t* _rowlimit;
        // free list
        uint32_t* _freestart;
        uint32_t* _freeend;
        uint32_t* _freelimit;
        
        Entity* rowAt(index_type index);
        const Entity* rowAt(index_type index) const;
    };

    template<typename Component>
    Component* DataRowset::at(index_type index)
    {
        if (_header.id != Component::kComponentId)
            return nullptr;
        
        return reinterpret_cast<Component*>(at(index));
    }

    template<typename Component>
    const Component* DataRowset::at(index_type index) const
    {
        if (_header.id != Component::kComponentId)
            return nullptr;
        
        return reinterpret_cast<const Component*>(at(index));
    }
    
} /* namespace component */

} /* namespace cinek */

#endif /* defined(Overview_Entity_DataRowset_hpp) */
