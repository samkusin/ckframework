/**
 * \file    overview/map.cpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "cinek/overview/map.hpp"
#include "cinek/overview/debug.hpp"


namespace cinekine {
    namespace overview {

Map::Map(const cinek_ov_map_bounds& bounds, const Allocator& allocator) :
    _bounds(bounds),
    _tilemaps(std_allocator<Tilemap>(allocator))
{
    //  validation
    //
    int32_t layerCount = (_bounds.zUp - _bounds.zDown) + 1;
    if (layerCount <=0)
    {
        _bounds.zUp = _bounds.zDown = 0;
        layerCount = 1;
    }
    if (!_bounds.xUnits || !_bounds.yUnits)
    {
        OV_LOG_ERROR("[Map::Map]", "Invalid bounds specified (%u x %u)", _bounds.xUnits, _bounds.yUnits);
        _bounds.xUnits = 1;
        _bounds.yUnits = 1;
    }

    //  allocate tilemaps
    //      Tilemaps are not copyable - so can't initialize via initializer lists.
    _tilemaps.reserve(layerCount);
    for (int32_t i = 0; i < layerCount; ++i)
    {
        _tilemaps.push_back({ _bounds.xUnits, _bounds.yUnits, std_allocator<cinek_tile>(allocator)});
    }
}


Map::~Map()
{
}


Tilemap* Map::getTilemapAtZ(int16_t z)
{
    return const_cast<Tilemap*>(
            reinterpret_cast<const Map*>(this)->getTilemapAtZ(z)
        );
}

const Tilemap* Map::getTilemapAtZ(int16_t z) const
{
    if (z < _bounds.zDown || z > _bounds.zUp)
        return nullptr;
    return &_tilemaps[z - _bounds.zDown];    
}

    } /* overview */ 
} /* cinekine */
