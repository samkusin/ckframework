/**
 * \file    overview/maptypes.h
 *
 * Contains map specific types and type interfaces.
 *
 * \note    Created by Samir Sinha on 1/31/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_MAP_TYPES_H
#define CINEK_OVERVIEW_MAP_TYPES_H

#include "cinek/overview/types.h"
#include "glm/glm.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/** Defines the XYZ bounds of a map. */  
typedef struct cinek_ov_map_bounds_t
{
    uint16_t xUnits;    /**< Number of units/tiles on the x axis. */
    uint16_t yUnits;    /**< Number of units/tiles on the y axis. */
    int16_t zUp;        /**< Number of tile layers on the +z axis. */
    int16_t zDown;      /**< Number of tile layers on the -z axis. */
}
cinek_ov_map_bounds;

/** Defines a location on the map */
typedef glm::vec3 cinek_ov_pos;

/** Defines a boundary box (min/max) */
typedef struct cinek_ov_bounds {
    cinek_ov_pos min;
    cinek_ov_pos max;
}
cinek_ov_bounds;

/** Defines a tiled region within the tilemap. */
typedef struct cinek_tilemap_rect_t
{
    uint16_t left;      /**< The left bound of the tilemap rect. */
    uint16_t top;       /**< The top bound of the tilemap rect. */
    uint16_t right;     /**< The right bound of the tilemap rect. */
    uint16_t bottom;    /**< The bottom bound of the tilemap rect. */
}
cinek_tilemap_rect;

#ifdef __cplusplus
}
#endif

#endif /* CINEK_OVERVIEW_MAP_TYPES_H */
