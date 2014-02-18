/**
 * \file    overview/stage.hpp
 *
 * Interface for the Stage.
 *
 * \note    Created by Samir Sinha on 2/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_STAGE_HPP
#define CINEK_OVERVIEW_STAGE_HPP

#include "./maptypes.h"
#include "cinek/framework/allocator.hpp"

namespace cinekine {
    namespace overview {

class Map;

/**
 * @class Stage stage.hpp "cinek/overview/stage.hpp"
 * @brief The game simulation mangaging interactions between game objects in a view.
 *
 * The stage runs the game simulation on a map usually supplied by a world 
 * model, and supplies renderers with a portion of the map to draw.
 *
 * A stage obtains data from a Director instance supplied by the 
 * application.
 * 
 * Ownership rules:
 * -
 *  
 */
class Stage 
{
    CK_CLASS_NON_COPYABLE(Stage);

public:
    /** 
     * Creates a stage with the given map boundaries used to generate the stage
     * map.
     *
     * @param mapBounds Map dimensions defining the stage's map.
     * @param allocator An optional allocator.
     */
    Stage(const cinek_ov_map_bounds& mapBounds, 
          const Allocator& allocator=Allocator());

    /** Destructor */
    virtual ~Stage();

    /** 
     * @return A shared pointer to a Map reference. 
     */
    std::shared_ptr<Map> getMapPtr() {
        return _map;
    }

private:
    std::shared_ptr<Map> _map;
};

    } /* overview */ 
} /* cinekine */

#endif
