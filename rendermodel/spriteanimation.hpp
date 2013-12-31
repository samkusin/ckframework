/**
 * @file    rendermodel/spriteanimation.hpp
 *
 * Sprite animation interface
 *
 * @note    Created by Samir Sinha on 12/30/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_SPRITEANIMATION_HPP
#define CINEK_RENDER_SPRITEANIMATION_HPP

#include "cinek/rendermodel/types.h"

namespace cinekine {
    namespace rendermodel {

    class SpriteTemplate;

    /**
     * \class State
     * \brief Links animation states to bitmaps.
     */
    class SpriteAnimation
    {
        friend class SpriteTemplate;
        
    public:
        /**
         * Initializes the state.
         * \param id         State identifier.
         * \param frameCount Number of frames in the state animation.
         */
        SpriteAnimation(cinek_rendermodel_anim_id id, uint16_t frameCount, cinek_bitmap_index* frames);
        /**
         * Returns the bitmap at the specified frame index.
         * @param  index Frame index.
         * @return       Bitmap index.
         */
        cinek_bitmap_index getFrame(uint16_t index) const {
            return _frames[index % _frameCount];
        }
        /**
         * Returns the frame at the specified time index.
         * @param  timeValue The time index.
         * @return           The bitmap frame index.
         */
        cinek_bitmap_index getFrameByTime(cinek_time timeValue) const;
        /**
         * \return Returns the number of frames in the state animation.
         */
        uint16_t getFrameCount() const { return _frameCount; }
        /**
         * \return The State ID.
         */
        cinek_rendermodel_anim_id getId() const { return _id; }
        /**
         * Sets the animation frame at the specified index.
         * @param index  Index into animation.
         * @param bitmap Bitmap index.
         */
        void setFrame(uint16_t index, cinek_bitmap_index bitmap) {
            _frames[index % _frameCount] = bitmap;
        }

    private:
        cinek_rendermodel_anim_id _id;
        cinek_time _duration;
        uint16_t _frameCount;
        cinek_bitmap_index* _frames;
    };

    }   // namespace rendermodel
}   //  namespace cinekine

#endif
