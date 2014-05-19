/**
 * @file    rendermodel/spritedatabase.hpp
 *
 * Container for SpriteTemplates.
 *
 * @note    Created by Samir Sinha on 7/4/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_SPRITEDATABASE_HPP
#define CINEK_RENDER_SPRITEDATABASE_HPP

#include "cinek/rendermodel/sprite.hpp"
#include "cinek/framework/string.hpp"
#include "cinek/framework/allocator.hpp"
#include "cinek/framework/memorypool.hpp"

#include <unordered_map>

namespace cinekine {
    namespace rendermodel {

/**
 * @class SpriteDatabase
 * @brief A store for SpriteTemplate objects.
 *
 * The SpriteDatabase acts as a container for SpriteTemplate objects.  It also
 * owns SpriteTemplate instances.  If a SpriteDatabase instance is destroyed,
 * all of its SpriteTemplates are also destroyed.  Any application using
 * SpriteDatabase should take this into account.
 */
class SpriteDatabase
{
    CK_CLASS_NON_COPYABLE(SpriteDatabase);

public:
    /**
     * @param initTemplateLimit The initial SpriteTemplate limit value.  Used
     *                          for efficient usage of memory.  SpriteTemplate
     *                          count can exceed the limit, but will result in
     *                          additional memory allocations.
     * @param allocator         An optional allocator.
     */
    SpriteDatabase(size_t initTemplateLimit,
                   const Allocator& allocator=Allocator());
    /**
     * Finds a SpriteTemplate given its ID.
     *
     * @param  id The Sprite Template ID
     * @return    Pointer to the Sprite Template (or NULL.)
     */
    const Sprite* sprite(cinek_sprite_id id) const;
    /**
     * Returns a SpriteTemplate ID given its name.
     *
     * @param  templateName SpriteTemplate name.
     * @return              The ID or kCinekSpriteTemplate_Null.
     */
    cinek_sprite_id spriteIdByName(const char* templateName) const;
    /**
     * Returns the animation ID given its state name.
     *
     * @param  animationName Animation state name.
     * @return               Numeric ID for the animation state name.
     */
    cinek_rendermodel_anim_id animationIDByName(const char* animationName) const;
    /**
     * Creates a SpriteTemplate.
     *
     * @param  name      Name of the template.
     * @param  classId   The bitmap class used for all bitmaps referenced in
     *                   the template.
     * @param  numStates The animation state count.
     * @return           Pointer to a SpriteTemplate or nullptr if out of
     *                   memory.
     */
    Sprite* createOrModifyFromName(const char* name,
        cinek_bitmap_atlas classId,
        uint16_t numStates);
    /**
     * Maps animation state name to a numeric ID.  Numeric IDs are used
     * to reference animations from SpriteTemplates.  If a state is already
     * mapped to the specified id, the method returns False.
     *
     * @param  name Name of the animation state.
     * @param  id   ID value (can be used to reference a SpriteTemplate
     *              animation.)
     * @return      False if a state already exists with the passed id.
     */
    bool mapAnimationStateNameToId(const char* name,
        cinek_rendermodel_anim_id id);

private:
    Allocator _allocator;
    ObjectPool<Sprite> _templatePool;

    template <typename Key, typename T>
    using unordered_map = std::unordered_map<Key, T,
        std::hash<Key>,
        std::equal_to<Key>,
        std_allocator<std::pair<const Key, T>> >;

    unordered_map<string, cinek_rendermodel_anim_id> _nameToAnimIds;
    unordered_map<string, cinek_sprite_id> _nameToIds;
    unordered_map<cinek_sprite_id, Sprite* > _idToTemplates;
};

    }   // namespace rendermodel
}   //  namespace cinekine

#endif
