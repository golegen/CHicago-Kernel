// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:17 BRT
// Last edited on November 10 of 2018, at 19:12 BRT

#ifndef __CHICAGO_DISPLAY_H__
#define __CHICAGO_DISPLAY_H__

#include <chicago/types.h>

#define DispBootSplashImage (&_binary_splash_bmp_start)

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

#ifndef __CHICAGO_DISPLAY__
extern UInt8 DispFont[128][16];
#endif

extern UInt8 _binary_splash_bmp_start;

UIntPtr DispGetFrameBuffer(Void);
UInt8 DispGetBytesPerPixel(Void);
UIntPtr DispGetWidth(Void);
UIntPtr DispGetHeight(Void);
Void DispExtractARGB(UIntPtr c, PUInt8 a, PUInt8 r, PUInt8 g, PUInt8 b);
Void DispRefresh(Void);
Void DispClearScreen(UIntPtr c);
Void DispScrollScreen(IntPtr scale, UIntPtr c);
Void DispPutPixel(UIntPtr x, UIntPtr y, UIntPtr c);
Void DispDrawLine(UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c);
Void DispDrawRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void DispFillRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void DispDrawRoundedRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);
Void DispFillRoundedRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);
Void DispDrawBitmap(PUInt8 bmp, UIntPtr x, UIntPtr y);
Void DispWriteFormated(UIntPtr x, UIntPtr y, UIntPtr bg, UIntPtr fg, Const PChar data, ...);
Void DispIncrementProgessBar(Void);
Void DispFillProgressBar(Void);
Void DispDrawProgessBar(Void);
Void DispInit(UIntPtr w, UIntPtr h, UIntPtr bpp, UIntPtr fb);

#endif		// __CHICAGO_DISPLAY_H__
