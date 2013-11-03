/**
 * \file    rendermodel/model.hpp
 *
 * A generalized model, representing a visual entity.
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_MODEL_HPP
#define CINEK_RENDER_MODEL_HPP

#include "cinek/rendermodel/types.h"
#include "cinek/core/cktimer.h"

namespace cinekine {
    namespace rendermodel {

/**
 * \class Model model.hpp "rendermodel/model.hpp"
 * \brief An abstract class representing a rendered entity.
 * 
 * The Model class abstracts graphical representations of game objects or 
 * other higher level objects.   Even model instance contains its own state
 * data.
 * 
 * A model's template contains Data shared across multiple model instances.
 * For more information, 
 */
class Model
{
public:
    virtual ~Model() {}

    /**
     * Updates the model's state.  This should be called with a valid deltaTime to ensure 
     * sync between a model's graphic state and the underlying game engine.
     * 
     * @param deltaTime Time between calls to update.
     */
    virtual void onUpdate(cinek_time deltaTime) = 0;
    /**
     * Sets the model's state.
     * State is an abstract concept on the Model level.  For example, a state could 
     * refer to a keyframe or set of bitmaps animated over time.  What states constitute
     * depends on the model's implementation. 
     * 
     * @param stateId The State identifier as defined by the application.
     */
    virtual void setState(uint32_t stateId) = 0;

};

    } /* rendermodel */ 
} /* cinekine */

#endif
