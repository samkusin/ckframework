/**
 * \file    overview/viewpoint.hpp
 *
 * Viewpoint class, defines a read-only representation of a map.
 *
 * \note    Created by Samir Sinha on 3/10/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_VIEWPOINT_HPP
#define CINEK_OVERVIEW_VIEWPOINT_HPP

#include "cinek/overview/map.hpp"
#include <memory>
 
namespace cinekine {
    namespace overview {

class Map;

/**
 * @class ViewpointTilemap viewpoint.hpp "cinek/overview/viewpoint.hpp"
 * @brief Represents the tilemap within a viewpoint, including clipping
 * information.
 */
struct ViewpointTilemap
{
    /** The viewpoint's underlying tilemap container. */
    TilemapContainer tilemap;
    /** Defines a window within the container. */
    cinek_tilemap_rect margin;
    /** Default constructor. */
    ViewpointTilemap(): tilemap(), margin{0,0,0,0} {}
};

/**
 * @class Viewpoint viewpoint.hpp "cinek/overview/viewpoint.hpp"
 * @brief A viewpoint represents a portion of a map.
 * 
 * Viewpoint objects allow read-only operations on a map section.
 * Viewpoints have a center-point and bounds relative to that center
 * point.  When a renderer obtains a map section, the viewpoint returns
 * map data based on the center point and bounds.
 * 
 * Ownership rules:
 * - A viewpoint has shared ownership of its map.
 *  
 */
class Viewpoint 
{
public:
    /** Empty Viewpoint constructor. */
    Viewpoint() = default;
    /**
     * Creates a viewpoint for the specified map.  The viewpoint is centered at 
     * (0,0,0) on the map.
     * @param map       A shared pointer to a read-only Map object.
     * @param position  The center-point map position of the viewpoint.
     * @param bounds    The initial bounds for the viewpoint.
     */
    Viewpoint(std::shared_ptr<Map> map, 
                const cinek_ov_pos& position,
                const cinek_ov_map_bounds& bounds
                );
    /** Destructor */
    virtual ~Viewpoint() = default;
    /**
     * Copy constructor.
     * @param other The Viewpoint to copy resources from.
     */
    Viewpoint(const Viewpoint& other) = default;

    /**
     * Move constructor.
     * @param other The Viewpoint to move resources from.
     */
    Viewpoint(Viewpoint&& other);
    /**
     * Move operator.
     * @param other The Viewpoint to move resources from.
     */
    Viewpoint& operator=(Viewpoint&& other);

    /**
     * Returns a Tilemap container for the specified layer.
     * @param  z [description]
     * @return   [description]
     */
    ViewpointTilemap getTilemapContainerForZ(int16_t z);
    /**
     * Sets the center-point for the viewpoint.
     * @param pos       Map coordinate.
     */
    void setPosition(const cinek_ov_pos& pos) {
        _position = pos;
    }
    /**
     * Returns the current position.
     * @return          Current map coordinate.
     */
    const cinek_ov_pos& getPosition() const {
        return _position;
    }
    /**
     * Sets the current relative boundaries for the viewpoint.
     * @param bounds    Map bounds for the current snapshot.
     */
    void setBounds(const cinek_ov_map_bounds& bounds) {
        _bounds = bounds;
    }
    /**
     * Returns the current viewpoint bounds.
     * @return          Viewpoint relative map boundaries.
     */
    const cinek_ov_map_bounds& getBounds() const {
        return _bounds;
    }

private:
    std::shared_ptr<Map> _map;
    cinek_ov_pos _position;
    cinek_ov_map_bounds _bounds;
};

    } /* overview */ 
} /* cinekine */

#endif
