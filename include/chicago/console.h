// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 15:24 BRT
// Last edited on October 27 of 2018, at 22:10 BRT

#ifndef __CHICAGO_CONSOLE_H__
#define __CHICAGO_CONSOLE_H__

#include <chicago/types.h>

Void ConSetRefresh(Boolean s);
Boolean ConGetRefresh(Void);
Void ConSetScale(IntPtr scale);
IntPtr ConGetScale(Void);
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
Void ConClearScreen(Void);
Void ConWriteCharacter(Char data);
Void ConWriteString(PChar data);
Void ConWriteInteger(UIntPtr data, UInt8 base);
Void ConWriteFormated(PChar data, ...);

#endif		// __CHICAGO_CONSOLE_H__
