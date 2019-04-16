// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:17 BRT
// Last edited on April 15 of 2019, at 22:26 BRT

#ifndef __CHICAGO_DISPLAY_H__
#define __CHICAGO_DISPLAY_H__

#include <chicago/types.h>

#define DispBootSplashImage (&_binary_splash_bmp_start)
#define DispFontStart (&_binary_font_psf_start)
#define DispFontEnd (&_binary_font_psf_end)

#define DISP_MODE_COPY 0x01
#define DISP_MODE_BLEND 0x02
#define DISP_MODE_INVERT 0x04

typedef struct {
	UInt8 b;
	UInt8 m;
	UInt32 size;
	UInt16 res1;
	UInt16 res2;
	UInt32 off;
} Packed BmpHeader, *PBmpHeader;

typedef struct {
	UInt32 size;
	UInt32 width;
	UInt32 height;
	UInt16 planes;
	UInt16 bit_count;
	UInt32 compression;
	UInt32 image_size;
	UInt32 xpels_per_meter;
	UInt32 ypels_per_meter;
	UInt32 clr_used;
	UInt32 clr_important;
} Packed BmpInfoHeader, *PBmpInfoHeader;

typedef struct {
	UInt32 magic;
	UInt32 version;
	UInt32 hdr_size;
	UInt32 flags;
	UInt32 num_glyph;
	UInt32 bytes_per_glyph;
	UInt32 height;
	UInt32 width;
} PCScreenFont, *PPCScreenFont;

extern UInt8 _binary_splash_bmp_start;
extern Char _binary_font_psf_start;
extern Char _binary_font_psf_end;

UIntPtr DispGetFrameBuffer(Void);
UInt8 DispGetBytesPerPixel(Void);
UIntPtr DispGetWidth(Void);
UIntPtr DispGetHeight(Void);
Void DispExtractARGB(UIntPtr c, PUInt8 a, PUInt8 r, PUInt8 g, PUInt8 b);
Void DispRefresh(Void);
Void DispClearScreen(UIntPtr c);
Void DispScrollScreen(UIntPtr c);
UIntPtr DispGetPixel(UIntPtr x, UIntPtr y);
Void DispPutPixel(UIntPtr x, UIntPtr y, UIntPtr c);
Void DispDrawLine(UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c);
Void DispDrawRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void DispFillRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void DispDrawRoundedRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);
Void DispFillRoundedRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);
Void DispCopy(UIntPtr src, UInt8 srcbpp, UIntPtr srcx, UIntPtr srcy, UIntPtr srcw, UIntPtr srch, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UInt8 mode);
Void DispDrawBitmap(PUInt8 bmp, UIntPtr x, UIntPtr y);
Void DispWriteFormated(UIntPtr x, UIntPtr y, UIntPtr bg, UIntPtr fg, Const PWChar data, ...);
Void DispIncrementProgessBar(Void);
Void DispFillProgressBar(Void);
Void DispDrawProgessBar(Void);
Void DispInit(UIntPtr w, UIntPtr h, UIntPtr bpp, UIntPtr fb);

#endif		// __CHICAGO_DISPLAY_H__
