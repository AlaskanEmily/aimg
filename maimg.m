% Copyright (c) 2018-2019 Alaskan Emily, Transnat Games
%
% Permission to use, copy, modify, and distribute this software for any
% purpose with or without fee is hereby granted, provided that the above
% copyright notice and this permission notice appear in all copies.
%
% THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
% WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
% MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
% ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
% WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
% ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
% OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

:- module maimg.

%==============================================================================%
% Mercury bindings for AImg
:- interface.
%==============================================================================%

:- use_module io.

%------------------------------------------------------------------------------%

:- type texture.

%------------------------------------------------------------------------------%

:- func width(texture) = int.
:- func height(texture) = int.

%------------------------------------------------------------------------------%

:- func pixels(texture) = c_pointer.

%------------------------------------------------------------------------------%

:- type result ---> ok(texture) ; nofile ; badfile.

%------------------------------------------------------------------------------%

:- pred load(string::in, result::out, io.io::di, io.io::uo) is det.

%------------------------------------------------------------------------------%

:- pred load_io(string::in, io.res(texture)::out, io.io::di, io.io::uo) is det.

%------------------------------------------------------------------------------%

:- pred save(string::in, texture::in, io.res::out, io.io::di, io.io::uo) is det.

%------------------------------------------------------------------------------%

:- func empty = (texture::uo) is det.

%------------------------------------------------------------------------------%

:- type color.

%------------------------------------------------------------------------------%

:- pred pixel(texture::in, int::in, int::in, color::out) is semidet.

%------------------------------------------------------------------------------%
% Retrieves a color component as an integer in the range [0,255]
:- func r(color) = int.
:- func g(color) = int.
:- func b(color) = int.
:- func a(color) = int.

%------------------------------------------------------------------------------%
% Retrieves a color component as a float in the range [0.0,1.0]
:- func rf(color) = float.
:- func gf(color) = float.
:- func bf(color) = float.
:- func af(color) = float.

%==============================================================================%
:- implementation.
%==============================================================================%

:- use_module string.

%------------------------------------------------------------------------------%
% Used for constructing Mercury data structures from inline C
%------------------------------------------------------------------------------%

:- func create_nofile = result.
create_nofile = nofile.
:- pragma foreign_export("C", create_nofile = (out), "Aimg_Mercury_CreateNoFile").

%------------------------------------------------------------------------------%

:- func create_badfile = result.
create_badfile = badfile.
:- pragma foreign_export("C", create_badfile = (out),
    "Aimg_Mercury_CreateBadFile").

%------------------------------------------------------------------------------%

:- func create_ok(texture) = result.
create_ok(I) = ok(I).
:- pragma foreign_export("C", create_ok(in) = (out), "Aimg_Mercury_CreateOK").

%------------------------------------------------------------------------------%

create_io_ok = io.ok.
:- func create_io_ok = io.res.
:- pragma foreign_export("C", create_io_ok = (out), "Aimg_Mercury_CreateIOOK").

%------------------------------------------------------------------------------%

:- func create_io_error(string) = io.res.
create_io_error(Message) = io.error(io.make_io_error(Message)).
:- pragma foreign_export("C", create_io_error(in) = (out),
    "Aimg_Mercury_CreateIOError").

%------------------------------------------------------------------------------%

:- pragma foreign_decl("C", "
#include ""aimg/image.h""
void AImg_ImageFinalizer(void *image, void *unused);
").
:- pragma foreign_type("C", texture, "struct AImg_Image*").
:- pragma foreign_type("C", color, "uint32_t").

:- pragma foreign_code("C", "
void AImg_ImageFinalizer(void *image, void *unused){
    AImg_DestroyImage(image);
    (void)image;
}
").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", pixels(Image::in) = (Pix::out), 
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " Pix = (MR_Word)Image->pixels; ").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", width(Image::in) = (W::out), 
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " W = Image->w; ").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", height(Image::in) = (H::out), 
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " H = Image->h; ").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", load(Path::in, Result::out, IO0::di, IO1::uo), 
    [promise_pure, thread_safe, will_not_throw_exception],
    "
    IO1 = IO0;
    {
        struct AImg_Image im;
        const unsigned err = AImg_LoadAuto(&im, Path);
        if(err == AIMG_LOADPNG_SUCCESS){
            struct AImg_Image *const image = MR_GC_malloc_atomic(sizeof(struct AImg_Image));
            MR_GC_register_finalizer(image, AImg_ImageFinalizer, NULL);
            image->pixels = im.pixels;
            image->w = im.w;
            image->h = im.h;
            Result = Aimg_Mercury_CreateOK(image);
        }
        else if(err == AIMG_LOADPNG_NO_FILE){
            Result = Aimg_Mercury_CreateNoFile();
        }
        else{
            Result = Aimg_Mercury_CreateBadFile();
        }
    }
    ").

%------------------------------------------------------------------------------%

load_io(Path, Result, !IO) :-
    load(Path, TexResult, !IO),
    (
        TexResult = ok(Texture),
        Result = io.ok(Texture)
    ;
        TexResult = badfile,
        Err = string.append(Path, " is not valid"),
        Result = io.error(io.make_io_error(Err))
    ;
        TexResult = nofile,
        Err = string.append(Path, " does not exist"),
        Result = io.error(io.make_io_error(Err))
    ).

%------------------------------------------------------------------------------%


:- pragma foreign_proc("C", save(Path::in, Texture::in, Result::out, IO0::di, IO1::uo), 
    [promise_pure, thread_safe, will_not_throw_exception],
    "
    IO1 = IO0;
    {
        const unsigned err = AImg_SaveTGA(Texture, Path);
        if(err == AIMG_LOADPNG_SUCCESS){
            Result = Aimg_Mercury_CreateIOOK();
        }
        else{
            printf(""AIMG error for %s: %i\\n"", Path, err);
            Result = Aimg_Mercury_CreateIOError(""Could not save image"");
        }
    }
    ").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", empty = (Image::uo), 
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    "
        struct AImg_Image *const image = MR_GC_malloc_atomic(sizeof(struct AImg_Image));
        image->w = image->h;
        image->pixels = NULL;
        Image = image;
    ").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", pixel(Image::in, X::in, Y::in, Color::out), 
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    "
        SUCCESS_INDICATOR = X >= 0 && X < Image->w && Y >= 0 && Y < Image->h;
        if(SUCCESS_INDICATOR){
            Color = AImg_GetPixel(Image, X, Y);
        }
    ").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", r(Color::in) = (R::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " R = AImg_RawToR(Color); ").

:- pragma foreign_proc("C", g(Color::in) = (G::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " G = AImg_RawToG(Color); ").

:- pragma foreign_proc("C", b(Color::in) = (B::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " B = AImg_RawToB(Color); ").

:- pragma foreign_proc("C", a(Color::in) = (A::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " A = AImg_RawToA(Color); ").

%------------------------------------------------------------------------------%

:- pragma foreign_proc("C", rf(Color::in) = (R::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " R = ((float)AImg_RawToR(Color)) / 255.0f; ").

:- pragma foreign_proc("C", gf(Color::in) = (G::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " G = ((float)AImg_RawToG(Color)) / 255.0f; ").

:- pragma foreign_proc("C", bf(Color::in) = (B::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " B = ((float)AImg_RawToB(Color)) / 255.0f; ").

:- pragma foreign_proc("C", af(Color::in) = (A::out),
    [will_not_call_mercury, will_not_throw_exception,
     thread_safe, promise_pure, does_not_affect_liveness],
    " A = ((float)AImg_RawToA(Color)) / 255.0f; ").

