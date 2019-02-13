// Copyright (c) 2018-2019 Alaskan Emily, Transnat Games
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "image.h"

#define UNICODE
#define UNICODE_

#include <assert.h>
#include <Windows.h>
#include <atlbase.h>
#include <wincodec.h>
#include <wincodecsdk.h>

// Implements aimg in terms of WIC (Windows Imaging Components).
// This is only intended for Vista+

class WICFactory {
public:
    static IWICImagingFactory* wic_factory;
    
    WICFactory(){
        CoInitialize(NULL);
        const HRESULT result = CoCreateInstance(CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory),
            (LPVOID*)&wic_factory);
        if(FAILED(result)){
            exit(result);
            wic_factory = NULL;
        }
    }
    
    ~WICFactory(){
        wic_factory->Release();
        CoUninitialize();
    }
};

IWICImagingFactory *WICFactory::wic_factory = NULL;
static WICFactory factory = WICFactory();

static unsigned aimg_load(struct AImg_Image *to, IWICBitmapDecoder &decoder){
    
    if(WICFactory::wic_factory == NULL)
       return 1;
    
    HRESULT result;
    CComPtr<IWICBitmapFrameDecode> frame;
    CComPtr<IWICFormatConverter> converter;
    
    if(FAILED(WICFactory::wic_factory->CreateFormatConverter(&converter)))
        return 1;
    
    if(FAILED(decoder.GetFrame(0, &frame)))
        return 1;
    
    if(sizeof(unsigned) == sizeof(UINT)){
        frame->GetSize(&(to->w), &(to->h));
    }
    else{
        UINT w, h;
        frame->GetSize(&w, &h);
        to->w = w;
        to->h = h;
    }
    
    if(FAILED(converter->Initialize(frame,
        GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone,
        NULL,
        0.0f,
        WICBitmapPaletteTypeCustom)))
        return 1;
    
    const size_t stride = to->w << 2,
        height = to->h;
    const size_t size = stride * height;
    
    to->pixels = (uint32_t*)malloc(size);
    if(converter->CopyPixels(NULL, stride, size, (BYTE*)(to->pixels)) != S_OK){
        free(to->pixels);
        return 1;
    }
    
    return 0;
}

static unsigned aimg_load_mem(struct AImg_Image *to, const void *data, unsigned size){
    // Create a stream that refers to the memory region.
    CComPtr<IWICStream> stream;
    CComPtr<IWICBitmapDecoder> decoder;
    
    // Create the stream
    if(FAILED(WICFactory::wic_factory->CreateStream(&stream)))
        return 1;
    
    // Set to be stream.
    if(FAILED(stream->InitializeFromMemory((BYTE*)data, size)))
        return 1;
    
    // Create the decoder.
    if(FAILED(WICFactory::wic_factory->CreateDecoderFromStream(stream, 0, WICDecodeMetadataCacheOnDemand, &decoder)))
        return 1;
    
    // Load from stream.
    return aimg_load(to, *decoder);
}

static unsigned aimg_load_file(struct AImg_Image *to, const char *path){
    CComPtr<IWICBitmapDecoder> decoder;
    
    // Convert the file name to UCS
    const size_t wide_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    
    wchar_t *const wide_string = (wchar_t *)malloc(sizeof(wchar_t) * wide_len);
    
    if(MultiByteToWideChar(CP_UTF8, 0, path, -1, wide_string, wide_len) == 0){
        free(wide_string);
        return 1;
    }
    
    // Create the decoder.
    const HRESULT result =
        WICFactory::wic_factory->CreateDecoderFromFilename(wide_string, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    
    free(wide_string);
    
    if(FAILED(result)){
        return 1;
    }
    else{
        return aimg_load(to, *decoder);
    }
}

static inline unsigned aimg_save(REFGUID format,
    IWICStream *stream,
    const struct AImg_Image *from){

    CComPtr<IWICBitmapEncoder> encoder;
    CComPtr<IWICBitmapFrameEncode> frame;
    
    // Create an encoder for this format
    if(FAILED(WICFactory::wic_factory->CreateEncoder(format, NULL, &encoder)))
        return 1;
    if(FAILED(encoder->Initialize(stream, WICBitmapEncoderNoCache)))
        return 1;
    if(FAILED(encoder->CreateNewFrame(&frame, NULL)))
        return 1;
    
    if(FAILED(frame->Initialize(NULL)))
        return 1;
    
    // Initialize the frame.
    const size_t lines = from->h, stride = from->w << 2;
    const size_t size = lines * stride;
    if(FAILED(frame->SetSize(from->w, from->h)))
        return 1;
    
    {
        WICPixelFormatGUID format_guid = GUID_WICPixelFormat32bppRGBA;
        if(FAILED(frame->SetPixelFormat(&format_guid)))
            return 1;
        // SetPixelFormat updates its arg to the actual format.
        // We don't really care if this format set wasn't applied.
    }
    
    if(FAILED(frame->WritePixels(lines, stride, size, (BYTE*)from->pixels)))
        return 1;
    
    // Commit the frame, then the encoder. This must be done in this order.
    if(FAILED(frame->Commit()))
        return 1;
    if(FAILED(encoder->Commit()))
        return 1;
    return 0;
}

static inline unsigned aimg_save_file(REFGUID format,
    const struct AImg_Image *from,
    const char *path){
    
    CComPtr<IWICStream> stream;
    
    // Create the stream
    if(FAILED(WICFactory::wic_factory->CreateStream(&stream))){
        puts("CreateStream failed");
        return 1;
    }
    
    // Convert the path.
    const size_t wide_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    
    wchar_t *const wide_string = (wchar_t *)malloc(sizeof(wchar_t) * wide_len);
    
    if(MultiByteToWideChar(CP_UTF8, 0, path, -1, wide_string, wide_len) == 0){
        free(wide_string);
        return 1;
    }
    
    
    // Set to be stream.
    const HRESULT result =
        stream->InitializeFromFilename(wide_string, GENERIC_WRITE);
    
    free(wide_string);
    
    if(FAILED(result)){
        puts("InitializeFromFilename failed");
        return 1;
    }
    
    return aimg_save(format, stream, from);
}

AIMG_API(unsigned)AImg_LoadPNGMem(struct AImg_Image *to, const void *data, unsigned size){
    return aimg_load_mem(to, data, size);
}

AIMG_API(unsigned)AImg_LoadPNG(struct AImg_Image *to, const char *path){
    return aimg_load_file(to, path);
}

AIMG_API(unsigned)AImg_SavePNG(const struct AImg_Image *from, const char *path){
    return aimg_save_file(GUID_ContainerFormatPng, from, path);
}

AIMG_API(unsigned)AImg_SaveJPG(const struct AImg_Image *from, const char *path){
    return aimg_save_file(GUID_ContainerFormatJpeg, from, path);
}
