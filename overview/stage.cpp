/**
 * \file    overview/stage.cpp
 *
 * Interface for the Stage.
 *
 * \note    Created by Samir Sinha on 2/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "./stage.hpp"
#include "./map.hpp"

namespace cinekine {
    namespace overview {


Stage::Stage(const cinek_ov_map_bounds& mapBounds,
	         const Allocator& allocator) :
    _map()
{
    _map = std::allocate_shared<Map, MapAllocator, const cinek_ov_map_bounds&>(
                MapAllocator(allocator), mapBounds
            );
}

Stage::~Stage()
{
}


    } /* overview */ 
} /* cinekine */
