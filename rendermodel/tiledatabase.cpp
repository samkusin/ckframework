/**
 * @file    rendermodel/tiledatabase.cpp
 *
 * Container for Tiles
 *
 * @note    Created by Samir Sinha on 7/4/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "tiledatabase.hpp"

namespace cinekine {
    namespace rendermodel {

    TileDatabase::TileDatabase(uint16_t tileLimit, const Allocator& allocator) :
        _tiles(tileLimit, cinek_tile_info {
                                            { 
                                              kCinekBitmapAtlas_Invalid, 
                                              kCinekBitmapIndex_Invalid 
                                            },
                                            0, nullptr
                                          },
               std_allocator<cinek_tile_info>(allocator))
    {
    }
    TileDatabase::TileDatabase(TileDatabase&& other) :
        _tiles(std::move(other._tiles))
    {
    }
   
    void TileDatabase::clear()
    {
        _tiles.clear();
    }
   
    void TileDatabase::mapTileToInfo(cinek_tile tile, const cinek_tile_info& info)
    {
        if (tile >= _tiles.size())
            return;
        _tiles[tile] = info;
    }
        
    }   // namespace rendermodel
}   //  namespace cinekine
