/**
 * \file    overview/quadtree.hpp
 *
 * Quadtree representation of all props in a map.
 *
 * \note    Created by Samir Sinha on 3/3/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_QUADTREE_HPP
#define CINEK_OVERVIEW_QUADTREE_HPP

#include "kazmath/aabb.h"

#include "cinek/cpp/ckstdalloc.hpp"

#include <vector>

namespace cinekine {
    namespace overview {

class QuadTreeOwner;

/**
 * @class QuadTree quadtree.hpp "cinek/overview/quadtree.hpp"
 * @brief Container for props and other objects in a map.
 *
 * Ownership rules:
 * - Quadtree is owned by one and only one Map.
 */
class QuadTree
{
public:
/*
    QuadTree(QuadTreeOwner& owner);
    ~QuadTree();

private:
    QuadTreeOwner& _owner;
    // children
    QuadTree* _qTree[4];
    // bounds
    kmAABB _aabb;
*/
};

    } /* overview */ 
} /* cinekine */

#endif
