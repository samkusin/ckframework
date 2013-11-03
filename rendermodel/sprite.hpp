/**
 * \file    rendermodel/sprite.hpp
 *
 * Contains a Sprite Model instance's state information.
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_SPRITE_HPP
#define CINEK_RENDER_SPRITE_HPP

#include "cinek/rendermodel/2d/model.hpp"

namespace cinekine {
    namespace rendermodel {

class SpriteTemplate;

/**
 * \class Sprite sprite.hpp "rendermodel/2d/sprite.hpp"
 * \brief A model representing a collection of one of more bitmaps.
 */
class Sprite: public Model
{
    CK_CLASS_NON_COPYABLE(Sprite);

public:
    /**
     * Constructs a sprite with the given template.
     * @param spriteTemplate Constructs a sprite based on this template.
     */
    Sprite(const SpriteTemplate& spriteTemplate);
    /**
     * Destructor
     */
    virtual ~Sprite();
    /**
     * Updates the sprite's render/animation state.
     * @param deltaTime Time since the last update call.
     */
    virtual void onUpdate(cinek_time deltaTime);
    /**
     * Sets the sprite's state.  State relies on the sprite template's 
     * state table of bitmaps.
     * @param stateId [description]
     */
    virtual void setState(uint32_t stateId);

private:
    const SpriteTemplate& _template;
    cinek_time _frameTime;
    uint32_t _stateId;
};

    } /* rendermodel */ 
} /* cinekine */

#endif
