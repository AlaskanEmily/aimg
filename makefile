# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

AIMG_ENABLE_PNG?=y
AIMG_ENABLE_JPG?=y

CC?=gcc
AR?=ar
RANLIB?=ranlib
LINKER?=$(CC)
SHAREDFLAG?=-shared
LINKFLAGS?=-g
CFLAGS=-Wall -Wextra -pedantic -Wshadow -Werror -g -ansi -Wenum-compare -Os -fPIC

OBJECTS=image.o save_tga.o load_tga.o
LIBS=

.if "${AIMG_ENABLE_PNG}" == "y"
OBJECTS+=save_png.o load_png.o
LIBS+=-lpng
.endif

.if "${AIMG_ENABLE_JPG}" == "y"
OBJECTS+=save_jpeg.o
LIBS+=-ljpeg
.endif

all: libaimg.so libaimg.a

libaimg.so: $(OBJECTS) build_bufferfile
	$(LINKER) $(OBJECTS) $(LIBS) $(LINKFLAGS) $(SHAREDFLAG) bufferfile/libbufferfile.a -o libaimg.so

libaimg.a: $(OBJECTS)
	$(AR) rc x.a $(OBJECTS)
	$(RANLIB) x.a
	mv x.a libaimg.a

build_bufferfile:
	$(MAKE) -C bufferfile

clean:
	rm *.a *.o *.so || true

.PHONY: all clean build_bufferfile
