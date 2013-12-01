/**
 * \file    rendermodel/types.h
 *
 * \note    Created by Samir Sinha on 1/6/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_TYPES_H
#define CINEK_RENDER_TYPES_H

#include "cinek/core/cktypes.h"


#ifdef __cplusplus
extern "C" {
#endif

/** A RGBA quartet using 8-bit components */
typedef struct cinek_rgba_color_t {
    uint8_t r;      /**< Red component */
    uint8_t g;      /**< Green component */
    uint8_t b;      /**< Blue component */
    uint8_t a;      /**< Alpha component */
}
cinek_rgba_color;

/*
    Bitmap Types
*/
/** Defines pixel formats supported by the renderer. */
typedef enum cinek_pixel_format_t
{
    kCinekPixelFormat_Unknown,
    kCinekPixelFormat_ARGB8888,
    kCinekPixelFormat_ABGR8888,
    kCinekPixelFormat_ARGB4444,
    kCinekPixelFormat_RGB888,
    kCinekPixelFormat_ARGB1555,
    kCinekPixelFormat_RGB565,
    kCinekPixelFormat_A8            /**< 8-bit Alpha, assuming RGB of white. */
}
cinek_pixel_format;

/** Defines bitmap formats supported by the renderer. */
typedef enum cinek_bitmap_type_t
{
    kCinekBitmapType_Unknown = 1,
    kCinekBitmapType_PNG
}
cinek_bitmap_type;

/** An index to a bitmap collection. */
typedef uint16_t cinek_bitmap_atlas;
/** An index to a bitmap in a bitmap collection */
typedef uint16_t cinek_bitmap_index;

/** Define for an invalid bitmap class value. */
#define kCinekBitmapAtlas_Invalid      ((cinek_bitmap_atlas)-1)
/** Define for a null bitmap index. */
#define kCinekBitmapIndex_Invalid      ((cinek_bitmap_index)-1)

/** Identifies a bitmap from its components. */
typedef struct cinek_bitmap_t
{
    cinek_bitmap_atlas bmpAtlas;
    cinek_bitmap_index bmpIndex;
}
cinek_bitmap;

/** Contains the UVs for a bitmap within a texture. */
typedef struct cinek_bitmap_uvs_t
{
    float u0;   /**< The upper-left U coord of a bitmap. */
    float v0;   /**< The upper-top V coord of a bitmap. */
    float u1;   /**< The lower right U coord of a bitmap. */
    float v1;   /**< The lower bottom V coord of a bitmap. */
}
cinek_bitmap_uvs;

/** Identifies a tile on the tile map. */
typedef uint16_t cinek_tile;

/** Identifies a sprite by key ID. */
typedef uint32_t cinek_sprite_template_id;
/** Identifies the animation of a particular model. */
typedef uint16_t cinek_rendermodel_anim_id;
/** Define for a null sprite template. */
#define kCinekSpriteTemplate_Null   ((cinek_sprite_template_id)0)
/** Define for a null sprite template. */
#define kCinekAnimation_Null        ((cinek_rendermodel_anim_id)0)

/** Keyframe information for an animation. */
typedef struct cinek_rendermodel_keyframe_t
{
    uint16_t frame; /**< Frame index. */
    cinek_time t;   /**< Time index at this frame. */
}
cinek_rendermodel_keyframe;

#ifdef __cplusplus
}
#endif

#endif
