/**
 * \file    rendermodel/types.h
 *
 * \note    Created by Samir Sinha on 1/6/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_TYPES_H
#define CINEK_RENDER_TYPES_H

#include "cinek/types.hpp"

#ifdef __cplusplus
extern "C" {
#endif

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
    cinek_bitmap_atlas bmpAtlas;    /**< Bitmap Atlas ID */
    cinek_bitmap_index bmpIndex;    /**< Bitmap Index into Atlas */
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


#ifdef __cplusplus
}
#endif

#endif
