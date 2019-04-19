// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:17 BRT
// Last edited on April 19 of 2019, at 11:30 BRT

#ifndef __CHICAGO_DISPLAY_H__
#define __CHICAGO_DISPLAY_H__

#include <chicago/img.h>

#define DispBootSplashImage (&_binary_splash_bmp_start)

extern UInt8 _binary_splash_bmp_start;

#ifndef __CHICAGO_DISPLAY__
extern PImage DispBackBuffer;
#endif

UIntPtr DispGetFrameBuffer(Void);
UInt8 DispGetBytesPerPixel(Void);
UIntPtr DispGetWidth(Void);
UIntPtr DispGetHeight(Void);
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
Void DispBitBlit(PImage src, UIntPtr srcx, UIntPtr srcy, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UInt8 mode);
Void DispIncrementProgessBar(Void);
Void DispFillProgressBar(Void);
Void DispDrawProgessBar(Void);
Void DispInit(UIntPtr w, UIntPtr h, UIntPtr bpp, UIntPtr fb);

#endif		// __CHICAGO_DISPLAY_H__
