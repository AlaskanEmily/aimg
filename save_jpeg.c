/*
 * Copyright (c) 2017-2019 Alaskan Emily, Transnat Games
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
#include <setjmp.h>
#include <stdio.h>
#include <jpeglib.h>

static void aimg_jpeg_error(j_common_ptr ptr){
    jmp_buf *const jmpbuf = ptr->client_data;
    longjmp(*jmpbuf, 0xFF);
}

unsigned AIMG_FASTCALL AImg_SaveJPG(const struct AImg_Image *from, const char *path){
    jmp_buf jmpbuf;
    struct jpeg_compress_struct compress;
    struct jpeg_error_mgr error;
    FILE *const file = fopen(path, "wb");
    unsigned i;

    if(!file)
        return AIMG_LOADPNG_NO_FILE;

    if(setjmp(jmpbuf)){
        fclose(file);
#ifdef NDEBUG
        remove(path);
#endif
        return AIMG_LOADPNG_PNG_ERR;
    }

    jpeg_stdio_dest(&compress, file);
    
    jpeg_std_error(&error);
    error.error_exit = aimg_jpeg_error;

    compress.client_data = &jmpbuf;
    compress.err = &error;

    compress.image_width = from->w;
    compress.image_height = from->h;
    compress.input_components = 4;
    compress.in_color_space = JCS_EXT_RGBA;

    jpeg_set_defaults(&compress);
    jpeg_start_compress(&compress, 1);

    for(i = 0; i < from->h; i++){
        JSAMPLE *row = (JSAMPLE*)(from->pixels + (from->h * from->w));
        jpeg_write_scanlines(&compress, &row, 1);
    }

    jpeg_finish_compress(&compress);

    fflush(file);
    fclose(file);

    return AIMG_LOADPNG_SUCCESS;
}
