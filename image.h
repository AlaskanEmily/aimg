/*
 * Copyright (c) 2015-2019 Alaskan Emily, Transnat Games
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef AIMG_IMAGE_H
#define AIMG_IMAGE_H
#pragma once

#include "export.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct AImg_Image{
    uint32_t *pixels;
    unsigned w, h;
};

AIMG_API(void) AImg_CloneImage(struct AImg_Image *to, const struct AImg_Image *from);

AIMG_API(void) AImg_SetPixel(struct AImg_Image *to, int x, int y, uint32_t color);
AIMG_API(uint32_t) AImg_GetPixel(struct AImg_Image *to, int x, int y);

AIMG_API(void) AImg_CreateImage(struct AImg_Image *that, unsigned w, unsigned h);
AIMG_API(void) AImg_DestroyImage(struct AImg_Image *that);

AIMG_API(uint32_t) AImg_RGBARawBlend(uint32_t src, uint32_t dst);

AIMG_API(void) AImg_Blit(const struct AImg_Image *src, struct AImg_Image *dst, int x, int y);

/*
 * Semi-private, does no clipping.
 */
AIMG_API(uint32_t *)AImg_Pixel(struct AImg_Image *to, int x, int y);
AIMG_API(const uint32_t *)AImg_PixelConst(const struct AImg_Image *to, int x, int y);

AIMG_API(uint32_t) AImg_RGBAToRaw(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
AIMG_API(void) AImg_RawToRGBA(uint32_t rgba, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a);
AIMG_API(uint8_t)AImg_RawToR(uint32_t rgba);
AIMG_API(uint8_t)AImg_RawToG(uint32_t rgba);
AIMG_API(uint8_t)AImg_RawToB(uint32_t rgba);
AIMG_API(uint8_t)AImg_RawToA(uint32_t rgba);

/* From and to can be the same image safely. */
AIMG_API(void) AImg_FlipImageVertically(const struct AImg_Image *from, struct AImg_Image *to);

#define AIMG_LOADPNG_SUCCESS 0u
#define AIMG_LOADPNG_NO_FILE 1u
#define AIMG_LOADPNG_BADFILE 2u
#define AIMG_LOADPNG_NFORMAT 4u
#define AIMG_LOADPNG_IS_NULL 8u
#define AIMG_LOADPNG_PNG_ERR 16u

/*
 * Returns 0 on success
 */
AIMG_API(unsigned)AImg_LoadAuto(struct AImg_Image *to, const char *path);
AIMG_API(unsigned)AImg_LoadPNGMem(struct AImg_Image *to, const void *data, unsigned size);
AIMG_API(unsigned)AImg_LoadTGAMem(struct AImg_Image *to, const void *data, unsigned size);
AIMG_API(unsigned)AImg_LoadPNG(struct AImg_Image *to, const char *path);
AIMG_API(unsigned)AImg_LoadTGA(struct AImg_Image *to, const char *path);

AIMG_API(unsigned)AImg_SaveAuto(const struct AImg_Image *from, const char *path);
AIMG_API(unsigned)AImg_SavePNG(const struct AImg_Image *from, const char *path);
AIMG_API(unsigned)AImg_SaveTGA(const struct AImg_Image *from, const char *path);
AIMG_API(unsigned)AImg_SaveJPG(const struct AImg_Image *from, const char *path);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* AIMG_IMAGE_H */
