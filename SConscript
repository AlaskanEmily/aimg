# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/ 

Import("environment")

src = [
    "image.c",
    "load_tga.c",
    "load_png.c",
    "save_png.c",
    "save_tga.c"
]

environment.Append(CPPDEFINES = ["AIMG_EXPORTS"])

aimg = environment.SharedLibrary("aimg", src, LIBS = ["png"])

Return("aimg")
