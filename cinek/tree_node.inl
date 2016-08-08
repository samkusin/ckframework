//  tree_node.inl
//
//  CKWorld Simulation Framework
//
//  Created by Samir Sinha on 6/10/16.
//  Copyright (c) 2016 Cinekine. All rights reserved.
//

namespace cinek {

template<typename T>
void TreeNode<T>::move(TreeNode& other)
{
    _data = std::move(other._data);
    _parent = nullptr;
    _next = nullptr;
    _prev = nullptr;
    _firstChild = nullptr;
}

template<typename T>
TreeNode<T>* TreeNode<T>::appendChild
(
    ThisType* child
)
{
    return insertChild(child, nullptr);
}

template<typename T>
TreeNode<T>* TreeNode<T>::insertChild
(
    ThisType* child,
    ThisType* sibling
)
{
    if (!child || child->_parent)
        return nullptr;
    if (sibling && sibling->_parent != this)
        return nullptr;
    
    if (!_firstChild) {
        _firstChild = child;
        _firstChild->_prev = _firstChild;
        _firstChild->_next = nullptr;
    }
    else {
        ThisType* prevChild = sibling ? sibling->_prev : _firstChild->_prev;
        ThisType* nextChild = sibling;
        if (prevChild) {
            prevChild->_next = child;
        }
        if (nextChild) {
            nextChild->_prev = child;
        }
        else {
            _firstChild->_prev = child;
        }
        child->_prev = prevChild;
        child->_next = nextChild;
        if (child->_next == _firstChild) {
            _firstChild = child;
        }
    }
    child->_parent = this;
    return child;
}

template<typename T>
TreeNode<T>* TreeNode<T>::removeChild
(
    ThisType* child
)
{
    if (!child || child->_parent != this)
        return nullptr;
    
    ThisType* nextChild = child->_next;
    ThisType* prevChild = child->_prev;
    
    if (nextChild) {
        nextChild->_prev = prevChild;
    }
    if (prevChild) {
        prevChild->_next = nextChild;
    }
    if (_firstChild == child) {
        _firstChild = nextChild;
    }
    child->_next = child->_prev = nullptr;
    child->_parent = nullptr;
    return child;
}

}
