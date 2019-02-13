/*
 * Copyright (c) 2015-2019 Alaskan Emily, Transnat Games,
 *      2016 Eric Armstrong
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

unsigned AIMG_FASTCALL AImg_SavePNG(const struct AImg_Image *from, const char *path){
    unsigned ret = AIMG_LOADPNG_SUCCESS;
    FILE * file;
    png_structp pngs = NULL;
    png_infop info = NULL;

    if(!from || !path)
        return AIMG_LOADPNG_IS_NULL;

    file = fopen(path, "wb");
    if(!file)
        return AIMG_LOADPNG_NO_FILE;

    {
        png_bytep *const rowlist =
            calloc(sizeof(png_bytep), from->h*sizeof(png_byte*));

        pngs = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        info = png_create_info_struct(pngs);
        if((!pngs)||(!info)){
            ret = AIMG_LOADPNG_BADFILE;
            goto ending;
        }

        if(setjmp(png_jmpbuf(pngs))){
            ret = AIMG_LOADPNG_PNG_ERR;
            goto ending;
        }

        png_set_IHDR(pngs, info, from->w, from->h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        {
            unsigned y = 0;
    start_save:
            rowlist[y] = (void *)AImg_PixelConst(from, 0, y);
            y++;
            if(y<from->h)
                goto start_save;

        }
        if(setjmp(png_jmpbuf(pngs))){
          ret = AIMG_LOADPNG_PNG_ERR;
            goto ending;
        }

        png_init_io(pngs, file);

        if(setjmp(png_jmpbuf(pngs))){
          ret = AIMG_LOADPNG_PNG_ERR;
            goto ending;
        }

        png_set_rows(pngs, info, rowlist);

        if(setjmp(png_jmpbuf(pngs))){
            ret = AIMG_LOADPNG_PNG_ERR;
            goto ending;
        }

        png_write_png(pngs, info, PNG_TRANSFORM_IDENTITY, NULL);

        png_destroy_write_struct(&pngs, &info);

        ret = AIMG_LOADPNG_SUCCESS;

    ending:
        free(rowlist);
    }

    png_destroy_info_struct(pngs, &info);
    png_destroy_write_struct(&pngs, &info);

    fflush(file);
    fclose(file);

    return ret;
}
