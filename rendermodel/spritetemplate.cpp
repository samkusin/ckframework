/**
 * \file    rendermodel/spritetemplate.cpp
 *
 * Model data shared across 
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "spritetemplate.hpp"

#include <algorithm>
#include <cstring>

namespace cinekine {
    namespace rendermodel {

/**
 * \class State
 * \brief Links animation states to bitmaps.
 */
class SpriteTemplate::State
{
    friend class SpriteTemplate;

public:
    /**
     * Initializes the state.
     * \param id         State identifier.
     * \param frameCount Number of frames in the state animation.
     */
    State(cinek_rendermodel_anim_id id, uint16_t frameCount, cinek_bitmap_index* frames);
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


SpriteTemplate::State::State(cinek_rendermodel_anim_id id, uint16_t frameCount,
                             cinek_bitmap_index* frames) :
    _id {id},
    _duration {0},
    _frameCount {frameCount},
    _frames {frames}
{
}

cinek_bitmap_index SpriteTemplate::State::getFrameByTime(cinek_time timeValue) const
{
    float scalar = (float)timeValue/_duration;
    return getFrame((uint16_t)(getFrameCount() * scalar));
}

SpriteTemplate::Animation::Animation() :
    _state { nullptr }
{
}

SpriteTemplate::Animation::Animation(SpriteTemplate::State* state) :
    _state(state)
{

}
cinek_bitmap_index SpriteTemplate::Animation::getFrame(uint16_t index) const
{
    return _state ? _state->getFrame(index) : kCinekBitmapIndex_Invalid;
}
        
cinek_bitmap_index SpriteTemplate::Animation::getFrameByTime(cinek_time timeValue) const
{
    return _state ? _state->getFrameByTime(timeValue) : kCinekBitmapIndex_Invalid;
}

cinek_bitmap_index SpriteTemplate::Animation::getFrameCount() const
{
    return _state ? _state->getFrameCount() : 0;
}

void SpriteTemplate::Animation::setFrame(uint16_t index, cinek_bitmap_index bitmap)
{
    if (_state)
    {
        _state->setFrame(index, bitmap);
    }
}


SpriteTemplate::SpriteTemplate(cinek_bitmap_atlas bitmapClass, uint16_t numStates,
                               const Allocator& allocator) :
    _allocator { allocator },
    _bitmapClass{ bitmapClass },
    _statePool( numStates, allocator ),
    _states(std_allocator<State*>(allocator))
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
    bool operator()(const SpriteTemplate::State* state, const cinek_rendermodel_anim_id& id) const
    {
        return state->getId() < id;
    }
};

SpriteTemplate::Animation SpriteTemplate::createAnimation(
                            cinek_rendermodel_anim_id animId, 
                            uint16_t frameCount,
                            cinek_time duration
                        )
{
    //  maintain a sorted state vector.
    auto stateIt = std::lower_bound(_states.begin(), _states.end(), animId, SpriteTemplateState_Comparator());
    State* state = nullptr;
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
        state->~State();
        cinek_bitmap_index* frames = _allocator.allocItems<cinek_bitmap_index>(frameCount);
        ::new(state) State(animId, frameCount, frames);
    }

    return Animation(state);
}

SpriteTemplate::Animation SpriteTemplate::getAnimation(cinek_rendermodel_anim_id animId) const
{
    auto stateIt = std::lower_bound(_states.begin(), _states.end(), animId, SpriteTemplateState_Comparator());
    if (stateIt != _states.end() && (*stateIt)->getId() == animId)
    {
        return Animation((*stateIt));
    }
    return Animation();
}

    }   // namespace rendermodel
}   // namespace cinekine
