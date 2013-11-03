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
        _tiles(tileLimit, cinek_bitmap { kCinekBitmapAtlas_Invalid, kCinekBitmapIndex_Invalid },
               std_allocator<cinek_bitmap>(allocator))
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
   
    void TileDatabase::mapTileToBitmap(cinek_tile tile, const cinek_bitmap& bitmap)
    {
        if (tile >= _tiles.size())
            return;
        _tiles[tile] = bitmap;
    }
        
    }   // namespace rendermodel
}   //  namespace cinekine
