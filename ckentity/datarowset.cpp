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

#include "datarowset.hpp"

#include "cinek/debug.h"

//  for memmove
#include <cstring>

namespace cinek {

namespace component
{
    constexpr DataRowset::index_type DataRowset::npos;

    DataRowset::DataRowset
    (
        const Descriptor& desc,
        uint32_t rowCount,
        const Allocator& allocator
    ) :
        _allocator(allocator),
        _header(desc),
        _rowstart(nullptr),
        _rowend(nullptr),
        _rowlimit(nullptr),
        _freeCnt(0),
        _entityToRow(allocator)
    {
        CK_ASSERT(rowCount != npos);

        _header.recordSize += sizeof(Entity); // flags included in record
        _rowstart = (uint8_t*)_allocator.alloc(_header.recordSize * rowCount);
        _rowend = _rowstart;
        _rowlimit = _rowstart + _header.recordSize*rowCount;
    }

    DataRowset::~DataRowset()
    {
        _allocator.free(_rowstart);
        _rowstart = _rowend = _rowlimit = nullptr;
    }

    DataRowset::DataRowset(DataRowset&& other) :
        _allocator(std::move(other._allocator)),
        _header(std::move(other._header)),
        _rowstart(other._rowstart),
        _rowend(other._rowend),
        _rowlimit(other._rowlimit),
        _freeCnt(other._freeCnt),
        _entityToRow(std::move(other._entityToRow))
    {
        other._header.recordSize = 0;
        other._header.id = kEmpty;
        other._rowstart = nullptr;
        other._rowend = nullptr;
        other._rowlimit = nullptr;
        other._freeCnt = 0;
    }

    DataRowset& DataRowset::operator=(DataRowset&& other)
    {
        _allocator = std::move(other._allocator);
        _header = std::move(other._header);
        _rowstart = other._rowstart;
        _rowend = other._rowend;
        _rowlimit = other._rowlimit;
        _freeCnt = other._freeCnt;
        _entityToRow = std::move(other._entityToRow);

        other._header.recordSize = 0;
        other._header.id = kEmpty;
        other._rowstart = nullptr;
        other._rowend = nullptr;
        other._rowlimit = nullptr;
        other._freeCnt = 0;

        return *this;
    }

    Entity* DataRowset::rowAt(index_type index)
    {
        return const_cast<Entity*>(static_cast<const DataRowset*>(this)->rowAt(index));
    }

    const Entity* DataRowset::rowAt(index_type index) const
    {
        return reinterpret_cast<const Entity*>(_rowstart + index*_header.recordSize);
    }

    auto DataRowset::allocate(Entity eid) -> index_type
    {
        auto indexIt = _entityToRow.find(eid);
        if (indexIt != _entityToRow.end())
            return indexIt->second;

        index_type idx = npos;
        if (_rowend != _rowlimit)
        {
            idx = (index_type)((_rowend - _rowstart) / _header.recordSize);
            _rowend += _header.recordSize;
        }
        if (idx != npos)
        {
            Entity* p = rowAt(idx);
            *p = eid;
            memset(p+1, 0, _header.recordSize-sizeof(uint32_t));
            _header.initCb(eid, at(idx));
            indexIt = _entityToRow.insert(std::make_pair(eid, idx)).first;
        }

        return idx;
    }

    void DataRowset::free(Entity eid)
    {
        auto indexIt = _entityToRow.find(eid);
        if (indexIt == _entityToRow.end())
            return;

        _entityToRow.erase(indexIt);
        
        freeRowWithIndexInternal(indexIt->second);
    }
    
    void DataRowset::freeWithIndex(index_type rowIndex)
    {
        CK_ASSERT_RETURN(rowIndex < rowCount());
        Entity* p = rowAt(rowIndex);
        free(p[0]);
    }
    
    void DataRowset::freeRowWithIndexInternal(index_type rowIndex)
    {
        CK_ASSERT_RETURN(rowIndex < rowCount());
        
        index_type lastIndex = rowCount()-1;
        if (rowIndex != lastIndex)
        {
            //  fill our freed row with data from the end of the
            //  rowset, shrinking our array by 1 row
            memcpy(rowAt(rowIndex), rowAt(lastIndex), _header.recordSize);
            Entity* p = rowAt(rowIndex);
            auto indexIt = _entityToRow.find(p[0]);
            CK_ASSERT(indexIt != _entityToRow.end());
            indexIt->second = rowIndex;
        }
        _rowend -= _header.recordSize;
    }
    
    /*
    void DataRowset::compress()
    {
        //  run through our vector, compressing it until no empty rows remain
        uint8_t* row = _rowstart;
        uint8_t* rowLeft = nullptr;
        uint8_t* rowRight = nullptr;
        uint8_t* origRowEnd = _rowend;

        while (row < _rowend)
        {
            Entity* p = reinterpret_cast<Entity*>(row);
            if (!p[0] && !rowLeft)
            {
                rowLeft = row;
            }
            else if (p[0] && rowLeft)
            {
                //  shift contents to the left, shrinking our rowset
                //  our left/right range detection allows us to *slightly*
                //  optimize our compression if there are multiple zero row
                //  ranges within the set.
                rowRight = row;
                memmove(rowLeft, rowRight, _rowend - rowRight);
                _rowend -= (rowRight - rowLeft);
                row = rowLeft;
                p = reinterpret_cast<Entity*>(row);
                rowLeft = nullptr;
                rowRight = nullptr;
            }
            if (p[0] && origRowEnd != _rowend)
            {
                //  we need to update the entity's row index
                //  checking if we've compressed the rowset, which tells us we
                //  need to change successive entity row indices.
                //  this is done as an optimization so we don't have to perform
                //  map lookups for every entity in the rowset (though the worse
                //  case - first rows were empty - will force lookups for every
                //  entity in our entity->row map.)
                _entityToRow[p[0]] = row - _rowstart;
            }
            row += _header.recordSize;
        }

        if (rowLeft && !rowRight)
        {
            //  edge case - trailing empty rows - no changes to existing
            //  entity row indices.
            _rowend = rowLeft;
        }
    }
    */

    uint32_t DataRowset::rowCount() const
    {
        return static_cast<uint32_t>((_rowend - _rowstart) / _header.recordSize);
    }

    uint32_t DataRowset::capacity() const
    {
        return static_cast<uint32_t>((_rowlimit - _rowstart) / _header.recordSize);
    }

    uint8_t* DataRowset::operator[](index_type index)
    {
        return at(index);
    }

    const uint8_t* DataRowset::operator[](index_type index) const
    {
        return at(index);
    }

    uint8_t* DataRowset::at(index_type index)
    {
        return const_cast<uint8_t*>(static_cast<const DataRowset*>(this)->at(index));
    }

    const uint8_t* DataRowset::at(index_type index) const
    {
        CK_ASSERT(index < rowCount() && index != npos);

        const Entity* p = rowAt(index);
        return p[0] ? reinterpret_cast<const uint8_t*>(p+1) : nullptr;
    }

    Entity DataRowset::entityAt(index_type index) const
    {
        return { *rowAt(index) };
    }
    
    bool DataRowset::hasEntity(Entity eid) const
    {
        return _entityToRow.find(eid) != _entityToRow.end();
    }

    auto DataRowset::firstIndex(index_type idx) const -> index_type
    {
        //  return the first valid component data row (might not be the first
        //  row, for instance.)
        if (idx != npos)
        {
            if (_rowstart != _rowend)
            {
                const Entity* row = rowAt(0);
                if (row[0])
                    return 0;

                return nextIndex(idx);
            }
        }
        return npos;
    }

    auto DataRowset::nextIndex(index_type idx) const -> index_type
    {
        if (idx != npos)
        {
            index_type sz = rowCount();
            //  indices are unsigned - so take care of the sz==0 edge case.
            //  this will allow us to iterate idx without overflow concerns
            if (!sz)
                return npos;

            while (idx < sz-1)
            {
                ++idx;
                const Entity* row = rowAt(idx);
                if (row[0])
                    return idx;
            }
        }
        return npos;
    }

    auto DataRowset::prevIndex(index_type idx) const -> index_type
    {
        if (idx != npos)
        {
            //  handle edge cases
            index_type sz = rowCount();
            if (!idx || idx >= sz)
                return npos;

            while (idx > 0)
            {
                --idx;
                const Entity* row = rowAt(idx);
                if (row[0])
                    return idx;
            }
        }
        return npos;
    }

} /* namespace component */

} /* namespace cinek */
