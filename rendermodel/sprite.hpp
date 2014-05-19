/**
 * @file    rendermodel/sprite.hpp
 *
 * Sprite related data shared across multiple sprite instances.
 *
 * @note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_SPRITE_HPP
#define CINEK_RENDER_SPRITE_HPP

#include "cinek/rendermodel/types.h"
#include "cinek/rendermodel/spriteanimation.hpp"
#include "cinek/framework/allocator.hpp"
#include "cinek/framework/memorypool.hpp"
#include "cinek/framework/vector.hpp"

namespace cinekine {
    namespace rendermodel {

/**
 * \class Sprite
 * \brief Defines bitmaps and animation states for a single sprite.
 */
class Sprite
{
    CK_CLASS_NON_COPYABLE(Sprite);

public:
    /**
     * Constructor reserving memory for a number of states for later definition,
     * serialization.
     *
     * @param bitmapClass   The bitmap class of bitmaps used for this sprite.
     * @param numStates     Number of states to reserve in the state table.
     * @param allocator     An optional allocator.
     */
    Sprite(cinek_bitmap_atlas bitmapClass, uint16_t numStates,
           const Allocator& allocator=Allocator());
    /** Destructor */
    ~Sprite();
    /** @cond */
    Sprite(Sprite&& other);
    Sprite& operator=(Sprite&& other);

    /** @endcond */
    /**
     * @return Returns the bitmap class for bitmaps used in the sprite.
     */
    cinek_bitmap_atlas getBitmapClass() const {
        return _bitmapClass;
    }

public:
    /**
     * Create or modify an animation with the specified animation ID.
     *
     * @param  animId     Animation identifier.
     * @param  frameCount The number of frames in the animation.
     * @param  duration   The length of the animation.
     * @return            The animation state created
     */
    SpriteAnimation* createAnimation(cinek_rendermodel_anim_id animId,
                                 uint16_t frameCount,
                                 uint32_t duration);

    /**
     * Returns the Animation interface for the specified animation.
     * @param  animId Animation ID.
     * @return        The mapped animation or a placeholder animation if the
     *                animation wasn't initialized.
     */
    SpriteAnimation* getAnimation(cinek_rendermodel_anim_id animId) const;

private:
    Allocator _allocator;
    cinek_bitmap_atlas _bitmapClass;

    //  state table containing animation lists mapped to state.
    ObjectPool<SpriteAnimation> _statePool;
    vector<SpriteAnimation*> _states;
};

    }   // namespace rendermodel
}   //  namespace cinekine

#endif
