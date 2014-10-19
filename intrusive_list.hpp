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
 * @file    cinek/intrusive_list.hpp
 * @author  Samir Sinha
 * @date    9/17/2014
 * @brief   STL-like intrusive_list implementation
 * @copyright Cinekine
 */


#ifndef CINEK_INTRUSIVE_LIST_HPP
#define CINEK_INTRUSIVE_LIST_HPP

#include "cinek/types.hpp"
#include "cinek/debug.hpp"

#include <iterator>

namespace cinek {

template<typename Object, typename IntrusiveList>
class intrusive_list_iterator : std::iterator<std::bidirectional_iterator_tag, Object>
{
public:
    typedef intrusive_list_iterator<Object, IntrusiveList> this_type;
    typedef Object*                 pointer;
    typedef Object&                 reference;

    //  copy-con, assign, destruct
    intrusive_list_iterator() : _ptr() {}
    intrusive_list_iterator(pointer ptr) : _ptr(ptr) {}
    intrusive_list_iterator(const this_type& it) : _ptr(it._ptr) {}
    this_type& operator=(const this_type& it) {
        _ptr = it._ptr;
        return *this;
    }
    //  comparable equivalence
    bool operator==(const this_type& rhs) const { return _ptr == rhs._ptr; }
    bool operator!=(const this_type& rhs) const { return _ptr != rhs._ptr; }
    //  dereferencable
    reference operator*() { return *_ptr; }
    pointer operator->() { return _ptr; }
    //  incrementable
    this_type& operator++() { _ptr = IntrusiveList::nextNode(_ptr); return *this; }
    this_type operator++(int) {
        this_type tmp = *this;
        ++(*this);
        return tmp;
    }
    //  decrementable
    this_type& operator--() { _ptr = IntrusiveList::prevNode(_ptr); return *this; }
    this_type operator--(int) {
        this_type tmp = *this;
        --(*this);
        return tmp;
    }

    pointer ptr() { return _ptr; }
    const pointer ptr() const { return _ptr; }

private:
    pointer _ptr;
};

template<typename Object, typename IntrusiveList>
class intrusive_list_const_iterator : std::iterator<std::bidirectional_iterator_tag, Object>
{
public:
    typedef intrusive_list_const_iterator<Object, IntrusiveList> this_type;
    typedef intrusive_list_iterator<Object, IntrusiveList> non_const_this_type;
    typedef const Object*               pointer;
    typedef const Object&               reference;

    //  copy-con, assign, destruct
    intrusive_list_const_iterator() : _ptr() {}
    intrusive_list_const_iterator(pointer ptr) : _ptr(ptr) {}
    intrusive_list_const_iterator(const this_type& it) : _ptr(it._ptr) {}
    intrusive_list_const_iterator(const non_const_this_type& it): _ptr((pointer)it.ptr()) {}
    this_type& operator=(const this_type& it) {
        _ptr = it._ptr;
        return *this;
    }
    //  comparable equivalence
    bool operator==(const this_type& rhs) const { return _ptr == rhs._ptr; }
    bool operator!=(const this_type& rhs) const { return _ptr != rhs._ptr; }
    //  dereferencable
    reference operator*() const { return *_ptr; }
    pointer operator->() const { return _ptr; }
    //  incrementable
    this_type& operator++() { _ptr = IntrusiveList::nextNode(_ptr); return *this; }
    this_type operator++(int) {
        this_type tmp = *this;
        ++(*this);
        return tmp;
    }
    //  decrementable
    this_type& operator--() { _ptr = IntrusiveList::prevNode(_ptr); return *this; }
    this_type operator--(int) {
        this_type tmp = *this;
        --(*this);
        return tmp;
    }

    pointer ptr() const {
        return _ptr;
    }

    typename non_const_this_type::pointer ptr()  {
        return const_cast<typename non_const_this_type::pointer>(_ptr);
    }

private:
    pointer _ptr;
};


//  Implements the following std concepts (iterator)
//      ForwardIterator
//      OutputIterator
//
template<class Object>
class intrusive_list
{
    CK_CLASS_NON_COPYABLE(intrusive_list);

public:
    typedef intrusive_list<Object>                  this_type;
    typedef Object                                  value_type;
    typedef Object*                                 pointer;
    typedef const Object*                           const_pointer;
    typedef Object&                                 reference;
    typedef const Object&                           const_reference;
    typedef intrusive_list_iterator<Object, this_type>         iterator;
    typedef intrusive_list_const_iterator<Object, this_type>   const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
    typedef size_t                                  size_type;

    intrusive_list() : _anchor(), _size(0) {
        setNodeNextNode(&_anchor, &_anchor);
        setNodePrevNode(&_anchor, &_anchor);
    }
    intrusive_list(intrusive_list&& other) : _anchor(std::move(other._anchor)), _size(other._size) {
        other._size = 0;
    }
    void clear() noexcept {
        //  need to clear out all nodes' references to prev and next
        //  since we're implementing an intrusive list
        while (!empty())
            pop_front();
    }
    size_type size() const noexcept { return _size; }
    size_type max_size() const noexcept { return (size_type)(-1); }
    bool empty() const noexcept { return _size == 0; }

    iterator begin() { return (iterator(nextNode(&_anchor))); }
    const_iterator begin() const { return const_iterator(nextNode(&_anchor)); }
    const_iterator cbegin() const { return const_iterator(nextNode(&_anchor)); }
    reverse_iterator rbegin() { return (reverse_iterator(end())); }
    const_reverse_iterator rbegin() const { return (const_reverse_iterator(cend())); }
    const_reverse_iterator crbegin() { return (const_reverse_iterator(cend())); }

    iterator end() { return iterator(&_anchor); }
    const_iterator end() const { return const_iterator(&_anchor); }
    const_iterator cend() const { return const_iterator(&_anchor); }
    reverse_iterator rend() { return (reverse_iterator(begin())); }
    const_reverse_iterator rend() const { return (const_reverse_iterator(cbegin())); }
    const_reverse_iterator crend() { return (const_reverse_iterator(cbegin())); }

    reference front() { return (*begin()); }
    const_reference front() const { return (*begin()); }
    reference back() { return (*(--end())); }
    const_reference back() const { return (*(--end())); }

    void push_front(pointer object) {
        insert(begin(), object);
    }
    void push_back(pointer object) {
        insert(end(), object);
    }
    void insert(const_iterator pos, pointer object) {
        CK_ASSERT(!nextNode(object) && !prevNode(object));
        pointer next = pos.ptr();
        pointer prev = prevNode(next);

        //  fixup surrounding nodes
        setNodePrevNode(next, object);
        setNodeNextNode(prev, object);
        //  fixup our inserted object
        setNodeNextNode(object, next);
        setNodePrevNode(object, prev);
        ++_size;
    }
    void pop_front() {
        erase(begin());
    }
    void pop_back() {
        erase(--end());
    }
    iterator erase(const_iterator pos) {
        pointer object = pos.ptr();
        pointer prev = prevNode(object);
        pointer next = nextNode(object);
        //  fixup our erased object's surrounding nodes
        setNodeNextNode(prev, next);
        setNodePrevNode(next, prev);
        //  fixup our erased object
        setNodeNextNode(object, nullptr);
        setNodePrevNode(object, nullptr);
        --_size;
        return iterator(next);
    }
    iterator erase(const_iterator first, const_iterator last) {
        while (first != last)
            first = erase(first);
        return iterator(last);
    }
    void remove(pointer object)
    {
        for(iterator pos = begin(); pos != end(); ++pos) {
            if (pos.ptr() != object) {
                erase(pos);
                return;
            }
        }
    }
    void swap(this_type& other) {
        std::swap(_size, other._size);

        pointer tmpPrev = prevNode(&_anchor);
        pointer tmpNext = nextNode(&_anchor);

        setNodePrevNode(&_anchor, prevNode(&other._anchor));
        setNodeNextNode(&_anchor, nextNode(&other._anchor));
        setNodePrevNode(&other._anchor, tmpPrev);
        setNodeNextNode(&other._anchor, tmpNext);
    }

    //  internal (undocumented)
    static pointer prevNode(const_pointer node) {
        return node->__prevListNode;
    }
    static pointer nextNode(const_pointer node) {
        return node->__nextListNode;
    }
    static void setNodeNextNode(pointer node, pointer nextNode) {
        node->__nextListNode = nextNode;
    }
    static void setNodePrevNode(pointer node, pointer prevNode) {
        node->__prevListNode = prevNode;
    }

private:
    value_type _anchor; // anchor.next = head of list
    size_type _size;
};

}   /* namespace cinek */


#endif
