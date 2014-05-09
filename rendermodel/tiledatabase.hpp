/**
 * @file    rendermodel/tiledatabase.hpp
 *
 * Container for Tiles
 *
 * @note    Created by Samir Sinha on 7/4/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_TILEDATABASE_HPP
#define CINEK_RENDER_TILEDATABASE_HPP

#include <cinek/framework/debug.hpp>
#include <cinek/framework/string.hpp>
#include <cinek/framework/vector.hpp>
#include <cinek/rendermodel/types.h>
#include <cinek/framework/map.hpp>

#include <vector>

namespace cinekine {
    namespace rendermodel {

/**
 * @class TileDatabase
 * @brief A store for Tile records.
 *
 * Tiles are compressed bitmap atlas/index pairs to minimize the memory usage of
 * tilemap grids.
 */
class TileDatabase
{
    CK_CLASS_NON_COPYABLE(TileDatabase);

public:
    /**
     * @param tileLimit The maximum number of tiles to reserve for the
     *                  database.
     * @param allocator An optional allocator.
     */
    TileDatabase(uint16_t tileLimit, const Allocator& allocator=Allocator());
    /**
     * @param other The source database.
     */
    TileDatabase(TileDatabase&& other);
    /**
     * Clears the database.
     */
    void clear();
    /**
     * @return The maximum number of tiles allowed in the database.
     */
    /**
     * Maps a tile atlas to a bitmap atlas.
     *
     * @param tile   The tile index to map bitmap info to.
     * @param bitmap The bitmap atlas and index used for the specified tile.
     */
    void mapTileToInfo(cinek_tile tile, const cinek_tile_info& info);
    /**
     * @param  tile The tile info to retrieve.
     * @return A TileInfo pair object containing bitmap atlas and index.
     */
    const cinek_tile_info& tileInfo(cinek_tile tile) const {
        CK_ASSERT(tile < _tiles.size());
        return _tiles.at(tile);
    }

    /**
     * Retrieves a TileTemplate from the given description/selection
     * parameters
     * @param  categoryId Tile category
     * @param  classId    Tile class
     * @param  roleFlags  Tile role
     * @return            The matching TileTemplate.  If there is no tile
     *                    found, the returned template has an handle of 0
     *                    which usually indicates an empty tile
     */
    cinek_tile tileHandleFromDescriptor(uint8_t categoryId,
                                uint8_t classId,
                                uint16_t roleFlags) const;

private:
    vector< cinek_tile_info > _tiles;
    unordered_map<uint32_t, cinek_tile> _tilesByDescriptor;

    uint32_t makeDescriptor(const cinek_tile_info& tile) const;
};

    }   // namespace rendermodel
}   //  namespace cinekine

#endif
