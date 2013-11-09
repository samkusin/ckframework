/**
 * \file    overview/viewpoint.cpp
 *
 * Viewpoint class, defines a read-only representation of a map.
 *
 * \note    Created by Samir Sinha on 3/10/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "cinek/overview/viewpoint.hpp"
#include "cinek/overview/map.hpp"
#include "cinek/overview/debug.hpp"

#include <algorithm>

namespace cinekine {
    namespace overview {


Viewpoint::Viewpoint(std::shared_ptr<Map> map, 
                const cinek_ov_pos& position,
                const cinek_ov_map_bounds& bounds
            ) :
    _map(map),
    _position(position),
    _bounds(bounds)
{

}

Viewpoint::Viewpoint(Viewpoint&& other) :
    _map(std::move(other._map)),
    _position(std::move(other._position)),
    _bounds(std::move(other._bounds))
{
    other._bounds = { 0, 0, 0, 0 };
}

Viewpoint& Viewpoint::operator=(Viewpoint&& other)
{
    return *this;
}


ViewpointTilemap Viewpoint::getTilemapContainerForZ(int16_t z)
{
    ViewpointTilemap viewpointTilemap;
    if (_map)
    {
        Tilemap* tilemap = _map.get()->getTilemapAtZ(z);
        if (tilemap != nullptr)
        {
            const int32_t MAP_WIDTH = tilemap->getColumnCount();
            const int32_t MAP_HEIGHT = tilemap->getRowCount();
            const int32_t MAP_HALF_WIDTH = MAP_WIDTH/2;
            const int32_t MAP_HALF_HEIGHT = MAP_HEIGHT/2;
            const int32_t vpX = (int32_t)_position.x;
            const int32_t vpY = (int32_t)_position.y;

            //  obtain viewpoint rectangle bounds within the parent tilemap.
            int32_t vpLeft = vpX + MAP_HALF_WIDTH - _bounds.xUnits/2;
            int32_t vpTop = vpY + MAP_HALF_HEIGHT - _bounds.yUnits/2;
            int32_t vpRight = vpLeft + _bounds.xUnits;
            int32_t vpBottom = vpTop + _bounds.yUnits;
            if (vpLeft < 0)
            {
                viewpointTilemap.margin.left = -vpLeft;
            }
            if (vpRight >= MAP_WIDTH)
            {
                viewpointTilemap.margin.right = viewpointTilemap.margin.left + vpRight - MAP_WIDTH;
            }
            if (vpTop < 0)
            {
                viewpointTilemap.margin.top = -vpTop;
            }
            if (vpBottom >= MAP_HEIGHT)
            {
                viewpointTilemap.margin.bottom = viewpointTilemap.margin.top + vpBottom - MAP_HEIGHT;
            }

            vpLeft = std::max(vpLeft, 0);
            vpTop = std::max(vpTop, 0);

            int32_t vpRows = 0, vpColumns = 0;
            if (vpY <= MAP_HEIGHT)
            {
                vpRows = std::min((int32_t)_bounds.yUnits, MAP_HEIGHT - vpY);
            }
            if (vpX <= MAP_WIDTH)
            {
                vpColumns = std::min((int32_t)_bounds.xUnits, MAP_WIDTH - vpX);
            }

            viewpointTilemap.tilemap = TilemapContainer(*tilemap, vpLeft, vpTop, vpRows, vpColumns);
        }
    }
    return viewpointTilemap;
}

    } /* overview */ 
} /* cinekine */
