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

#ifndef AIMG_EXPORT_H
#define AIMG_EXPORT_H
#pragma once

#if ( defined _MSC_VER ) || ( defined __WATCOMC__ )
#ifdef _WIN64
#define AIMG_CCALL
#else
#define AIMG_CCALL __cdecl
#endif
#ifdef AIMG_EXPORTS
#define AIMG_API(X) __declspec(dllexport) X AIMG_CCALL
#else
#define AIMG_API(X) __declspec(dllimport) X AIMG_CCALL
#endif
#else
#ifdef __CYGWIN__
#define AIMG_CCALL __cdecl
#else
#define AIMG_CCALL
#endif
#define AIMG_FASTCALL
#define AIMG_API(X) X AIMG_CCALL
#endif

#endif /* AIMG_EXPORT_H */
