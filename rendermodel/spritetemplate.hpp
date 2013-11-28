/**
 * @file    rendermodel/spritetemplate.hpp
 *
 * Sprite related data shared across multiple sprite instances.
 *
 * @note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_SPRITETEMPLATE_HPP
#define CINEK_RENDER_SPRITETEMPLATE_HPP

#include "cinek/rendermodel/types.h"
#include "cinek/cpp/ckalloc.hpp"
#include "cinek/cpp/ckmemorypool.hpp"
#include "cinek/cpp/ckvector.hpp" 

namespace cinekine {
    namespace rendermodel {

/**
 * \class SpriteTemplate spritetemplate.hpp "rendermodel/2d/spritetemplate.hpp"
 * \brief Defines bitmaps and animation states for a single sprite.
 */
class SpriteTemplate
{
public:
    /**
     * Constructor reserving memory for a number of states for later definition,
     * serialization.
     *
     * @param bitmapClass   The bitmap class of bitmaps used for this sprite.
     * @param numStates     Number of states to reserve in the state table.
     * @param allocator     An optional allocator.
     */
    SpriteTemplate(cinek_bitmap_atlas bitmapClass, uint16_t numStates,
                   const Allocator& allocator=Allocator());
    /** Destructor */
    ~SpriteTemplate();
    /** @cond */
    SpriteTemplate(SpriteTemplate&& other);
    SpriteTemplate& operator=(SpriteTemplate&& other);
    /** @endcond */
    /**
     * @return Returns the bitmap class for bitmaps used in the sprite.
     */
    cinek_bitmap_atlas getBitmapClass() const {
        return _bitmapClass;
    }
 
public:
    class State;                /**< State object defined internally */
    /**
     * @class Animation
     * @brief Links animation states to bitmaps.
     */
    class Animation
    {
    public:
        Animation();

        /**
         * Returns the bitmap at the specified frame index.
         * @param  index Frame index.
         * @return       Bitmap index.
         */
        cinek_bitmap_index getFrame(uint16_t index) const;
        /**
         * Returns the frame at the specified time index.
         * @param  timeValue The time index.
         * @return           The bitmap frame index.
         */
        cinek_bitmap_index getFrameByTime(cinek_time timeValue) const;
        /**
         * @return Returns the number of frames in the state animation.
         */
        uint16_t getFrameCount() const;
        /**
         * Sets a bitmap frame for the specified animation.
         * @param index  Zero based index up to the frame count minus one.
         * @param bitmap The bitmap index.
         */
        void setFrame(uint16_t index, cinek_bitmap_index bitmap);

    private:
        friend class SpriteTemplate;
        Animation(State* state);
        State* _state;
    };

public:
    /**
     * Create or modify an animation with the specified animation ID.
     * 
     * @param  animId     Animation identifier.
     * @param  frameCount The number of frames in the animation.
     * @param  duration   The length of the animation.
     */
    Animation createAnimation(cinek_rendermodel_anim_id animId,
                       uint16_t frameCount,
                       cinek_time duration);

    /**
     * Returns the Animation interface for the specified animation.
     * @param  animId Animation ID.
     * @return        The mapped animation or a placeholder animation if the 
     *                animation wasn't initialized.
     */
    Animation getAnimation(cinek_rendermodel_anim_id animId) const;

private:
    Allocator _allocator;
    cinek_bitmap_atlas _bitmapClass;

    //  state table containing animation lists mapped to state.
    ObjectPool<State> _statePool;
    vector<State*> _states;
};

    }   // namespace rendermodel
}   //  namespace cinekine

#endif
