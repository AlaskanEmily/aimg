/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

#include "image.h"
#include <stdio.h>

/* Loads and saves TGA files. Used for testing only */

int main(int argc, char *argv[]){
    if(argc<3){
        puts("Usage: tga_parse_test <infile> <outfile>");
        return 0;
    }
    else{
        struct AImg_Image img;
        if(AImg_LoadTGA(&img, argv[1])!=AIMG_LOADPNG_SUCCESS){
            fputs("Cannot load file: ", stdout);
            puts(argv[1]);
        }
        else if(AImg_SaveTGA(&img, argv[2])!=AIMG_LOADPNG_SUCCESS){
            fputs("Cannot save file: ", stdout);
            puts(argv[2]);
        }
        return 0;
    }
}
