// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 20:03 BRT
// Last edited on December 09 of 2018, at 17:12 BRT

#ifndef __CHICAGO_STRING_H__
#define __CHICAGO_STRING_H__

#include <chicago/types.h>

PVoid StrCopyMemory(PVoid dest, PVoid src, UIntPtr count);
PVoid StrCopyMemory24(PVoid dest, PVoid src, UIntPtr count);
PVoid StrCopyMemory32(PVoid dest, PVoid src, UIntPtr count);
PVoid StrSetMemory(PVoid dest, UInt8 val, UIntPtr count);
PVoid StrSetMemory24(PVoid dest, UInt32 val, UIntPtr count);
PVoid StrSetMemory32(PVoid dest, UInt32 val, UIntPtr count);
Boolean StrCompareMemory(PVoid m1, PVoid m2, UIntPtr count);
UIntPtr StrGetLength(PWChar str);
UIntPtr StrGetLengthC(PChar str);
Boolean StrCompare(PWChar dest, PWChar src);
Boolean StrCompareC(PChar dest, PChar src);
PWChar StrCopy(PWChar dest, PWChar src);
PChar StrCopyC(PChar dest, PChar src);
Void StrConcatenate(PWChar dest, PWChar src);
Void StrConcatenateC(PChar dest, PChar src);
PWChar StrTokenize(PWChar str, PWChar delim);
PChar StrTokenizeC(PChar str, PChar delim);
PWChar StrDuplicate(PWChar str);
PChar StrDuplicateC(PChar str);
Void StrUnicodeFromC(PWChar dest, PChar src, UIntPtr len);

#endif		// __CHICAGO_STRING_H__
