// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 15:24 BRT
// Last edited on April 20 of 2019, at 10:27 BRT

#ifndef __CHICAGO_CONSOLE_H__
#define __CHICAGO_CONSOLE_H__

#include <chicago/img.h>

Void ConAcquireLock(Void);
Void ConSetSurface(PImage img, Boolean disp, Boolean free, UIntPtr x, UIntPtr y);
Void ConSetRefresh(Boolean s);
Boolean ConGetRefresh(Void);
Void ConSetCursorEnabled(Boolean e);
Void ConSetColor(UIntPtr bg, UIntPtr fg);
Void ConSetBackground(UIntPtr c);
Void ConSetForeground(UIntPtr c);
Void ConGetColor(PUIntPtr bg, PUIntPtr fg);
UIntPtr ConGetBackground(Void);
UIntPtr ConGetForeground(Void);
Void ConSetCursor(UIntPtr x, UIntPtr y);
Void ConSetCursorX(UIntPtr pos);
Void ConSetCursorY(UIntPtr pos);
Void ConGetCursor(PUIntPtr x, PUIntPtr y);
UIntPtr ConGetCursorX(Void);
UIntPtr ConGetCursorY(Void);
Void ConRefreshScreen(Void);
Void ConClearScreen(Void);
Void ConWriteCharacter(WChar data);
Void ConWriteString(PWChar data);
Void ConWriteInteger(UIntPtr data, UInt8 base);
Void ConWriteFormated(PWChar data, ...);

#endif		// __CHICAGO_CONSOLE_H__
