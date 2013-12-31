/**
 * \file    rendermodel/spritetemplate.cpp
 *
 * Model data shared across 
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "./spritetemplate.hpp"

#include <algorithm>
#include <cstring>

namespace cinekine {
    namespace rendermodel {

SpriteTemplate::SpriteTemplate(cinek_bitmap_atlas bitmapClass, uint16_t numStates,
                               const Allocator& allocator) :
    _allocator { allocator },
    _bitmapClass{ bitmapClass },
    _statePool( numStates, allocator ),
    _states(std_allocator<SpriteAnimation*>(allocator))
{
    _states.reserve(numStates);
}

SpriteTemplate::~SpriteTemplate()
{
    for( auto& state : _states )
    {
        _allocator.free(state->_frames);
    }
}

SpriteTemplate::SpriteTemplate(SpriteTemplate&& other) :
    _bitmapClass(other._bitmapClass),
    _statePool(std::move(other._statePool)),
    _states(std::move(other._states))
{
    other._bitmapClass = kCinekBitmapAtlas_Invalid;
}

SpriteTemplate& SpriteTemplate::operator=(SpriteTemplate&& other)
{
    _bitmapClass = other._bitmapClass;
    other._bitmapClass = kCinekBitmapAtlas_Invalid;
    _statePool = std::move(other._statePool);
    _states = std::move(other._states);
    return *this;
}

struct SpriteTemplateState_Comparator
{
    bool operator()(const SpriteAnimation* state, const cinek_rendermodel_anim_id& id) const
    {
        return state->getId() < id;
    }
};

SpriteAnimation* SpriteTemplate::createAnimation(
                            cinek_rendermodel_anim_id animId, 
                            uint16_t frameCount,
                            cinek_time duration
                        )
{
    //  maintain a sorted state vector.
    auto stateIt = std::lower_bound(_states.begin(), _states.end(), animId, SpriteTemplateState_Comparator());
    SpriteAnimation* state = nullptr;
    if (stateIt == _states.end() || (*stateIt)->getId() != animId)
    {
        cinek_bitmap_index* frames = _allocator.allocItems<cinek_bitmap_index>(frameCount);
        state = _statePool.allocateAndConstruct(animId, frameCount, frames);
        _states.emplace(stateIt, state);
    }
    else
    {
        state = (*stateIt);
        _allocator.free(state->_frames);
        state->~SpriteAnimation();
        cinek_bitmap_index* frames = _allocator.allocItems<cinek_bitmap_index>(frameCount);
        ::new(state) SpriteAnimation(animId, frameCount, frames);
    }

    return state;
}

SpriteAnimation* SpriteTemplate::getAnimation(cinek_rendermodel_anim_id animId) const
{
    auto stateIt = std::lower_bound(_states.begin(), _states.end(), animId, SpriteTemplateState_Comparator());
    if (stateIt != _states.end() && (*stateIt)->getId() == animId)
    {
        return *stateIt;
    }
    return nullptr;
}

    }   // namespace rendermodel
}   // namespace cinekine
