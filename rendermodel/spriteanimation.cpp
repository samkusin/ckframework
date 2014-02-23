/**
 * @file    rendermodel/spriteanimation.cpp
 *
 * Sprite animation interface
 *
 * @note    Created by Samir Sinha on 12/30/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "./spriteanimation.hpp"

namespace cinekine {
    namespace rendermodel {

    SpriteAnimation::SpriteAnimation(cinek_rendermodel_anim_id id, uint16_t frameCount,
                                     cinek_bitmap_index* frames) :
        _id {id},
        _duration {0},
        _frameCount {frameCount},
        _frames {frames}
    {
    }

    cinek_bitmap_index SpriteAnimation::getFrameByTime(uint32_t timeValue) const
    {
        float scalar = (float)timeValue/_duration;
        return getFrame((uint16_t)(getFrameCount() * scalar));
    }

    }   // namespace rendermodel
}   //  namespace cinekine