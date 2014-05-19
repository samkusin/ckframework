/**
 * \file    rendermodel/modelinstance.hpp
 *
 * A generalized model, representing a visual entity.
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_MODELINSTANCE_HPP
#define CINEK_RENDER_MODELINSTANCE_HPP

#include "cinek/rendermodel/types.h"

namespace cinekine {
    namespace rendermodel {

/**
 * \class ModelInstance
 * \brief An abstract class representing a rendered entity.
 *
 * The Model class abstracts graphical representations of game objects or
 * other higher level objects.   Even model instance contains its own state
 * data.
 *
 * A model's template contains Data shared across multiple model instances.
 * For more information,
 */
class ModelInstance
{
public:
    virtual ~ModelInstance() {}
    /**
     * Sets the model's state.
     * State is an abstract concept on the Model level.  For example, a state could
     * refer to a keyframe or set of bitmaps animated over time.  What states constitute
     * depends on the model's implementation.
     *
     * @param stateId   The State identifier as defined by the application.
     * @param startTime The animation start time used to time animations
     */
    virtual void setState(cinek_rendermodel_anim_id stateId,
                          uint32_t startTime) = 0;
};

    } /* rendermodel */
} /* cinekine */

#endif
