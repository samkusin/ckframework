/**
 * \file    rendermodel/sprite.cpp
 *
 * Contains a Sprite Model instance's state information.
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "spriteinstance.hpp"
#include "sprite.hpp"
#include "spriteanimation.hpp"

namespace cinekine {
    namespace rendermodel {

SpriteInstance::SpriteInstance(const Sprite& spriteTemplate) :
    _template(spriteTemplate),
    _startTime(0),
    _stateId(kCinekAnimation_Null),
    _animation(nullptr)
{

}

SpriteInstance::~SpriteInstance()
{
}

void SpriteInstance::setState(cinek_rendermodel_anim_id stateId, uint32_t startTime)
{
    _stateId = stateId;
    _startTime = startTime;

    _animation = _template.getAnimation(stateId);
}

cinek_bitmap_atlas SpriteInstance::getBitmapAtlas() const
{
    return _template.getBitmapClass();
}

uint16_t SpriteInstance::getBitmapFrameCount() const
{
    return _animation->getFrameCount();
}

cinek_bitmap_index SpriteInstance::getBitmapFromTime(uint32_t currentTime) const
{
    return _animation->getFrameByTime(currentTime - _startTime);
}

cinek_bitmap_index SpriteInstance::getBitmapFrame(uint16_t index) const
{
    return _animation->getFrame(index);
}

    } /* rendermodel */
} /* cinekine */