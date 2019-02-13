/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

#include "image.h"
#include <stdio.h>

/* Loads and saves image files.
 *
 * Can be used to re-encode files, especially TGA files as many codecs do not
 * support RLE, or can only decode and not encode RLE TGA (such as Corona).
 */

int main(int argc, char *argv[]){
    if(argc<3){
        puts("Usage: aimg_convert <infile> <outfile>");
        return 0;
    }
    else{
        struct AImg_Image img;
        if(AImg_LoadAuto(&img, argv[1])!=AIMG_LOADPNG_SUCCESS){
            fputs("Cannot load file: ", stdout);
            puts(argv[1]);
        }
        else if(AImg_SaveAuto(&img, argv[2])!=AIMG_LOADPNG_SUCCESS){
            fputs("Cannot save file: ", stdout);
            puts(argv[2]);
        }
        return 0;
    }
}
