/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

#ifndef AIMG_MEMSET_PATTERN4_H
#define AIMG_MEMSET_PATTERN4_H
#pragma once

/* Provides memset_pattern4 on non-Apple platforms which don't have it. */

#ifndef __APPLE__

static void memset_pattern4(void *to, const void *pattern, unsigned long len){
    unsigned i;
    for(i = 0; i < len; i++)
        ((unsigned char *)to)[i] = ((unsigned char *)pattern)[i & 3];
}

#else

#include <string.h>

#endif

#endif /* AIMG_MEMSET_PATTERN4_H */
