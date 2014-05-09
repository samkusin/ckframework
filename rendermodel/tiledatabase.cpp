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
                                            0, 0, 0
                                          },
               std_allocator<cinek_tile_info>(allocator)),
        _tilesByDescriptor(std_allocator<std::pair<uint32_t, cinek_tile>>(allocator))
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

        uint32_t descriptor = makeDescriptor(info);
        _tilesByDescriptor[descriptor] = tile;
    }

    cinek_tile TileDatabase::tileHandleFromDescriptor(
                                    uint8_t categoryId,
                                    uint8_t classId,
                                    uint16_t roleFlags) const
    {
        cinek_tile_info tile;
        tile.categoryIndex = categoryId;
        tile.classIndex = classId;
        tile.flags = roleFlags;
        uint32_t descriptor = makeDescriptor(tile);
        auto it = _tilesByDescriptor.find(descriptor);
        if (it == _tilesByDescriptor.end())
            return 0;

        return it->second;
    }

    uint32_t TileDatabase::makeDescriptor(const cinek_tile_info& tile) const
    {
        uint32_t descriptor = tile.flags;
        descriptor <<= 8;
        descriptor += tile.categoryIndex;
        descriptor <<= 8;
        descriptor += tile.classIndex;
        return descriptor;
    }

    }   // namespace rendermodel
}   //  namespace cinekine
