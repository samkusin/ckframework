/**
 * @file    rendermodel/spritetemplate.cpp
 *
 * Sprite related data shared across multiple sprite instances.
 *
 * @note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "spritedatabase.hpp"
#include "sprite.hpp"

namespace cinekine {
    namespace rendermodel {

SpriteDatabase::SpriteDatabase(size_t initTemplateLimit, const Allocator& allocator) :
    _allocator(allocator),
    _templatePool(initTemplateLimit, allocator),
    _nameToAnimIds(std_allocator<std::pair<string, cinek_rendermodel_anim_id>>(allocator)),
    _nameToIds(std_allocator<std::pair<string, cinek_sprite_id>>(allocator)),
    _idToTemplates(std_allocator<std::pair<cinek_sprite_id, SpriteTemplate*> >(allocator))
{
}

const Sprite* SpriteDatabase::sprite(cinek_sprite_id id) const
{
    auto it = _idToTemplates.find(id);
    if (it == _idToTemplates.end())
        return nullptr;
    return it->second;
}

cinek_sprite_id SpriteDatabase::spriteIdByName(const char* templateName) const
{
    auto it = _nameToIds.find(templateName);
    if (it == _nameToIds.end())
        return kCinekSprite_Null;
    return it->second;
}

cinek_rendermodel_anim_id SpriteDatabase::animationIDByName(const char* animationName) const
{
    auto it = _nameToAnimIds.find(animationName);
    if (it == _nameToAnimIds.end())
        return kCinekAnimation_Null;
    return it->second;
}

Sprite* SpriteDatabase::createOrModifyFromName(const char* name,
        cinek_bitmap_atlas classId,
        uint16_t numStates)
{
    //  check if a sprite of that name exists, if so use that sprite's id,
    //  and replace the old template with a new one.
    //  otherwise just create a new one.
    auto itId = _nameToIds.find(string(name));
    cinek_sprite_id id;
    if (itId != _nameToIds.end())
    {
        id = itId->second;
    }
    else {
        id = _nameToIds.size()+1;
    }

    Sprite* theTemplate;
    auto itTemplate = _idToTemplates.find(id);
    if (itTemplate != _idToTemplates.end())
    {
        theTemplate = itTemplate->second;
        theTemplate->~Sprite();
        new(theTemplate) Sprite(classId, numStates, _allocator);
    }
    else
    {
        theTemplate = _templatePool.allocateAndConstruct(classId, numStates, _allocator);
        _idToTemplates.emplace(id, theTemplate);
    }
    return theTemplate;
}

bool SpriteDatabase::mapAnimationStateNameToId(const char* name,
        cinek_rendermodel_anim_id id)
{
    for(const auto& val : _nameToAnimIds)
    {
        if (val.second == id)
            return false;
    }
    _nameToAnimIds[string(name)] = id;
    return true;
}


    }   // namespace rendermodel
}   //  namespace cinekine