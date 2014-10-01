/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Cinekine Media
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
 * @file    cinek/value_map.hpp
 * @author  Samir Sinha
 * @date    9/25/2014
 * @brief   Attribute Map (Dictionary name value) definition
 * @copyright Cinekine
 */

#ifndef CINEK_VALUE_MAP_HPP
#define CINEK_VALUE_MAP_HPP

#include "cinek/value.hpp"
#include "cinek/debug.hpp"

namespace cinekine {

    /**
     * @class   ValueMap
     * @brief   A Specialized k/v map where 'value' belongs to a predefined set
     *          of types.
     *
     * @details The ValueMap is designed to mimize heap allocation by placing
     *          map entries into key/value pair groups.  Each group is allocated
     *          once upon ValueMap construction.  Values reside in fixed size
     *          memory blocks.  The size of these blocks is determined at
     *          compile time based on the union (max) size of the of the value's
     *          object types.
     *
     *          Details about how the ValueMap manages memory:
     *
     *          A ValueMap tracks the head and tail 'entry set'.  Each entryset
     *          contains vector-like first, last and limit pointers to k/v
     *          entries.  The 'entry' is a key and *ptr to value* pair.  This
     *          ptr to value points to a memory block containing the value.
     *          ValueMap manages value pointers to expedite its other major
     *          implementation feature sorted entries by key to expedite key
     *          searches.
     *
     *          A set of these entries is guaranteed to reside in contiguous
     *          memory and has a fixed size.
     *
     *          When a set is full and a new entry is added, ValueMap allocates
     *          a new entryset of the same size.  When that entryset is full,
     *          ValueMap allocates another entryset and so on advancing the tail
     *          entryset pointer.
     *
     *          One additional optimization involves a single allocation for
     *          both the entryset object and its values (since the entryset has
     *          fixed size values and a fixed number of values.)  Example below:
     *
     *          ----------------------------------------------------
     *          entryset                    struct
     *          ----------------------------------------------------
     *          entryset.entry              sorted k/value-ptr pairs
     *          ----------------------------------------------------
     *          value stack                 value (buffer)
     *          ----------------------------------------------------
     *
     */
    template<typename Key, typename ValueType>
    class ValueMap
    {
    public:
        typedef Key                 key_type;
        typedef ValueType           value_type;

        /**
         * Creates a ValueMap with an initial value count
         *
         * @param cnt       Initial value count.  This value is also the count
         *                  used to grow the map on an overflow condition.
         * @param allocator The memory allocator used when allocating values.
         */
        ValueMap(uint32_t cnt, const Allocator& allocator=Allocator());
        /**
         * Sets a new or replaces an existing entry in the map.
         *
         * @param key The entry key
         * @param val The entry value
         */
        template<typename T> void set(const Key& key, const T& val);
        /**
         * Returns an entry value from the key.  If the key does not exist,
         * returns the specified default.
         *
         * @param  key        The entry key
         * @param  defaultVal The default to return if an entry doesn't exist
         * @return            The typed value.  For supported value types, see
         *                    @ref value
         */
        template<typename T> T get(const Key& key, const T& defaultVal = T()) const;

        ~ValueMap();

    private:

        using entry = std::pair<key_type, value_type*>;

        struct entryset
        {
            entry* first;           // to first entry in entry stack
            entry* last;            // to the next free entry on the stack
            entry* limit;           // to the end of the entry stack
            value_type* nextValue;  // to the first free value on the stack
            entryset* nextSet;      // to the next set on the allocator heap

            entryset(uint8_t* entryStackHead, uint32_t entryCnt,
                     uint8_t* valueStackHead);
            ~entryset();

            bool full() const { return last == limit; }

            const entry* find(const key_type& k) const;
            entry* find(const key_type& k);
            entry* add(const key_type& k);
        };

        entryset* createSet(uint32_t cnt);

        entryset* _headSet;

    private:
        Allocator _allocator;
    };

    ////////////////////////////////////////////////////////////////////////////

    template<typename Key, typename ValueType>
    ValueMap<Key, ValueType>::entryset::entryset(
            uint8_t* entryStackHead,
            uint32_t entryCnt,
            uint8_t* valueStackHead
    ) :
        first(reinterpret_cast<entry*>(entryStackHead)),
        last(first),
        limit(first + entryCnt),
        nextValue(reinterpret_cast<value_type*>(valueStackHead)),
        nextSet(nullptr)
    {
    }

    template<typename Key, typename ValueType>
    ValueMap<Key,ValueType>::entryset::~entryset()
    {
        while (first != last)
        {
            --last;
            last->second->~value_type();
            last->~entry();
        }
    }

    template<typename Key, typename ValueType>
    auto ValueMap<Key,ValueType>::entryset::find(const key_type& k) const -> const entry*
    {
        entry* e = first;
        while (e != last)
        {
            if (e->first == k)
                return e;
            ++e;
        }
        return nullptr;
    }

    template<typename Key, typename ValueType>
    auto ValueMap<Key,ValueType>::entryset::add(const key_type& k) -> entry*
    {
        if (last == limit)
            return nullptr;
        entry* e = last;
        ++last;
        e->first = k;
        e->second = nextValue++;
        return e;
    }

    template<typename Key, typename ValueType>
    auto ValueMap<Key,ValueType>::entryset::find(const key_type& k) -> entry*
    {
        return const_cast<entry*>(static_cast<const entryset*>(this)->find(k));
    }

    template<typename Key, typename ValueType>
    ValueMap<Key,ValueType>::ValueMap(uint32_t cnt, const Allocator& allocator) :
        _allocator(allocator),
        _headSet(nullptr)
    {
        _headSet = createSet(cnt);
    }

    template<typename Key, typename ValueType>
    ValueMap<Key,ValueType>::~ValueMap()
    {
        //  free all entries, and with each entry, free its value.
        while(_headSet)
        {
            entryset* nextset = _headSet->nextSet;
            _headSet->~entryset();
            _allocator.free(_headSet);
            _headSet = nextset;
        }
    }

    template<typename Key, typename ValueType>
    auto ValueMap<Key,ValueType>::createSet(uint32_t cnt) -> entryset*
    {
         //  allocate a block containing entryset, k/v entry pairs and a stack
        //  of value objects
        uint8_t* memBlock = reinterpret_cast<uint8_t*>(
                                _allocator.alloc(sizeof(entryset) +
                                                 sizeof(entry) * cnt +
                                                 sizeof(value_type) * cnt)
                                );
        if (!memBlock)
            return nullptr;

        uint8_t* firstEntry = memBlock + sizeof(entryset);
        uint8_t* firstValue = firstEntry + sizeof(entry) * cnt;

        return ::new(memBlock) entryset(firstEntry, cnt, firstValue);
    }

    template<typename Key, typename ValueType>
    template<typename T>
    void ValueMap<Key,ValueType>::set(const Key& key, const T& val)
    {
        //  if a key exists, we're good.  if no key exists, find the entryset
        //  where we can add this key
        entryset* set = _headSet;
        entryset* lastSet = nullptr;
        entry* found = nullptr;
        while (set)
        {
            found = set->find(key);
            if (found) break;
            if (!set->full())
                break;
            lastSet = set;
            set = set->nextSet;
        }
        if (!found)
        {
            if (!set)
            {
                set = createSet((uint32_t)(lastSet->limit - lastSet->first));
                lastSet->nextSet = set;
            }
            if (!set)
            {
                CK_LOG_ERROR("ValueMap", "Set allocation failure");
                return;
            }
            found = set->add(key);
        }
        found->second->set(val);
    }

    template<typename Key, typename ValueType>
    template<typename T>
    T ValueMap<Key,ValueType>::get(const Key& key, const T& defaultVal) const
    {
        entryset* set = _headSet;
        while (set)
        {
            entry* found = set->find(key);
            if (found)
                return found->second->template get<T>();
            set = set->nextSet;
        }
        return defaultVal;
    }

}   /* namespace cinekine */

#endif
