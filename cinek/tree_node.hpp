/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Cinekine Media
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
 * @file    cinek/tree_node.hpp
 * @author  Samir Sinha
 * @date    6/10/2016
 * @brief   Defines a node in a tree hierarchy
 * @copyright Cinekine
 */

#ifndef CINEK_TREE_NODE_HPP
#define CINEK_TREE_NODE_HPP

#include <utility>

namespace cinek {

    template<typename T>
    class TreeNode
    {
    public:
        using ThisType = TreeNode<T>;
        
        TreeNode() :
            _data(T()),
            _parent(nullptr),
            _next(nullptr),
            _prev(nullptr),
            _firstChild(nullptr)
        {
        }
        TreeNode(T data) :
            _data(std::move(data)),
            _parent(nullptr),
            _next(nullptr),
            _prev(nullptr),
            _firstChild(nullptr)
        {
        }
        
        TreeNode(TreeNode& other) { move(other); }
        TreeNode& operator=(TreeNode&& other) { move(other); return *this; }
        
        const T& data() const { return _data; }
        T& data() { return _data; }
        const ThisType* parent() const { return _parent; }
        const ThisType* next() const { return _next; }
        const ThisType* prev() const { return _prev; }
        const ThisType* firstChild() const { return _firstChild; }
        
        ThisType* appendChild(ThisType* child);
        ThisType* insertChild(ThisType* child, ThisType* sibling);
        ThisType* removeChild(ThisType* child);

    private:
        void move(TreeNode& other);
        
        T _data;
        
        //  child is the head of the parent's child list
        //  next, prev comprise the sibling list
        ThisType* _parent;
        ThisType* _next;
        ThisType* _prev;
        ThisType* _firstChild;
    };

}

#include "tree_node.inl"

#endif